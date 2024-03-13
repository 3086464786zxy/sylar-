#include "scheduler.h" 
#include "macro.h"
#include "log.h" 
#include "hook.h"
#include <iostream>

namespace sylar {
	
static sylar::Logger::ptr g_logger = SYLAR_LOG_NAME("system");	

static thread_local Scheduler* t_scheduler = nullptr;
static thread_local Fiber* t_fiber = nullptr;

Scheduler::Scheduler(size_t threads, bool use_caller, const std::string& name) {
	SYLAR_ASSERT(threads > 0);

	if (use_caller) {
		sylar::Fiber::GetThis();
		//调用线程加入线程池,线程池线程数减1
		--threads;

		//此时调度器为nullptr
		SYLAR_ASSERT(GetThis() == nullptr);
		t_scheduler = this;

		//新的线程的主协程，并不会参与到我们的协程调度中去
		m_rootFiber.reset(new Fiber(std::bind(&Scheduler::run, this), 0, true));	//使用call, back

		t_fiber = m_rootFiber.get();
		m_rootThread = GetThreadId(); 
		m_threadIds.push_back(m_rootThread);
	} else {
		m_rootThread = -1;	
	}
	m_threadCount = threads;
	m_name = name;
}

Scheduler::~Scheduler() {
	SYLAR_ASSERT(m_stopping);
	if (GetThis() == this) {
		t_scheduler = nullptr;
	}
} 

Scheduler* Scheduler::GetThis() {
	return t_scheduler;
} 

Fiber* Scheduler::GetMainFiber() {
	return t_fiber;
}

void Scheduler::start() {
	MutexType::Lock lock(m_mutex);
	//m_stopping默认是true, 当start后改变为false
	if (!m_stopping) {
		return;
	} 

	m_stopping = false;

	//线程池最初是未创建的，为空
	SYLAR_ASSERT(m_threads.empty());

	m_threads.resize(m_threadCount);
	//设置线程池中的线程，并将线程指向主协程指向的函数
	for (size_t i = 0; i < m_threadCount; ++i) {
		m_threads[i].reset(new Thread(std::bind(&Scheduler::run, this)
							, m_name + "_" + std::to_string(i))); 
		m_threadIds.push_back(m_threads[i]->getId());
	}
	lock.unlock();

	//if (m_rootFiber) {
	//	m_rootFiber->call();
	//	SYLAR_LOG_INFO(g_logger) << "call out " << m_rootFiber->getState();   
	//}
} 

void Scheduler::stop() {
	m_autoStop = true;
	if (m_rootFiber
			&& m_threadCount == 0 
			&& ((m_rootFiber->getState() == Fiber::TERM 
				|| m_rootFiber->getState() == Fiber::INIT))) {
		SYLAR_LOG_INFO(g_logger) << this << " stopped";
		m_stopping = true;	

		if (stopping()) {
			return;
		}
	}	

	//bool exit_on_this_fiber = false;
	if (m_rootThread != -1) {
		SYLAR_ASSERT(GetThis() == this);	//用了use_caller,一定要在创建scheduler的线程中调用stop
	} else {
		SYLAR_ASSERT(GetThis() != this);	//没有用use_caller,则可以在非它自己的线程中调用stop
	} 

	m_stopping = true;

	//有多少run就要执行多少个tickle
	for (size_t i = 0; i < m_threadCount; ++i) {
		tickle();	//线程唤醒
	} 

	if (m_rootFiber) {
		tickle();
	}

	//在stop调用前加入任务，加入任务在start()前后均可以
	if (m_rootFiber) {
		//当任务全部执行完退出
		//while (!stopping()) {
		//	if (m_rootFiber->getState() == Fiber::TERM 
		//			|| m_rootFiber->getState() == Fiber::EXCEP) {
		//		m_rootFiber.reset(new Fiber(std::bind(&Scheduler::run, this), 0, true));	//使用call, back
		//		SYLAR_LOG_INFO(g_logger) << "root fiber is term, reset";
		//		t_fiber = m_rootFiber.get();
		//	}	
		//	m_rootFiber->call();
		//}
		if (!stopping()) {
			m_rootFiber->call();
		}
	}

	std::vector<Thread::ptr> thrs;
	{
		MutexType::Lock lock(m_mutex);
		thrs.swap(m_threads);
	}

	for (auto& i : thrs) {
		i->join();
	}

	//if (exit_on_this_fiber) {
	//	
	//}
}

void Scheduler::setThis() {
	t_scheduler = this;
}

void Scheduler::run() {
	SYLAR_LOG_INFO(g_logger) << "run";
	//设置hook
	set_hook_enable(true);
	setThis();
	if (sylar::GetThreadId() != m_rootThread) {
		t_fiber = Fiber::GetThis().get();
	}

	Fiber::ptr idle_fiber(new Fiber(std::bind(&Scheduler::idle, this))); 	//什么都不干的时候使用的协程
	Fiber::ptr cb_fiber;

	FiberAndThread ft;
	while (true) {
		ft.reset();
		bool tickle_me = false;
		bool is_active = false;
		{
			MutexType::Lock lock(m_mutex);
			auto it = m_fibers.begin();
			while (it != m_fibers.end()) {
				if (it->thread != -1 && it->thread != sylar::GetThreadId()) {
					++it;
					tickle_me = true;		//需要通知其他线程
					continue;				
				} 

				SYLAR_ASSERT(it->fiber || it->cb);
				if (it->fiber && it->fiber->getState() == Fiber::EXEC) {
					++it;
					continue;
				}

				ft = *it;
				m_fibers.erase(it);
				++m_activeThreadCount;
				is_active = true;
				break;
			}
		}

		if (tickle_me) {
			tickle();		//唤醒其它线程 
		}

		if (ft.fiber && (ft.fiber->getState() != Fiber::EXCEP || ft.fiber->getState() != Fiber::TERM)) {
			ft.fiber->swapIn();
			--m_activeThreadCount;
			
			if (ft.fiber->getState() == Fiber::READY) {
				schedule(ft.fiber);
			} else if (ft.fiber->getState() != Fiber::TERM 
						&& ft.fiber->getState() != Fiber::EXCEP) {
				ft.fiber->m_state = Fiber::HOLD; 	//让出执行状态
			} 
			ft.reset();
		} else if (ft.cb) {
			if (cb_fiber) {
				cb_fiber->reset(ft.cb);
			} else {
				cb_fiber.reset(new Fiber(ft.cb));
				ft.cb = nullptr;
			}
			ft.reset();
			cb_fiber->swapIn();
			--m_activeThreadCount;
			if (cb_fiber->getState() == Fiber::READY) {
				schedule(cb_fiber);
				cb_fiber.reset();
			} else if (cb_fiber->getState() == Fiber::EXCEP 
						|| cb_fiber->getState() == Fiber::TERM) {
				cb_fiber->reset(nullptr);
			} else { //if (cb_fiber->getState() != Fiber::TERM) {
				cb_fiber->m_state = Fiber::HOLD;
				cb_fiber.reset();
			}
		} else {
			if (is_active) {
				--m_activeThreadCount;
				continue;
			}
			if (idle_fiber->getState() == Fiber::TERM) {
				SYLAR_LOG_INFO(g_logger) << "idle fiber term";
				break;
			}

			++m_idleThreadCount;
			idle_fiber->swapIn();
			--m_idleThreadCount;
			if (idle_fiber->getState() != Fiber::TERM 
					&& idle_fiber->getState() != Fiber::EXCEP) {
				idle_fiber->m_state = Fiber::HOLD;
			}
		}
	}
}

void Scheduler::tickle() {	
	SYLAR_LOG_INFO(g_logger) << "tickle";
}

bool Scheduler::stopping() {
	MutexType::Lock lock(m_mutex);
	return m_autoStop && m_stopping && m_fibers.empty() 
			&& m_activeThreadCount == 0;
}

void Scheduler::idle() {
	SYLAR_LOG_INFO(g_logger) << "idle";
	while (!stopping()) {
		sylar::Fiber::YieldToHold();
	}
} 

}
