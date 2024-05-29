#include "tcp_server.h" 
#include "config.h"
#include "log.h"

#include <string.h>

namespace sylar {

static sylar::Logger::ptr g_logger = SYLAR_LOG_NAME("system");

static sylar::ConfigVar<uint64_t>::ptr g_tcp_server_recv_timeout = 
	sylar::Config::Lookup("tcp_server.recv_timeout", (uint64_t)(60 * 1000 * 2),
			"tcp server recv timeout");

TcpServer::TcpServer(sylar::IOManager* worker
				, sylar::IOManager* accept_worker) 
	: m_worker(worker)
	, m_acceptWorker(accept_worker)
	, m_recvTimeout()
	, m_name("sylar/1.0.0")
	, m_isStop(true) {
	
}

TcpServer::~TcpServer() {
	for (auto& i : m_socks) {
		i->close();
	}
	m_socks.clear();
}

bool TcpServer::bind(sylar::Address::ptr addr) {
	std::vector<Address::ptr> addrs;
	std::vector<Address::ptr> fails;
	addrs.push_back(addr);
	return bind(addrs, fails);
}

bool TcpServer::bind(const std::vector<Address::ptr>& addrs
						, std::vector<Address::ptr>& fails) {
	for (auto& addr : addrs) {
		Socket::ptr sock = Socket::CreateTCP(addr);
		if (!sock->bind(addr)) {
			SYLAR_LOG_ERROR(g_logger) << "bind fail errno=" 
				<< errno << " errstr=" << strerror(errno) 
				<< " addr=[" << addr->toString() << "]";
			//将bind失败的地址放入fails中， 继续bind其它地址
			fails.push_back(addr);
			continue;
		}
		if (!sock->listen()) {
			SYLAR_LOG_ERROR(g_logger) << "listen fail errno=" 
				<< errno << " errstr=" << strerror(errno) 
			    << " addr=[" << addr->toString() << "]";	
			fails.push_back(addr);
			continue;
		}
		m_socks.push_back(sock);
	}
	
	//fails不为空的时候，清空m_socks并返回false
	//服务器开机或重启有时候server启动慢，server句柄没释放掉，bind端口还占用就失败了
	//所以失败后，需要重新尝试启动
	if (!fails.empty()) {
		m_socks.clear();
		return false;
	}
	
	for (auto& i : m_socks) {
		SYLAR_LOG_INFO(g_logger) << "server bind success: " << *(i.get());
	}
	return true;
}

void TcpServer::startAccept(Socket::ptr sock) {
	while (!m_isStop) {
		Socket::ptr client = sock->accept();
		if (client) {
			client->setRecvTimeout(m_recvTimeout);
			m_worker->schedule(std::bind(&TcpServer::handleClient
						, shared_from_this(), client));
		} else {
			SYLAR_LOG_ERROR(g_logger) << "accept errno=" << errno 
				<< " errstr=" << strerror(errno);
		}
	}
}

bool TcpServer::start() {
	if (!m_isStop) {
		return true;
	}	
	m_isStop = false;
	for (auto& sock : m_socks) {
		m_acceptWorker->schedule(std::bind(&TcpServer::startAccept
							, shared_from_this(), sock));
	}
	return true;
}

void TcpServer::stop() {
	m_isStop = true;
	auto self = shared_from_this();
	m_acceptWorker->schedule([this, self] {
		for (auto& sock : m_socks) {
			sock->cancelAll();
			sock->close();
		}	
		m_socks.clear();
	});
}

void TcpServer::handleClient(Socket::ptr client) {
	SYLAR_LOG_INFO(g_logger) << "handleCilent: " << *client;
}

}
