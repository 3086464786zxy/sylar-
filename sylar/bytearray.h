#ifndef __SYLAR_BYTEARRAY_H__ 
#define __SYLAR_BYTEARRAY_H__ 

#include <memory> 
#include <sys/uio.h>
#include <stdint.h>
#include <string> 
#include <vector>
#include <sys/types.h>

namespace sylar {

class ByteArray {
public:
	typedef std::shared_ptr<ByteArray> ptr;
	
	struct Node {
		Node(size_t s);
		Node();
		~Node();

		char* ptr;
		Node* next;
		size_t size;
	};	

	ByteArray(size_t base_size = 4096);
	~ByteArray();
	
	//write		10以下的数可以用一个字节， 所以采用两种方式， 一种不压缩，一种采用protobuf压缩 
	//固定长度
	void writeFint8(int8_t value);		// 写一个字节的数据
	void writeFuint8(uint8_t value);
	void writeFint16(int16_t value);
	void writeFuint16(uint16_t value);
	void writeFint32(int32_t value);
	void writeFuint32(uint32_t value);
	void writeFint64(int64_t value);
	void writeFuint64(uint64_t value);

	//可变长度
	void writeInt32(int32_t value);
	void writeUint32(uint32_t value);
	void writeInt64(int64_t value);
	void writeUint64(uint64_t value);
	void writeFloat(float value);
	void writeDouble(float value);

	//length:int16 , data
	void writeStringF16(const std::string& value);		//写长度为16的string
	//length:int32 , data
	void writeStringF32(const std::string& value);
	//length:int64 , data
	void writeStringF64(const std::string& value);
	//length:varint , data
	void writeStringVint(const std::string& value);		//表示用压缩长度来表示， 具体有多少个就压缩多少int进去  
	//data
	void writeStringWithoutLength(const std::string& value); 
	
	//read 
	int8_t 	 readFint8();
	uint8_t  readFuint8();
	int16_t  readFint16();
	uint16_t readFuint16();
	int32_t  readFint32();
	uint32_t readFuint32();
	int64_t  readFint64();
	uint64_t readFuint64();	

	int32_t  readInt32();
	uint32_t readUint32();
	int64_t  readInt64();
	uint64_t readUint64();

	float 	 readFloat();
	double 	 readDouble();

	//length:int16, data
	std::string readStringF16();
	//length:int32, data
	std::string readStringF32();
	//length:int64, data
	std::string readStringF64();
	//length:varint, data
	std::string readStringVint();

	//内部操作 
	void clear();

	void write(const void* buf, size_t size);
	void read(void* buf, size_t size);
	void read(void* buf, size_t size, size_t position) const;

	size_t getPosition() const { return m_position;};
	void setPosition(size_t v);

	bool writeToFile(const std::string& name) const;
	bool readFromFile(const std::string& name);

	size_t getBaseSize() const { return m_baseSize; } 
	size_t getReadSize() const { return m_size - m_position; } 
	size_t getSize() const { return m_size; }

	bool isLittleEndian() const;
	void setIsLittleEndian(bool val);

	std::string toString() const; 
	std::string toHexString() const;		//转换16进制文本

	//只获取当前内容， 不修改position
	/**
     * 获取可读取的缓存,保存成iovec数组
     * buffers 保存可读取数据的iovec数组
     * len 读取数据的长度,如果len > getReadSize() 则 len = getReadSize()
     * 返回实际数据的长度
     */
	int32_t  getReadBuffers(std::vector<iovec>& buffers, uint64_t len = ~0ull) const;

	/**
     * 获取可读取的缓存,保存成iovec数组,从position位置开始
     * buffers 保存可读取数据的iovec数组
     * len 读取数据的长度,如果len > getReadSize() 则 len = getReadSize()
     * position 读取数据的位置
     * 返回实际数据的长度
     */
	int32_t  getReadBuffers(std::vector<iovec>& buffers, uint64_t len, uint64_t position) const;

	//增加容量， 不修改position
	/**
     *  获取可写入的缓存,保存成iovec数组
     * buffers 保存可写入的内存的iovec数组
     * len 写入的长度
     * 返回实际的长度
     * 如果(m_position + len) > m_capacity 则 m_capacity扩容N个节点以容纳len长度
     */
	uint64_t getWriteBuffers(std::vector<iovec>& buffers, uint64_t len);

private: 
	void addCapacity(size_t size);
	size_t getCapacity() const { return m_capacity - m_position; } 
private:
	size_t m_baseSize;		// 内存块的大小(节点) 
	size_t m_position;		// 在总容量中的位置
	size_t m_capacity;		// 当前所有节点的总容量 
	size_t m_size;			// 当前所有数据的大小 
	int m_endian;			// 字节序， 默认为大端 
	Node* m_root;			// 第一个内存块的指针 
	Node* m_cur;			// 当前内存块的指针
};

}

#endif
