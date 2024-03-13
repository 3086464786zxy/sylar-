#ifndef __SYLAR_ADDRESS_H__ 
#define __SYLAR_ADDRESS_H__ 

#include <memory> 
#include <string> 
#include <sys/socket.h> 
#include <sys/types.h> 
#include <arpa/inet.h>
#include <iostream>  
#include <unistd.h>
#include <sstream>
#include <sys/un.h>
#include <vector>
#include <map>
#include <ifaddrs.h>

namespace sylar {

class IPAddress;
class Address {
public:
	typedef std::shared_ptr<Address> ptr;

	static Address::ptr Create(const sockaddr* address, uint32_t port = 0);
	//域名转IP
	static bool Lookup(std::vector<Address::ptr>& result, const std::string& host, 
					int family = AF_UNSPEC,	int type = 0, int protocol = 0); 
	static Address::ptr LookupAny(const std::string& host, 
					int family = AF_UNSPEC,	int type = 0, int protocol = 0); 
	static std::shared_ptr<IPAddress> LookupAnyIPAddress(const std::string& host, 
					int family = AF_UNSPEC,	int type = 0, int protocol = 0); 

	static bool GetInterfaceAddresses(std::multimap<std::string 
					,std::pair<Address::ptr, uint32_t>>& result 
					,int family = AF_UNSPEC);
	static bool GetInterfaceAddresses(std::vector<std::pair<Address::ptr, uint32_t>>& result
					,const std::string& iface, int family = AF_UNSPEC);

	virtual ~Address() {} 

	int getFamily() const;

	virtual const sockaddr* getAddr() const = 0;
	virtual socklen_t getAddrLen() const = 0;
	
	// 序列化函数
	virtual std::ostream& insert(std::ostream& os) const = 0;
	std::string toString();

	bool operator < (const Address& rhs) const;
	bool operator == (const Address& rhs) const;
	bool operator != (const Address& rhs) const;
};

class IPAddress : public Address {
public:
	typedef std::shared_ptr<IPAddress> ptr;

	static IPAddress::ptr Create(const char* address, uint32_t port = 0);

	// 获得IP地址的广播地址
	virtual IPAddress::ptr broadcastAddress(uint32_t prefix_len) = 0;
	// 获得IP地址的网络地址
	virtual IPAddress::ptr networdAddress(uint32_t prefix_len) = 0;
	// 或得子网演码 
	virtual IPAddress::ptr subnetMask(uint32_t prefix_len) = 0;
		
	// 端口号
	virtual uint32_t getPort() const = 0;
	virtual void setPort(uint32_t v) = 0;
};

class IPv4Address : public IPAddress {
public:
	typedef std::shared_ptr<IPv4Address> ptr;

	static IPv4Address::ptr Create(const char* address, uint32_t port = 0);

	IPv4Address(const sockaddr_in& address);
	IPv4Address(uint32_t address = INADDR_ANY, uint32_t port = 0);

	const sockaddr* getAddr() const override;
	socklen_t getAddrLen() const override; 
	std::ostream& insert(std::ostream& os) const override;

	// 获得IP地址的广播地址
	IPAddress::ptr broadcastAddress(uint32_t prefix_len) override;
	// 获得IP地址的网络地址
	IPAddress::ptr networdAddress(uint32_t prefix_len) override;
	// 获得子网演码 
	IPAddress::ptr subnetMask(uint32_t prefix_len) override;
		
	// 端口号
	uint32_t getPort() const override;
	void setPort(uint32_t v) override;
public:
	sockaddr_in m_addr;
};

class IPv6Address : public IPAddress {
public:
	typedef std::shared_ptr<IPv6Address> ptr;

	static IPv6Address::ptr Create(const char* address, uint32_t port = 0);

	IPv6Address();
	IPv6Address(const sockaddr_in6& addr);
	IPv6Address(const uint8_t address[16], uint32_t port = 0);

	const sockaddr* getAddr() const override;
	socklen_t getAddrLen() const override; 
	std::ostream& insert(std::ostream& os) const override;

	// 获得IP地址的广播地址
	IPAddress::ptr broadcastAddress(uint32_t prefix_len) override;
	// 获得IP地址的网络地址
	IPAddress::ptr networdAddress(uint32_t prefix_len) override;
	// 或得子网演码 
	IPAddress::ptr subnetMask(uint32_t prefix_len) override;
		
	// 端口号
	uint32_t getPort() const override;
	void setPort(uint32_t v) override;
public:
	sockaddr_in6 m_addr;

};

class UnixAddress : public Address {
public:
	typedef std::shared_ptr<UnixAddress> ptr;
	UnixAddress();
	UnixAddress(const std::string& path);

	const sockaddr* getAddr() const override;
	socklen_t getAddrLen() const override; 
	std::ostream& insert(std::ostream& os) const override;
private:
	struct sockaddr_un m_addr; 
	socklen_t m_length;
};

class UnKnowAddress : public Address {
public:
	typedef std::shared_ptr<UnKnowAddress> ptr;
	
	UnKnowAddress(int family);
	UnKnowAddress(const sockaddr& addr);
	const sockaddr* getAddr() const override;
	socklen_t getAddrLen() const override;
	std::ostream& insert(std::ostream& os) const override;
private:
	sockaddr m_addr;
};
}
#endif
