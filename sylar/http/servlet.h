#ifndef __SYLAR_HTTP_SERVLET_H__ 
#define __SYLAR_HTTP_SERVLET_H__ 

#include <memory> 
#include <functional> 
#include <string> 
#include <unordered_map>
#include <vector>
#include "http.h" 
#include "http_session.h"
#include "../thread.h"

namespace sylar {
namespace http {

class Servlet {
public:
	typedef std::shared_ptr<Servlet> ptr;

	Servlet(const std::string& name) 
		: m_name(name) {}
	virtual ~Servlet() {}
	//request: http请求  response: http响应  session: http连接
	virtual int32_t handle(sylar::http::HttpRequest::ptr request
					, sylar::http::HttpResponse::ptr response
					, sylar::http::HttpSession::ptr session) = 0;
	//返回servlet名称	
	const std::string& getName() const { return m_name; }
protected:
	std::string m_name;
};

class FunctionServlet : public Servlet {
public:
	typedef std::shared_ptr<FunctionServlet> ptr;
	typedef std::function<int32_t(sylar::http::HttpRequest::ptr request
					, sylar::http::HttpResponse::ptr response
					, sylar::http::HttpSession::ptr session)> callback;

	FunctionServlet(callback cb);
	virtual int32_t handle(sylar::http::HttpRequest::ptr request
					, sylar::http::HttpResponse::ptr response
					, sylar::http::HttpSession::ptr session) override;
	
private:
	callback m_cb;
};

//Servlet分发器
class ServletDispatch : public Servlet {
public:
	typedef std::shared_ptr<ServletDispatch> ptr;
	typedef RWMutex RWMutexType;

	ServletDispatch();
	virtual int32_t handle(sylar::http::HttpRequest::ptr request
					, sylar::http::HttpResponse::ptr response
					, sylar::http::HttpSession::ptr session) override;
	
	//添加servlet
	void addServlet(const std::string& uri, Servlet::ptr slt);
	//添加servlet, cb(FunctionServlet::callback cb)
	void addServlet(const std::string& uri, FunctionServlet::callback cb);
	//添加模糊匹配servlet
	void addGlobServlet(const std::string& uri, Servlet::ptr slt);
	//添加模糊匹配servlet, cb(FunctioanSErvlet::callback cb)
	void addGlobServlet(const std::string& uri, FunctionServlet::callback cb);

	//删除servlet
	void delServlet(const std::string& uri);
	//删除模糊匹配servlet
	void delGlobServlet(const std::string& uri);

	//返回默认servlet
	Servlet::ptr getDefault() const { return m_default; }
	//设置默认servlet
	void setDefault(Servlet::ptr v) { m_default = v; }

	//通过uri获取servlet, 返回对应的servlet
	Servlet::ptr getServlet(const std::string& uri);

	//同过uri获取模糊匹配servlet, 返回对应的servlet
	Servlet::ptr getGlobServlet(const std::string& uri);

	//通过uri获取servlet, 优先精准匹配, 其次模糊匹配, 最后返回默认
	Servlet::ptr getMatchedServlet(const std::string& uri);
private:
	RWMutexType m_mutex;
	//uri(sylar/xxx) -> servlet 精准匹配(优先选择)
	std::unordered_map<std::string, Servlet::ptr> m_datas;
	//uri(/sylar/*) -> servlet 	模糊匹配 
	std::vector<std::pair<std::string, Servlet::ptr>> m_globs;	
	//默认servlet, 所有路径都没匹配到的时候使用 
	Servlet::ptr m_default;

};

//uri不存在, 则默认返回404
class NotFoundServlet : public Servlet {
public:
	typedef std::shared_ptr<NotFoundServlet> ptr;
	
	NotFoundServlet(const std::string& name);
	virtual int32_t handle(sylar::http::HttpRequest::ptr request
					, sylar::http::HttpResponse::ptr response
					, sylar::http::HttpSession::ptr session) override;
private:
	std::string m_name;
	std::string m_content;
};

}
}

#endif
