#ifndef __SYLAR_CONFIG_H__
#define __SYLAR_CONFIG_H__

#include<iostream>
#include<memory>
#include<string>
#include<sstream>
#include<boost/lexical_cast.hpp>
#include"log.h"
#include"/usr/local/include/yaml-cpp/yaml.h"
#include"/usr/local/include/yaml-cpp/node/node.h"
#include<algorithm>
#include<vector>
#include<list>
#include<unordered_map>
#include<map>
#include<set>
#include<unordered_set>
#include<functional>

#include "thread.h" 

namespace sylar {

class ConfigVarBase {
public:
        typedef std::shared_ptr<ConfigVarBase> ptr;
        ConfigVarBase(const std::string& name, const std::string& description = "")
                        : m_name(name)
                        , m_description(description) {
                std::transform(m_name.begin(), m_name.end(), m_name.begin(), ::tolower);
        }
        virtual ~ConfigVarBase() {
        }

        const std::string getName() const { return m_name; }
        const std::string& getDescription() const { return m_description; }

        virtual std::string toString() = 0;
        virtual bool fromString(const std::string& val) = 0;

		virtual std::string getTypename() const = 0;

protected:
        std::string m_name;
        std::string m_description;
};

// F from_type, T to_type
template<class F, class T>
class LexicalCast {
public:
        T operator() (const F& v) {
                return boost::lexical_cast<T>(v);
        }
};

template<class T>
class LexicalCast<std::string, std::vector<T>> {
public:
            std::vector<T> operator() (const std::string& v) {
                YAML::Node node = YAML::Load(v);
                typename std::vector<T> vec;
                std::stringstream ss;
                for (size_t i = 0; i < node.size(); i++) {
                        ss.str(""); 	//清空ss
                        ss << node[i];
                        vec.push_back(LexicalCast<std::string, T>()(ss.str()));
                }
                return vec;
        }
};

template<class T>
class LexicalCast<std::vector<T>, std::string> {
public:
            std::string operator() (const std::vector<T>& v) {
                YAML::Node node(YAML::NodeType::Sequence);
                for (auto& i : v) {
                        node.push_back(YAML::Load(LexicalCast<T, std::string>()(i)));
                }
                std::stringstream ss;
                ss << node;
                return ss.str();
        }
};

template<class T>
class LexicalCast<std::string, std::list<T>> {
public:
            std::list<T> operator() (const std::string& v) {
                YAML::Node node = YAML::Load(v);
                typename std::list<T> lis;
                std::stringstream ss;
                for (size_t i = 0; i < node.size(); i++) {
                        ss.str("");
                        ss << node[i];
                        lis.push_back(LexicalCast<std::string, T>()(ss.str()));
                }
                return lis;
        }
};

template<class T>
class LexicalCast<std::list<T>, std::string> {
public:
            std::string operator() (const std::list<T>& v) {
                YAML::Node node(YAML::NodeType::Sequence);
                for (auto& i : v) {
                        node.push_back(YAML::Load(LexicalCast<T, std::string>()(i)));
                }
                std::stringstream ss;
                ss << node;
                return ss.str();
        }
};

template<class T>
class LexicalCast<std::string, std::set<T>> {
public:
        std::set<T> operator()(const std::string& v) {
                YAML::Node node = YAML::Load(v);
                typename std::set<T> t_set;
                std::stringstream ss;
                for (size_t i = 0; i < node.size(); i++) {
                        ss.str("");
                        ss << node[i];
                        t_set.insert(LexicalCast<std::string, T>()(ss.str()));
                }

                return t_set;
        }
};

template<class T>
class LexicalCast<std::set<T>, std::string> {
public:
        std::string operator()(const std::set<T>&v ) {
                YAML::Node node(YAML::NodeType::Sequence);
                for (auto& i : v) {
                        node.push_back(YAML::Load(LexicalCast<T, std::string>()(i)));
                }
                std::stringstream ss;
                ss << node;
                return ss.str();
        }
};

template<class T>
class LexicalCast<std::string, std::unordered_set<T>> {
public:
            std::unordered_set<T> operator() (const std::string& v) {
                YAML::Node node = YAML::Load(v);
                typename std::unordered_set<T> t_u_set;
                std::stringstream ss;
                for (size_t i = 0; i < node.size(); i++) {
                        ss.str("");
                        ss << node[i];
                        t_u_set.insert(LexicalCast<std::string, T>()(ss.str()));
                }
                return t_u_set;
        }
};

template<class T>
class LexicalCast<std::unordered_set<T>, std::string> {
public:
            std::string operator() (const std::unordered_set<T>& v) {
                YAML::Node node(YAML::NodeType::Sequence);
                for (auto& i : v) {
                        node.push_back(YAML::Load(LexicalCast<T, std::string>()(i)));
                }
                std::stringstream ss;
                ss << node;
                return ss.str();
        }
};

template<class T>
class LexicalCast<std::string, std::map<std::string, T>> {
public:
        std::map<std::string, T> operator()(const std::string& v) {
                YAML::Node node = YAML::Load(v);
                typename std::map<std::string, T> t_map;
                std::stringstream ss;
                for (auto it = node.begin(); it != node.end(); it++) {
                        ss.str("");
                        ss << it->second;
                        t_map.insert(std::make_pair(it->first.Scalar(),
                                                LexicalCast<std::string, T>()(ss.str())));
                }
                return t_map;
        }
};

template<class T>
class LexicalCast<std::map<std::string, T>, std::string> {
public:
        std::string operator()(const std::map<std::string, T>&v) {
                YAML::Node node(YAML::NodeType::Map);
                for (auto& i : v) {
                        node[i.first] = YAML::Load(LexicalCast<T, std::string>()(i.second));
                }
                std::stringstream ss;
                ss << node;
                return ss.str();
        }
};

template<class T>
class LexicalCast<std::string, std::unordered_map<std::string, T>> {
public:
        std::unordered_map<std::string, T> operator()(const std::string& v) {
                YAML::Node node = YAML::Load(v);
                typename std::unordered_map<std::string, T> t_umap;
                std::stringstream ss;
                for (auto it = node.begin(); it != node.end(); it++) {
                        ss.str("");
                        ss << it->second;
                        t_umap.insert(std::make_pair(it->first.Scalar(),
                                                LexicalCast<std::string, T>()(ss.str())));
                }
                return t_umap;
        }
};

template<class T>
class LexicalCast<std::unordered_map<std::string, T>, std::string> {
public:
        std::string operator()(const std::unordered_map<std::string, T>&v) {
                YAML::Node node(YAML::NodeType::Map);
                for (auto& i : v) {
                        node[i.first] = YAML::Load(LexicalCast<T, std::string>()(i.second));
                }
                std::stringstream ss;
                ss << node;
                return ss.str();
        }
};


// FromStr T operator() (const std::string&)
// ToStr std::string operator() (const T&)
template<class T, class FromStr = LexicalCast<std::string, T>,
                class ToStr = LexicalCast<T, std::string>>
class ConfigVar : public ConfigVarBase {
public:
	typedef RWMutex RWMutexType;
    typedef std::shared_ptr<ConfigVar> ptr;
	typedef std::function<void(const T& old_value, const T& new_value)> on_change_cb;

