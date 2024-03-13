#ifndef __SYLAR_FIBER_H__ 
#define __SYLAR_FIBER_H__ 

#include <memory> 
#include <functional>
#include <ucontext.h> 
#include <thread.h> 

class Scheduler;
namespace sylar {

class Fiber : public std::enable_shared_from_this<Fiber> {
friend class Scheduler;
public:
	typedef std::shared_ptr<Fiber> ptr;
	
	enum State {
		INIT = 0,	//初始
		HOLD = 1,	//暂停状态
		EXEC = 2,	//执行
		TERM = 3,	//终止 
		READY = 4,  //可执行状态
		EXCEP = 5	//异常终止
	};
private:
	Fiber();

public:
	Fiber(std::function<void()> cb, size_t stacksize = 0, bool use_caller = false);
	~Fiber();

	// 重置协程函数，并重置状态
	// 当前协程执行完毕后，分配的内存还在，我们可以重置函数和功能，重新使用该协程，省掉内存的分配和释放 
	void reset(std::function<void()> cb);
	// 切换到当前协程执行
	void swapIn();
	// 切换到后台执行
	void swapOut();

	void call();
	void back();

	uint64_t getId() const { return m_id; }

	State const getState() const { return m_state; }

public: 
	//设置当前协程 
	static void SetThis(Fiber* f);
	// 返回当前执行的协程
	static Fiber::ptr GetThis();
	// 协程切换到后台，并且设置为Ready状态 
	static void YieldToReady(); 
	// 协程切换到后台，并且设置为Hold状态 
	static void YieldToHold(); 
	// 总协程数 
	static uint64_t TotalFibers();

	static void MainFunc(); 
	static void CallerMainFunc();
	static uint64_t GetFiberId();

private:
	uint64_t m_id = 0;
	uint32_t m_stacksize = 0;

	ucontext_t m_ctx;
	void* m_stack = nullptr;

	std::function<void()> m_cb;

	State m_state = INIT;
};

}
#endif 
