#ifndef __SYLAR_HTTP_HTTP__SERVER_H_
#define __SYLAR_HTTP_HTTP_SERVER_H__

#include "../tcp_server.h"
#include "http_session.h" 
#include "servlet.h"

namespace sylar {
namespace http {

class HttpServer : public TcpServer {
public:
	typedef std::shared_ptr<HttpServer> ptr;
	
	//m_isKeepalive 是否长连接
	// worker: 工作调度器 
	// accept_worker: 接受连接调度器
	HttpServer(bool keepalive = false
				, sylar::IOManager* worker = sylar::IOManager::GetThis()
				, sylar::IOManager* accept_worker = sylar::IOManager::GetThis());
	ServletDispatch::ptr getServletDispatch() const { return m_dispatch; }
	void setServletDispatch(ServletDispatch::ptr v) { m_dispatch = v; }
protected:
	virtual void handleClient(Socket::ptr client) override;
private:
	bool m_isKeepalive;
	ServletDispatch::ptr m_dispatch;
};

}
}

#endif
