#ifndef __SYLAR_SCHEDULAR_H__ 
#define __SYLAR_SCHEDULAR_H__ 

#include <memory> 
#include <vector> 
#include <list>
#include <atomic>
#include "fiber.h" 
#include "thread.h" 

namespace sylar {

class Scheduler {
public:
	typedef std::shared_ptr<Scheduler> ptr;
	typedef Mutex MutexType;

	//线程池默认线程数为1, use_caller将发起者的线程也加入到线程池中去
	Scheduler(size_t threads = 1, bool use_caller = true, const std::string& name = "");
	virtual ~Scheduler();
	
	const std::string& getName() const { return m_name; } 
	
	static Scheduler* GetThis();
	static Fiber* GetMainFiber();
	
	void start();
	void stop();	

	//need_tickle 需要告知
	template<class FiberOrCb> 
	void schedule(FiberOrCb fc, int thread = -1) {
		bool need_tickle = false;
		{
			MutexType::Lock lock(m_mutex);
			need_tickle = scheduleNoLock(fc, thread);
		} 

		if (need_tickle) {
			tickle();
		}
	}

	template<class InputIterator> 
	void schedule(InputIterator begin, InputIterator end) {
		bool need_tickle = false;
		{
			MutexType::Lock lock(m_mutex);
			while (begin != end) {
				need_tickle = scheduleNoLock(&*begin++, -1) || need_tickle;
			}
		}
		if (need_tickle) {
			tickle();
		}
	}

protected:
	virtual void tickle();
	void run();
	virtual bool stopping(); 	// 让子类清理任务机会
	virtual void idle();	//当协程没任务的时候，不让线程程终止，此时可以占据CPU或者sleep,让具体的实现子类来做

	void setThis();

	bool hasIdleThreads() { return m_idleThreadCount > 0; }
private:
	template<class FiberOrCb> 
	bool scheduleNoLock(FiberOrCb fc, int thread) {
		bool need_tickle = m_fibers.empty();
		FiberAndThread ft(fc, thread);
		if (ft.fiber || ft.cb) {
			m_fibers.push_back(ft);
		} 
		return need_tickle;
	}

	struct FiberAndThread {
		Fiber::ptr fiber;
		std::function<void()> cb;
		int thread;		//线程id 

		FiberAndThread(Fiber::ptr f, int thr) 	// 协程在哪个线程上跑的 
			: fiber(f), thread(thr) {} 

		FiberAndThread(Fiber::ptr* f, int thr) 	//注意: 使用swap(*f)，将f变为空指针， 引用减1, 避免f指向的智能指针引用一次, fiber再一次引用，导致协程释放出问题 
			: thread(thr) {
			fiber.swap(*f);
		}

		FiberAndThread(std::function<void()> f, int thr) 
			: cb(f), thread(thr) {
				
		}

		FiberAndThread(std::function<void()>* f, int thr) 
			: thread(thr) {
			cb.swap(*f);			
		}

		FiberAndThread() 
			: thread(-1) {
		}

		void reset() {
			fiber = nullptr;
			cb = nullptr;
			thread = -1;
		}
	};
private:
	MutexType m_mutex;
	std::vector<Thread::ptr> m_threads;
	std::list<FiberAndThread> m_fibers;
	Fiber::ptr m_rootFiber;
	std::string m_name;

protected:
	std::vector<int> m_threadIds;
	int m_rootThread;
	size_t m_threadCount = 0;
	std::atomic<size_t> m_activeThreadCount {0};		//活跃线程数量 
	std::atomic<size_t> m_idleThreadCount = {0};			//闲置线程数量  
	bool m_stopping = true;				//停止状态  
	bool m_autoStop = false;				//是否主动停止    
};

}

#endif
