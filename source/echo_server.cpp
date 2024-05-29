#include "../sylar/tcp_server.h"
#include "../sylar/address.h"
#include "../sylar/log.h"
#include "../sylar/iomanager.h" 
#include "../sylar/socket.h"
#include "../sylar/bytearray.h"

#include <string.h> 
#include <vector> 
#include <memory>

sylar::Logger::ptr g_logger = SYLAR_LOG_ROOT();
class EchoServer : public sylar::TcpServer {
public:
	typedef std::shared_ptr<TcpServer> ptr;
	//type启动参数，文本形式或二进制形式等
	EchoServer(int type);
	void handleClient(sylar::Socket::ptr client);
private:
	//文本格式1, 二进制格式2
	int m_type = 0;
};

EchoServer::EchoServer(int type) 
	: m_type(type) {
}

void EchoServer::handleClient(sylar::Socket::ptr client) {
	SYLAR_LOG_INFO(g_logger) << "handleClient " << *client;
	sylar::ByteArray::ptr ba(new sylar::ByteArray);
	while (true) {
		ba->clear();
		std::vector<iovec> iovs;
		ba->getWriteBuffers(iovs, 1024);

		int rt = client->recv(&iovs[0], iovs.size());
		if (rt == 0) {
			SYLAR_LOG_INFO(g_logger) << "client close: " << *client;
			break;
		} else if (rt < 0) {
			SYLAR_LOG_ERROR(g_logger) << "client error rt=" << rt 
				<< " errno=" << errno << " errstr=" << strerror(errno);
			break;
		}	
		//因为getWriteBuffers不会修改m_size和m_position, 此处主动更新m_position
		ba->setPosition(ba->getPosition() + rt);
		// 设置0, 是为了toString
		ba->setPosition(0);

		SYLAR_LOG_INFO(g_logger) << std::string((char*)iovs[0].iov_base, rt);
		if (m_type == 1) {	//文本
			SYLAR_LOG_INFO(g_logger) << ba->toString();
		} else {
			SYLAR_LOG_INFO(g_logger) << ba->toHexString();
		}
	}
}

int type = 1;

void run() {
	EchoServer::ptr es(new EchoServer(type));	
	auto addr = sylar::Address::LookupAny("0.0.0.0:8020");
	while (!es->bind(addr)) {
		sleep(2);
	}
	es->start();
}

int main(int argc, char** argv) {
	if (argc < 2) {
		// -t 文本   -b 二进制
		SYLAR_LOG_INFO(g_logger) << "used as[" << argv[0] << " -t ] or [" 
			<< argv[0] << " -b]";
		return 0;
	}

	if (strcmp(argv[1], "-b") == 0) {
		type = 2;	
	} else {
		type = 1;
	}

	sylar::IOManager iom(2);
	iom.schedule(run);

	return 0;
}
