#ifndef __SYLAR_IOMANAGER_H__ 
#define __SYLAR_IOMANAGER_H__ 

#include "scheduler.h" 
#include "timer.h" 

namespace sylar {

class IOManager : public Scheduler, public TimerManager {
public:
	typedef std::shared_ptr<IOManager> ptr;
	typedef RWMutex RWMutexType;

	enum Event {
		NONE 	= 0x0,
		READ 	= 0x1,	//读事件 EPOLLIN
		WRITE	= 0x4,	//写事件 EPOLLOUT
	};	
private: 
	struct FdContext {
		typedef Mutex MutexType;
		struct EventContext {
			Scheduler* scheduler = nullptr;		//事件执行的scheduler Fiber::ptr fiber;
			Fiber::ptr fiber;			//事件的协程
			std::function<void()> cb;	//时间的回调函数
		};

		EventContext& getContext(Event event);
		void resetContext(EventContext& ctx);	
		void triggerEvent(Event event);

		EventContext read;			//读事件
		EventContext write;			//写事件
		int fd = 0;					//事件关联的句柄
		Event events = NONE;		//事件关联的句柄
		MutexType mutex;
	};

public:
	IOManager(size_t threads = 1, bool use_caller = true, const std::string& name = "");
	~IOManager();
	
	//0 success, -1 error
	int addEvent(int fd, Event event, std::function<void()> cb = nullptr);
	bool delEvent(int fd, Event event);
	bool cancelEvent(int fd, Event event);

	bool cancelAll(int fd);

	static IOManager* GetThis();
	
protected:
	void tickle() override;
	bool stopping() override;
	bool stopping(uint64_t& timeout);
	void idle() override;

	void contextResize(size_t size);

	void onTimerInsertAtFront() override;
private:
	int m_epfd = 0;
	int m_tickleFds[2];

	//事件数
	std::atomic<size_t> m_pendingEventCount = { 0 };
	RWMutexType m_mutex;
	std::vector<FdContext*> m_fdContexts;
};

}

#endif 
