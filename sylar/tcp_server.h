#ifndef __TCP_SERVER_H__
#define __TCP_SERVER_H__

#include <memory> 
#include <functional>
#include <vector>
#include <stdint.h>

#include "iomanager.h"
#include "socket.h"
#include "address.h"
#include "noncopyable.h"

namespace sylar {

class TcpServer : public std::enable_shared_from_this<TcpServer> 
					, Noncopyable {
public:
	typedef std::shared_ptr<TcpServer> ptr;
	//默认当前线程的iomanager
	TcpServer(sylar::IOManager* worker = sylar::IOManager::GetThis()
				, sylar::IOManager* accept_worker = sylar::IOManager::GetThis());
	virtual ~TcpServer(); 

	virtual bool bind(sylar::Address::ptr addr);
	//fails存放失败后的bind
	virtual bool bind(const std::vector<Address::ptr>& addrs
						, std::vector<Address::ptr>& fails);
	virtual bool start();
	virtual void stop();

	uint64_t getRecvTimeout() const { return m_recvTimeout; }
	std::string getName() const { return m_name; } 
	void setRecvTimeout(uint64_t v) { m_recvTimeout = v; } 
	void setName(const std::string& v) { m_name = v; } 

	bool isStop() const { return m_isStop; }
protected:
	//处理新连接的socket类
	virtual void handleClient(Socket::ptr client);	
	virtual void startAccept(Socket::ptr sock);

private:
	/// 监听Socket数组, 监听多个地址
	std::vector<Socket::ptr> m_socks;
	//充当线程池， 新连接的socket的调度器
	IOManager* m_worker;	
	IOManager* m_acceptWorker;
	//读超时时间
	uint64_t m_recvTimeout;
	//服务器名称(socket)
	std::string m_name; 
	//服务是否停止
	bool m_isStop;
};

}
#endif
