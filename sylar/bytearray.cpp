#include "bytearray.h" 
#include "endian1.h"
#include <fstream> 
#include <sstream>
#include <cmath>
#include <iomanip>
#include <string.h>
#include "log.h" 

namespace sylar {

static sylar::Logger::ptr g_logger = SYLAR_LOG_ROOT();

ByteArray::Node::Node(size_t s) 
	: ptr(new char[s])  
	, next(nullptr) 
	, size(s) {	
}

ByteArray::Node::Node() 
	: ptr(nullptr) 
	, next(nullptr) 
	, size(0) {
}

ByteArray::Node::~Node() {
	if (ptr) {
		delete[] ptr;
	}
}

ByteArray::ByteArray(size_t base_size ) 
	: m_baseSize(base_size)
    , m_position(0) 
    , m_capacity(base_size) 
	, m_size(0)
	, m_endian(SYLAR_BIG_ENDIAN)
	, m_root(new Node(base_size)) 
	, m_cur(m_root) {	//网络字节序一般为大端
}

ByteArray::~ByteArray() {
	Node* temp = m_root;
	while (temp) {
		m_cur = temp;
		temp = temp->next;
		delete m_cur;
	}
}

bool ByteArray::isLittleEndian() const {
	return m_endian == SYLAR_LITTLE_ENDIAN;
}

void ByteArray::setIsLittleEndian(bool val) {
	if (val) {
		m_endian = SYLAR_LITTLE_ENDIAN;
	} else {
		m_endian = SYLAR_BIG_ENDIAN;
	}
}

void ByteArray::writeFint8(int8_t value) {	//1个字节不需要转换网络序列
	write(&value, sizeof(value));
}

void ByteArray::writeFuint8(uint8_t value) {
	write(&value, sizeof(value));
}

void ByteArray::writeFint16(int16_t value) {
	if (m_endian != SYLAR_BYTE_ORDER) {		//当前字节序列与主机不相同则要转换
		value = byteswap(value);
	}
	write(&value, sizeof(value));
}

void ByteArray::writeFuint16(uint16_t value) {
	if (m_endian != SYLAR_BYTE_ORDER) {
		value = byteswap(value);
	}
	write(&value, sizeof(value));
}

void ByteArray::writeFint32(int32_t value) {
	if (m_endian != SYLAR_BYTE_ORDER) {
		value = byteswap(value);
	}
	write(&value, sizeof(value));
}

void ByteArray::writeFuint32(uint32_t value) {
	if (m_endian != SYLAR_BYTE_ORDER) {
		value = byteswap(value);
	}
	write(&value, sizeof(value));
}

void ByteArray::writeFint64(int64_t value) {
	if (m_endian != SYLAR_BYTE_ORDER) {
		value = byteswap(value);
	}
	write(&value, sizeof(value));
}

void ByteArray::writeFuint64(uint64_t value) {
	if (m_endian != SYLAR_BYTE_ORDER) {
		value = byteswap(value);
	}
	write(&value, sizeof(value));
}

//这个过程的目的是将负数编码为奇数，从而避免与非负数的编码冲突。
//例如，-1 编码为 1，-2 编码为 3，依此类推。
//非负数通过乘以 2 被编码为偶数，这样就不会与负数的编码冲突。
//例如，0 编码为 0，1 编码为 2，2 编码为 4，依此类推。
static uint32_t EncodeZigzag32(const int32_t& v) {
	if (v < 0) {
		return ((uint32_t)(-v)) * 2 - 1;
	} else {
		return v * 2;
	}
}

static uint64_t EncodeZigzag64(const int64_t& v) {
	if (v < 0) {
		return ((uint64_t)(-v)) * 2 - 1;
	} else {
		return v * 2;
	}
}

static int32_t DecodeZigzag32(const uint32_t& v) {
	// v >> 1 相当与v / 2 
	// v & 1 转换的最后一位: 负数为1, 偶数为0
	// ^异或 -将有符号数边为有符号数
	//-：这是一个取反操作。对于 int64_t 类型，取反会将正数变为负数，将负数变为正数。
	//因为(v & 1) 的结果要么是0要么是1，取反后就会得到0或-1。
	// v ^ -1  (-1: 111111111...)  eg:(2) ^ -(v & 1) == -3 因为v >> 1为偶数，最低位0^1 = 1, 正好加1, 而最高位0^1=1,为负号
	// v ^ 0 = v
	return (v >> 1) ^ -(v & 1);
}

static int64_t DecodeZigzag64(const uint64_t& v) {
	return (v >> 1) ^ -(v & 1);
}

void ByteArray::writeInt32(int32_t value) {
	writeUint32(EncodeZigzag32(value));
}

void ByteArray::writeUint32(uint32_t value) {
	uint8_t tmp[5];		//最高压缩为5字节 
	uint8_t i = 0;
	while (value >= 0x80) { 	//大于一字节 1000 0000
		tmp[i++] =	(value & 0x7f) | 0x80;  	// 存储低7位并添加标记位 
		value >>= 7;							// 右移7位 
	}
	tmp[i++] = value;
	write(tmp, i);
}

void ByteArray::writeInt64(int64_t value) {
	writeUint64(EncodeZigzag64(value));		
}

void ByteArray::writeUint64(uint64_t value) {
	uint8_t tmp[10];
	uint8_t i = 0;
	while (value >= 0x80) {
		tmp[i++] = (value & 0x7f) | 0x80; //最高位为1, 读取时要删除 &0x70
		value >>= 7;
	}
	tmp[i++] = value;
	write(tmp, i);
}

void ByteArray::writeFloat(float value) {
	uint32_t v;
	//使用memcpy函数将float变量value的内存内容复制到uint32_t变量v中。
	//这实际上是将float值转换为其二进制表示形式。
	memcpy(&v, (char*)&value, sizeof(value));
	writeFuint32(v);
}

void ByteArray::writeDouble(float value) {
	uint64_t v;
	memcpy(&v, (char*)&value, sizeof(value));
	writeFuint64(v);
}

void ByteArray::writeStringF16(const std::string& value) {
	writeFuint16(value.size());					//先写string的size
	write(value.c_str(), value.size());
}

void ByteArray::writeStringF32(const std::string& value) {
	writeFuint32(value.size());
	write(value.c_str(), value.size());	
}

void ByteArray::writeStringF64(const std::string& value) {
	writeFuint64(value.size());
	write(value.c_str(), value.size());
}

void ByteArray::writeStringVint(const std::string& value) {
	writeFuint64(value.size());
	write(value.c_str(), value.size());	
}

void ByteArray::writeStringWithoutLength(const std::string& value) {
	write(value.c_str(), value.size());
}

int8_t 	 ByteArray::readFint8() {
	uint8_t v;
	read(&v, sizeof(v));
	return v;
}

uint8_t  ByteArray::readFuint8() {
	uint8_t v;
	read(&v, sizeof(v));
	return v;	
}

#define XX(type) \
	type v; \
	read(&v, sizeof(v)); \
	if (m_endian == SYLAR_BYTE_ORDER) { \
		return v; \
	} else { \
		return byteswap(v); \
	}

int16_t  ByteArray::readFint16() {
	XX(int16_t);
}

uint16_t ByteArray::readFuint16() {
	XX(uint16_t);
}

int32_t  ByteArray::readFint32() {
	XX(int32_t);
}

uint32_t ByteArray::readFuint32() {
	XX(uint32_t);
}

int64_t  ByteArray::readFint64() {
	XX(int64_t);
}

uint64_t ByteArray::readFuint64() {
	XX(uint64_t);
}

#undef XX 

int32_t  ByteArray::readInt32() {
	return DecodeZigzag32(readUint32());
}

uint32_t ByteArray::readUint32() {
	uint32_t result = 0;				//初始化结果变量为0，用于存储解码后的uint32_t值  
	for (int i = 0; i < 32; i += 7) { 	//循环32位，每次处理7位  
		uint8_t b = readFuint8();		//每次读7一个字节 
		if (b < 0x80) {					//如果字节的最高位不是1（即不是继续标志）
			result |= ((uint32_t)b) << i; 	//将读取的字节左移i位后，与结果变量进行或运算，更新结果  
			break;						// 跳出循环，因为已经读取了完整的uint32_t值  
		} else { // 如果字节的最高位是1，则先清除最高位（作为继续标志），再左移i位后，与结果变量进行或运算	
				 result |= (((uint32_t)(b & 0x7f)) << i);
		}
	}
	return result;
}

int64_t  ByteArray::readInt64() {
	return DecodeZigzag64(readUint64());
}

uint64_t ByteArray::readUint64() {
	uint64_t result = 0;				//初始化结果变量为0，用于存储解码后的uint32_t值  
	for (int i = 0; i < 64; i += 7) { 	//循环32位，每次处理7位  
		uint8_t b = readFuint8();		//每次读7一个字节 
		if (b < 0x80) {					//如果字节的最高位不是1（即不是继续标志）
			result |= ((uint64_t)b) << i; 	//将读取的字节左移i位后，与结果变量进行或运算，更新结果  
			break;						// 跳出循环，因为已经读取了完整的uint32_t值  
		} else { // 如果字节的最高位是1，则先清除最高位（作为继续标志），再左移i位后，与结果变量进行或运算	
				 result |= (((uint64_t)(b & 0x7f)) << i);
		}
	}
	return result;
}

float 	 ByteArray::readFloat() {
	uint32_t v = readFuint32();
	float value;
	memcpy(&value, (char*)&v, sizeof(v));
	return value;
}

double 	 ByteArray::readDouble() {
	uint64_t v = readFuint64();
	double value;
	memcpy(&value, (char*)&v, sizeof(v));
	return value;
}

std::string ByteArray::readStringF16() {
	uint16_t len = readFuint16();
	std::string buff;
	buff.resize(len);
	read(&buff[0], len);
	return buff;	
}

std::string ByteArray::readStringF32() {
	uint32_t len = readFuint32();
	std::string buff;
	buff.resize(len);
	read(&buff[0], len); 
	return buff;
}

std::string ByteArray::readStringF64() {
	uint64_t len = readFuint64();
	std::string buff;
	buff.resize(len);
	read(&buff[0], len);
	return buff;
}

std::string ByteArray::readStringVint() {
	uint64_t len = readFuint64();
	std::string buff;
	buff.resize(len);
	read(&buff[0], len);
	return buff;
}

void ByteArray::clear() {
	m_position = m_size = 0;
	m_capacity = m_baseSize;
	Node* tmp = m_root->next;
	while (tmp) {
		m_cur = tmp;
		tmp = tmp->next;
		delete m_cur;
	}
	m_cur = m_root;
	m_root->next = nullptr;
}

void ByteArray::write(const void* buf, size_t size) {
	if (size == 0) {
		return;
	}
	addCapacity(size);

	size_t npos = m_position % m_baseSize;		//当前节点内存块的位置 
	size_t ncap = m_cur->size - npos;			//当前节点剩下的容量
	size_t bpos = 0;  							//写数据的当前位置
	
	while (size > 0) {
		if (ncap >= size) {
			memcpy((char*)m_cur->ptr + npos, (const char*)buf + bpos, size);
			if (m_cur->size == npos + size) {
				m_cur = m_cur->next;
			}
			m_position += size;
			bpos += size;
			size = 0;
		} else {
			memcpy((char*)m_cur->ptr + npos, (const char*)buf + bpos, ncap);
			m_position += ncap;
			bpos += ncap;
			size -= ncap;
			m_cur = m_cur->next;
			ncap = m_cur->size;
			npos = 0;
		}
	}

	if (m_position > m_size) {
		m_size = m_position;
	}
}

void ByteArray::read(void* buf, size_t size) {
	if (size > getReadSize()) {		
		throw std::out_of_range("not enough len");
	} 

	size_t npos = m_position % m_baseSize;
	size_t ncap = m_cur->size - npos; 
	size_t bpos = 0;
	while (size > 0) {
		if (ncap >= size) {		//read: 当前节点中有size长度的容量可读, 不足则再读下一个节点
			memcpy((char*)buf + bpos, (const char*)m_cur->ptr + npos, size);
			if (m_cur->size == npos + size) {
				m_cur = m_cur->next;
			}
			m_position += size;
			bpos += size;
			size = 0;
		} else {
			memcpy((char*)buf + bpos, (const char*)m_cur->ptr + npos, ncap);
			m_position += ncap;
			bpos += ncap;
			size -= ncap;
			m_cur = m_cur->next;
			ncap = m_cur->size;
			npos = 0;
		}
	}
} 

void ByteArray::read(void* buf, size_t size, size_t position) const {
	if (size > getReadSize()) {		
		throw std::out_of_range("not enough len");
	} 

	size_t npos = position % m_baseSize;
	size_t ncap = m_cur->size - npos; 
	size_t bpos = 0;
	Node* cur = m_cur;
	while (size > 0) {
		if (ncap >= size) {		//read: 当前节点中有size长度的容量可读, 不足则再读下一个节点
			memcpy((char*)buf + bpos, (char*)cur->ptr + npos, size);
			if (cur->size == npos + size) {
				cur = m_cur->next;
			}
			position += size;
			bpos += size;
			size = 0;
		} else {
			memcpy((char*)buf + bpos, (char*)cur->ptr + npos, ncap);
			position += ncap;
			bpos += ncap;
			size -= ncap;
			cur = cur->next;
			ncap = cur->size;
			npos = 0;
		}
	}
}

void ByteArray::setPosition(size_t v) {
	if (v > m_capacity) {
		throw std::out_of_range("set_position out of range");
	}
	m_position = v;
	if (m_position > m_size) {
		m_size = m_position;
	}
	m_cur = m_root;
	while (v > m_cur->size) {
		v -= m_cur->size;	
		m_cur = m_cur->next;
	}
	if (v == m_cur->size) {
		m_cur = m_cur->next;
	}
}

bool ByteArray::writeToFile(const std::string& name) const {
	std::ofstream ofs;
	ofs.open(name, std::ios::trunc | std::ios::binary);		//turnc覆盖写入 
	if (!ofs) {
		SYLAR_LOG_ERROR(g_logger) << "writeToFile name=" << name 
			<< " error, errno=" << errno << " errstr=" << strerror(errno);
		return false;
	}	

	int64_t read_size = getReadSize();
	int64_t pos = m_position;
	Node* cur = m_cur;

	while (read_size > 0) {
		int diff = pos % m_baseSize;
		int64_t len = (read_size > (int64_t)m_baseSize ? m_baseSize : read_size) - diff;
		ofs.write(cur->ptr + diff, len);
		cur = cur->next;
		pos += len;
		read_size -= len;
	}

	return true;
}

bool ByteArray::readFromFile(const std::string& name) {
	std::ifstream ifs;
	ifs.open(name, std::ios::binary);
	if (!ifs) {
		SYLAR_LOG_ERROR(g_logger) << "readFromFile name=" << name 
			<< " error, errno=" << errno << " errstr=" << strerror(errno);
		return false;
	}

	std::shared_ptr<char> buff(new char[m_baseSize], [](char* ptr) { delete[] ptr;});
	while (!ifs.eof()) {
		ifs.read(buff.get(), m_baseSize);
		write(buff.get(), ifs.gcount());
	}
	return true;
}

void ByteArray::addCapacity(size_t size) {
	if (size == 0) {
		return;
	}
	size_t old_cap = getCapacity();
	if (old_cap >= size) {
		return;
	}

	size -= old_cap;
	size_t count = ceil(1.0 * size / m_baseSize);
	Node* tmp = m_root;
	while (tmp->next) {
		tmp = tmp->next;
	}

	Node* first = NULL;
	for (size_t i = 0; i < count; ++i) {
		tmp->next = new Node(m_baseSize);
		if (first == NULL) {
			first = tmp->next;
		}
		tmp = tmp->next;
		m_capacity += m_baseSize;
	}
	
	if (old_cap == 0) {
		m_cur = first;
	}
}

std::string ByteArray::toString() const {
	std::string str;
	str.resize(getReadSize());
	if (str.empty()) {
		return str;
	}
	read(&str[0], str.size(), m_position);
	return str;
}

std::string ByteArray::toHexString() const {
	std::string str = toString();
	std::stringstream ss;

	for (size_t i = 0; i < str.size(); i++) {
		if (i > 0 && i % 32 == 0) {
			ss << std::endl;
		}
		ss << std::setw(2) << std::setfill('0') << std::hex 
			<< (int)(uint8_t)str[i] << " ";				//(int)(uint8_t)输出每个字符的ASCII码值
	}

	return ss.str();
}

int32_t  ByteArray::getReadBuffers(std::vector<iovec>& buffers, uint64_t len) const {
	len = len > getReadSize() ? getReadSize() : len;
	if (len == 0) {
		return 0;	
	}

	uint64_t size = len;

	size_t npos = m_position % m_baseSize;
	size_t ncap = m_cur->size - npos;
	struct iovec iov;
	Node* cur = m_cur;

	while (len > 0) {
		if (ncap >= len) {
			iov.iov_base = cur->ptr + npos;
			iov.iov_len = len;
			len = 0;
		} else {
			iov.iov_base = cur->ptr + npos;
			iov.iov_len = ncap;
			len -= ncap;
			cur = cur->next;
			ncap = cur->size;
			npos = 0;
		}
		buffers.push_back(iov);
	}
	return size;
}

int32_t  ByteArray::getReadBuffers(std::vector<iovec>& buffers, uint64_t len, uint64_t position) const {
	len = len > getReadSize() ? getReadSize() : len;
	if (len == 0) {
		return 0;	
	}

	uint64_t size = len;

	size_t npos = position % m_baseSize;

	size_t count = position / m_baseSize; 
	Node* cur = m_root;
	while (count > 0) {
		cur = cur->next;
		--count;
	}

	size_t ncap = m_cur->size - npos;
	struct iovec iov;

	while (len > 0) {
		if (ncap >= len) {
			iov.iov_base = cur->ptr + npos;
			iov.iov_len = len;
			len = 0;
		} else {
			iov.iov_base = cur->ptr + npos;
			iov.iov_len = ncap;
			len -= ncap;
			cur = cur->next;
			ncap = cur->size;
			npos = 0;
		}
		buffers.push_back(iov);
	}
	return size;
}

uint64_t ByteArray::getWriteBuffers(std::vector<iovec>& buffers, uint64_t len) {
	if (len == 0) {
		return 0;
	}		
	addCapacity(len);
	uint64_t size = len;

	size_t npos = m_position % m_baseSize;
	size_t ncap = m_cur->size - npos;
	struct iovec iov;
	Node* cur = m_cur;
	while (len > 0) {
		if (ncap >= len) {
			iov.iov_base = cur->ptr + npos;
			iov.iov_len = len;
			len = 0;
		} else {
			iov.iov_base = cur->ptr + npos;
			iov.iov_len = ncap;

			len -= ncap;
			cur = cur->next;
			ncap = cur->size;
			npos = 0;
		}
		buffers.push_back(iov);
	}
	return size;
}

}