        ConfigVar(const std::string& name
                        , const T& default_value
                        , const std::string& description = "")
                        : ConfigVarBase(name, description)
                        , m_val(default_value) {
        }

        std::string toString() override {
                try {
                    //return boost::lexical_cast<std::string>(m_val);
					RWMutexType::ReadLock lock(m_mutex);
                    return ToStr()(m_val);
                } catch (std::exception& e) {
                        SYLAR_LOG_ERROR(SYLAR_LOG_ROOT()) << "ConfigVar::toString exception"
                                        << e.what() << " convert: " << typeid(m_val).name() << " to string";
                }
                return "";
        }

        bool fromString(const std::string& val) override {
                try {
                    //m_val = boost::lexical_cast<T>(val);
					RWMutexType::WriteLock lock(m_mutex);
                    setValue(FromStr()(val));
                } catch (std::exception& e) {
                        SYLAR_LOG_ERROR(SYLAR_LOG_ROOT()) << "ConfigVar::fromString execption"
                                << e.what() << " convert: string to " << typeid(m_val).name();
                }
                return false;
        }

        const T getValue() { 
			RWMutexType::ReadLock lock(m_mutex);	
			return m_val;
	   	}

		void setValue(const T& v) {
			{
				RWMutexType::ReadLock lock(m_mutex);
				if (v == m_val) {
					return;
				}
				for (auto& i : m_cbs) {
					i.second(m_val, v);
				}
			}
			RWMutexType::WriteLock lock(m_mutex);
            m_val = v;
        }

