#ifndef __SYLAR_SOCKET_H__ 
#define __SYLAR_SOCKET_H__ 

#include <memory> 
#include <stdint.h>
#include "address.h" 

namespace sylar {

class Socket : public std::enable_shared_from_this<Socket>, Noncopyable {
public:
	typedef std::shared_ptr<Socket> ptr;
	typedef std::weak_ptr<Socket> weak_ptr;
	
	Socket(int family, int type, int protocol = 0);
	~Socket();

	int64_t getSendTimeout();
	void setSendTimeout(int64_t v);

	int64_t getRecvTimeout();
	void setRecvTimeout(int64_t v);

	bool getOption(int level, int option, void* result, size_t* len);
	template<class T> 
	bool getOption(int level, int option, T& result) {
		size_t length = sizeof(T);
		return getOption(level, option, &result, &length);
	}

	bool setOption(int level, int option, const void* result, size_t len);
	template<class T> 
	bool setOption(int level, int option, const T& value) {
		return setOption(level, option, &value, sizeof(T));	
	} 

	Socket::ptr accept();
	
	bool bind(const Address::ptr addr);
	bool connect(const Address::ptr addr, uint64_t timeout_ms = -1);
	bool listen(int backlob = SOMAXCONN);
	bool close();

	int send(const void* buffer, size_t length, int flags = 0);
	int send(const iovec* buffers, size_t length, int flags = 0);
	int sendTo(const void* buffer, size_t length, const Address::ptr to, int flags = 0);
	int sendTo(const iovec* buffers, size_t length, int flags = 0);

	int recv(void* buffer, size_t length, int flags = 0);
	int recv(void* buffer, size_t length, int flags = 0);
	int recvFrom(void* buffer, size_t length, Address::ptr from, int flags = 0);
	int recvFrom(void* buffer, size_t length, Address::ptr from, int flags = 0);

	//获取远程连接的地址
	Address::ptr getRemoteAddress();
	//获取本地IP 
	Address::ptr getLocalAddress(); 

	int getFamily() const;
	int getType() const; 
	int getProtocol() const;

	bool isConnected() const;
	bool isValid() const; 
	int getError();

	std::ostream& dump(std::ostream& os) const;
	//返回socket句柄
	int getSocket() const;
	

private: 
		
}
}
