#include "http_session.h" 
#include "http_parser.h"
#include <string>

namespace sylar {
namespace http {

HttpSession::HttpSession(Socket::ptr sock, bool owner) 
	: SocketStream(sock, owner) {
}

HttpRequest::ptr HttpSession::recvRequest() {
	//HttpRequestParser::ptr parser(new HttpRequestParser);
	////uint64_t buff_size = HttpRequestParser::GetHttpRequestBufferSize();
	//uint64_t buff_size = 100;
	//std::shared_ptr<char> buffer(
    //        new char[buff_size], [](char* ptr){
    //            delete[] ptr;
    //        });
	////获取指向buffer的原始指针
	//char* data = buffer.get();

	////偏移量, 用于记录当前已经读取的数据在buffer中的位置 
	//int offset = 0;

	////循环读取数据, 直到请求被完全解析或发生错误
	//do {
	//	int len = read(data, buff_size);
	//	//如果没有读却到数据或发生错误，则关闭连接并返回nullptr
	//	if (len <= 0) {
	//		close();		//关闭连接
	//		return nullptr;
	//	}

	//	//更新已读取的数据长度(包括之前已经读取的剩下的数据) 
	//	len += offset;

	//	//尝试解析已经读取的数据
	//	size_t nparser = parser->execute(data, len);

	//	//如果有错误, 直接反回nullptr
	//	if (parser->hasError()) {
	//		close();
	//		return nullptr;		//解析失败
	//	}
	//
	//	//更新偏移量, 为下一次读取做准备
	//	offset = len - nparser;	
	//	if (offset == (int)buff_size) {
	//		//nsparser = 0
	//		//关闭连接, 缓冲区满了没有数据解析
	//		close();
	//		return nullptr;
	//	}

	//	//Parser解析完
	//	if (parser->isFinished()) {
	//		break;			
	//	}
	//} while(true);
	////以上只解析request协议部分，下面body部分(若存在)

	////获取请求体的长度(如果存在) 
	//int64_t length = parser->getContentLength();

	//if (length > 0) {
	//	std::string body;
	//	//预先分配足够的空间以存储请求体 
	//	body.reserve(length);

	//	//初始读取长度
	//	int len = 0;

	//	//如果buffer中已经包含整个请求体 
	//	if (length >= offset) {
	//		//从buffer中复制请求体到body字符串 
	//		//memcpy(&body[0], data, length);
	//		//此时data剩下的body长度为offset
	//		body.append(data, offset);
	//	} else {
	//		body.append(data, length);
	//	}

	//	//还需从客户端读取的请求体长度
	//	length -= offset;

	//	//还有信息 
	//	if (length > 0) {
	//		//读取剩余的数据到body字符串中, readFixSize读取固定大小的数据 
	//		if (readFixSize(&body[len], length) <= 0) {
	//			close();
	//			return nullptr;
	//		}
	//	}

	//	//将完整的请求体设置到请求对象中 
	//	parser->getData()->setBody(body);
	//}
	//return parser->getData();
	HttpRequestParser::ptr parser(new HttpRequestParser);
    uint64_t buff_size = HttpRequestParser::GetHttpRequestBufferSize();
    //uint64_t buff_size = 100;
    std::shared_ptr<char> buffer(
            new char[buff_size], [](char* ptr){
                delete[] ptr;
            });
    char* data = buffer.get();
    int offset = 0;
    do {
        int len = read(data + offset, buff_size - offset);
        if(len <= 0) {
            close();
            return nullptr;
        }
        len += offset;
        size_t nparse = parser->execute(data, len);
        if(parser->hasError()) {
            close();
            return nullptr;
        }
        offset = len - nparse;
        if(offset == (int)buff_size) {
            close();
            return nullptr;
        }
        if(parser->isFinished()) {
            break;
        }
    } while(true);
    int64_t length = parser->getContentLength();
    if(length > 0) {
        std::string body;
        body.resize(length);

        int len = 0;
        if(length >= offset) {
            memcpy(&body[0], data, offset);
            len = offset;
        } else {
            memcpy(&body[0], data, length);
            len = length;
        }
        length -= offset;
        if(length > 0) {
            if(readFixSize(&body[len], length) <= 0) {
                close();
                return nullptr;
            }
        }
        parser->getData()->setBody(body);
    }

    return parser->getData();
}

int HttpSession::sendResponse(HttpResponse::ptr rsp) {
	std::stringstream ss;
	ss << *rsp;
	std::string data = ss.str();
	return writeFixSize(data.c_str(), data.size());
}

}
}