		std::string getTypename() const { return typeid(T).name();}

		uint64_t addListener(on_change_cb cb) { 
			static uint64_t s_fun_id = 0;
			RWMutexType::WriteLock lock(m_mutex);
			++s_fun_id;
            m_cbs[s_fun_id] = cb; 
			return s_fun_id;
        } 

        void delListener(uint64_t key, on_change_cb cb) { 
			RWMutexType::WriteLock lock(m_mutex);
            m_cbs.erase(key);
        } 

        on_change_cb getListener(uint64_t key) { 
			RWMutexType::ReadLock lock(m_mutex);
            auto it = m_cbs.find(key);
            return it == m_cbs.end() ? nullptr : it->second;
        } 

        void clearListener() { 
			RWMutexType::WriteLock lock(m_mutex);
			m_cbs.clear();
        } 
private:
		RWMutexType m_mutex;
        T m_val;

		// 变更回调函数， uint64_t key, 要求唯一，一般可以用hash 
		std::map<uint64_t, on_change_cb> m_cbs;
};

class Config {
public:
    typedef std::map<std::string, ConfigVarBase::ptr> ConfigVarMap; 
	typedef RWMutex RWMutexType;
	template<class T>
    static typename ConfigVar<T>::ptr Lookup(const std::string& name,
        const T& default_value, const std::string& description = "") {
    		auto tmp = Lookup<T>(name);
    		RWMutexType::WriteLock lock(GetMutex());
    		if (tmp) {
    			SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << "Lookup name" << name << "exists";
    			return tmp;
    		}// else {
    		//	SYLAR_LOG_ERROR(SYLAR_LOG_ROOT()) << "Lookup name=" << name << " exists but type not ";
    		//<< " " << it->second->toString();
    		//}
    
    		if (name.find_first_not_of("abcdefghijklmnopqrstuvwxyz._0123456789")
    				!= std::string::npos) {
    			SYLAR_LOG_ERROR(SYLAR_LOG_ROOT()) << "Lookup name invalid " << name;
    			throw std::invalid_argument(name);
    		}
    
    		typename ConfigVar<T>::ptr v(new ConfigVar<T>(name, default_value, description));
    		GetDatas()[name] = v;
    		return v;
    }
    
    template<class T>
    static typename ConfigVar<T>::ptr Lookup(const std::string& name) {
    		RWMutexType::ReadLock lock(GetMutex());
            auto it = GetDatas().find(name);
            if (it == GetDatas().end()) {
                    return nullptr;
            }
            return std::dynamic_pointer_cast<ConfigVar<T>>(it->second);
    }
    
    static void LoadFromYaml(const YAML::Node& root);
    
    static ConfigVarBase::ptr LookupBase(const std::string& name);
    
	static void Visit(std::function<void(ConfigVarBase::ptr)> cb);	
    
private:
        //static ConfigVarMap m_datas;
	static ConfigVarMap& GetDatas() {
		static ConfigVarMap s_datas; 
		return s_datas;	
	}
	
	static RWMutexType& GetMutex() {
		static RWMutexType s_mutex;
		return s_mutex;
	}
};
}

#endif           
