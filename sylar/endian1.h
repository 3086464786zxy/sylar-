#ifndef __SYLAR_ENDIAN_H__ 
#define __SYLAR_ENDIAN_H__ 

#define SYLAR_LITTLE_ENDIAN 1 
#define SYLAR_BIG_ENDIAN 2 

#include <byteswap.h> 
#include <type_traits>
#include <stdint.h> 

namespace sylar {

//template<class T> 
//static T CreateMask(uint32_t bits) {
//	return (1 << (sizeof(T) * 8 - bits)) - 1;
//}
	
template<class T> 
typename std::enable_if<sizeof(T) == sizeof(uint64_t), T>::type 
byteswap(T value) {
	return (T)bswap_64((uint64_t)value); 
}

template<class T> 
typename std::enable_if<sizeof(T) == sizeof(uint32_t), T>::type 
byteswap(T value) {
	return (T)bswap_32((uint32_t)value);
}

template<class T> 
typename std::enable_if<sizeof(T) == sizeof(uint16_t), T>::type 
byteswap(T value) {
	return (T)bswap_16((uint16_t)value);
}
	
//#if BYTE_ORDER == BIG_ENDIAN 
//#define SYLAR_BYTE_ORDER SYLAR_BIG_ENDIAN 
//#else 
//#define SYLAR_BYTE_ORDER SYLAR_LITTLE_ENDIAN 
//#endif 
//
//#if SYLAR_BYTE_ORDER == SYLAR_BIG_ENDIAN 
//template<class T> 
//T byteswapOnLittleEndian(T t) {
//	return t;
//}
//
//template<class T> 
//T byteswapOnBigEndian(T t) {
//	return byteswap(t);
//}
//
//#else 
//
//template<class T> 
//T byteswapOnLittleEndian(T t) {
//	return byteswap(t);
//}
//
//template<class T> 
//T byteswapOnBigEndian(T t) {
//	return t;
//}
//
//#endif 

// 检查是否定义了BYTE_ORDER，并判断其是否为BIG_ENDIAN  
#if BYTE_ORDER == BIG_ENDIAN   
    // 如果系统是大端字节序，则定义SYLAR_BYTE_ORDER为SYLAR_BIG_ENDIAN  
    #define SYLAR_BYTE_ORDER SYLAR_BIG_ENDIAN   
#else   
    // 如果系统不是大端字节序（默认为小端字节序），定义SYLAR_BYTE_ORDER为SYLAR_LITTLE_ENDIAN  
    #define SYLAR_BYTE_ORDER SYLAR_LITTLE_ENDIAN   
#endif   
  
// 根据预编译的SYLAR_BYTE_ORDER值，定义两个模板函数，用于在不同字节序之间进行转换  
// 如果SYLAR_BYTE_ORDER为SYLAR_BIG_ENDIAN（即编译时针对大端字节序系统）  
#if SYLAR_BYTE_ORDER == SYLAR_BIG_ENDIAN   
  
// byteswapOnLittleEndian函数：在大端字节序系统上，直接返回参数，因为不需要字节交换  
// （这里的命名可能有些误导，因为实际上这个函数不处理小端数据到大端的转换）  
template<class T>   
T byteswapOnLittleEndian(T t) {  
    return t;  
}  
  
// byteswapOnBigEndian函数：在大端字节序系统上，对参数进行字节交换，转换为小端字节序  
template<class T>   
T byteswapOnBigEndian(T t) {  
    return byteswap(t); // 假设byteswap函数已经定义，用于执行字节交换操作  
}  
  
// 如果SYLAR_BYTE_ORDER为SYLAR_LITTLE_ENDIAN（即编译时针对小端字节序系统）  
#else   
  
// byteswapOnLittleEndian函数：在小端字节序系统上，对参数进行字节交换，转换为大端字节序  
template<class T>   
T byteswapOnLittleEndian(T t) {  
    return byteswap(t); // 假设byteswap函数已经定义，用于执行字节交换操作  
}  
  
// byteswapOnBigEndian函数：在小端字节序系统上，直接返回参数，因为不需要字节交换  
// （这里的命名可能有些误导，因为实际上这个函数不处理大端数据到小端的转换）  
template<class T>   
T byteswapOnBigEndian(T t) {  
    return t;  
}  
  
#endif // 结束条件编译

}

#endif
