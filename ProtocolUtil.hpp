#ifndef __PROTOCOL_UTIL_HPP__
#define __PROTOCOL_UTIL_HPP__

#include "Log.hpp"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sstream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/stat.h>


#define HOME_PAGE "index.html"

#define OK 200
#define NOT_FOUND 404
#define BAD_REQUEST 400


class Request
{
public:
	std::string _rq_line;
	std::string _rq_head;
	std::string _blank;
	std::string _rq_text;
private:
	std::string _method;	//请求方法
	std::string _uri;	
	std::string _version;	//协议版本
	
	bool _cgi;	
	
	std::string _path;
	std::string _param;
	
	int _resource_size;
	std::string _resource_suffix;
	
	
public:
	void RequestLineParse()	//请求行解析
	{
		std::stringstream ss(_rq_line);
		ss >> _method >> _uri >> _version;
	}
	
	bool IsMethodOK()
	{
		if(strcasecmp(_method.c_str(), "GET") == 0)
			return true;
		if(strcasecmp(_method.c_str(),"POST") == 0)
		{
			_cgi = true;
			return true;
		}
		return false;
	}
	
	void UriParse()
	{
		if(strcasecmp(_method.c_str(), "GET") == 0)
		{
			std::size_t pos = _uri.find('?');
			if(std::string::npos != pos)
			{
				_cgi = true;
				_path += _uri.substr(0,pos);
				_param += _uri.substr(pos+1); 
				
			}
			else
			{
				_path += _uri;
			}
		}
		else
		{
			_path += _uri;
		}
		
		if(_path[_path.size()-1] = '/')
		{
			_path += HOME_PAGE;
		}

    }
	
	bool IsPathOK()
	{
		struct stat st;
		if(stat(_path.c_str(), &st) < 0)
		{
			LOG(WARNING, "Path Not Found");
			return false;
		}
		
		if(S_ISDIR(st.st_mode))
		{
			_path += '/';
			_path += HOME_PAGE;
		}
		else
		{
			if((st.st_mode & S_IXUSR ) || (st.st_mode & S_IXGRP) || (st.st_mode & S_IXOTH))
			{
               _cgi = true;
            }
		}
		
		_resource_size = st.st_size;
		std::size_t pos = _path.rfind(".");
		if(std::string::npos != pos)
		{
			_resource_suffix = _path.substr(pos);
		}
		
		return true;
	}
	
};


class Response
{
public:
	std::string _rsp_line;
	std::string _rsp_head;
	std::string _blank;
	std::string _rsp_text;
	int _code;
};


class Connect
{
private:
	int _sock;
public:
	Connect(int sock)
	{
		_sock = sock;
	}
	
	int RecvOneLine(std::string& line)
	{
		char c = 'c';
		while(c != '\n')
		{
			//ssize_t recv(int sockfd, void *buf, size_t len, int flags);
			ssize_t size = recv(_sock, &c, 1, 0);
			if(size > 0)
			{
				if(c == '\r')
				{
					recv(_sock, &c, 1, MSG_PEEK);
					if(c == '\n')
					{
						recv(_sock, &c, 1, 0);
					}
					else
					{
						c = '\n';
					}
				}
				line.push_back(c);
			}
			else
			{
				break;
			}
		}
		return line.size();
	}	
	
};

class Entry
{
public:
	static void *HandlerRequest(void* abc)
	{
        int sock = *(int*)abc;
        delete (int*)abc;

		Connect* conn = new Connect(sock);
		Request* rq = new Request();
		Response* rsp = new Response();	
		int &code = rsp->_code;
		conn->RecvOneLine(rq->_rq_line);
		rq->RequestLineParse();
		if(!rq->IsMethodOK())
		{
			//conn->RecvRequestHead(rq->_rq_head);
        LOG(INFO,"bad");
			code = BAD_REQUEST;
			goto end;

		}
		
		rq->UriParse();
		
		if(!rq->IsPathOK())
		{
			//conn->RecvRequestHead(rq->_rq_head);
			code = NOT_FOUND;
			goto end;
		}
		
		LOG(INFO,"Request Path Is OK!!");
end:
        ;

	}
};








#endif
