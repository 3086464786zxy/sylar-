#ifndef __SYLAR_TIMER_H 
#define __SYLAR_TIMER_H 

#include <memory> 
#include <vector> 
#include <set>
#include "thread.h" 

namespace sylar {

class TimerManager;
class Timer : public std::enable_shared_from_this<Timer> {
friend class TimerManager;
public:
	typedef std::shared_ptr<Timer> ptr;
	bool cancel(); 
	bool refresh(); 
	//from_now是否从当前时间开始计算
	bool reset(uint64_t ms, bool from_now); 
private:
	Timer(uint64_t ms, std::function<void()> cb, 
		  bool recurring, TimerManager* manager);	// cb回调函数 
	Timer(uint64_t next);
private: 
	bool m_recurring = false;		//是否循环定时器 
	uint64_t m_ms = 0;				//执行周期
	uint64_t m_next = 0;			//精确的执行时间
	std::function<void()> m_cb;
	TimerManager* m_manager = nullptr;
private:
	struct Comparator {
		bool operator () (const Timer::ptr& lhs, const Timer::ptr& rhs) const;
	};
};

class TimerManager {
friend class Timer;
public:
	typedef RWMutex RWMutexType;

	TimerManager();
	virtual ~TimerManager();

	Timer::ptr addTimer(uint64_t ms, std::function<void()> cb, 
					    bool recurring = false);
   
	//条件定时器， 智能指针作为条件，其本身有引用计数可作为条件是否存在
    Timer::ptr addConditionTimer(uint64_t ms, std::function<void()> cb, 
						std::weak_ptr<void> weak_cond,
						bool recurring = false); 	 
	uint64_t getNextTimer(); 
	void listExpiredCb(std::vector<std::function<void()>>& cbs);
	bool hasTimer();
protected:
	virtual void onTimerInsertAtFront() = 0;
	void addTimer(Timer::ptr val, RWMutexType::WriteLock& lock);
private:
	bool detectClockRollover(uint64_t now_ms);

private:
	RWMutexType m_mutex;
	std::set<Timer::ptr, Timer::Comparator> m_timers; 		//第二个参数传递一个比较结构体  
	bool m_tickled = false;
	uint64_t m_previousTime = 0;
};

}

#endif 
