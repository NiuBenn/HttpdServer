#ifndef __PROTOCOL_UTIL_HPP__
#define __PROTOCOL_UTIL_HPP__

#include "Log.hpp"
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <sstream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unordered_map>
#include <sys/sendfile.h>
#include <fcntl.h>
#include <sys/wait.h>

#define HOME_PAGE "index.html"
#define WEB_ROOT "HTMLROOT"

#define OK 200
#define NOT_FOUND 404
#define BAD_REQUEST 400
#define SERVER_ERROR 500

#define HTTP_VERSION "http/1.0"

std::unordered_map<std::string, std::string> stuffix_map{
    {".html","text/html"},
    {".htm", "text/html"},
    {".css", "text/css"},
    {".js", "application/x-javascript"}
};

class Tools
{
public:
	static void MakeKV(std::unordered_map<std::string, std::string> &kvmap,std::string &str)
	{
		std::size_t pos = str.find(": ");
		if(std::string::npos != pos)
		{
			return;
		}
		
		std::string k = str.substr(0,pos);
		std::string v = str.substr(pos+2);
		
		kvmap.insert(make_pair(k,v));		
	}
	
	static std::string IntToStr(int code)
	{
		std::stringstream ss;
		ss << code;
		return ss.str();
	}
	
	static std::string GetCodeDesr(int code)
	{
		switch(code)
		{
			case 200:
				return "OK";
			case 400:
				return "Bad Request";
			case 404:
				return "Not Found";
			default:
				return "Unknow";
        }
	}
	
	static std::string SuffixToType(std::string &suffix)
	{
		return stuffix_map[suffix];
	}
		
};

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
	int _content_length;
	std::unordered_map<std::string, std::string> _head_kvmap;
	
	
public:

	Request()
	{
		_blank = "\n";
		_cgi = false;
		_path = WEB_ROOT;
		_resource_size = 0;
		_content_length = -1;
		_resource_suffix = ".html";
	}
	
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
                std::cout<<"first param:"<<_param<<std::endl;
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
		
		if(_path[_path.size()-1] == '/')
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
	
	bool RequestHeadParse()
	{
		std::size_t begin = 0;
		while(begin < _rq_head.size())
		{
			std::size_t pos = _rq_head.find('\n', begin);
			if(std::string::npos == pos)
			{
				break;
			}
			
			std::string substr = _rq_head.substr(begin, pos-begin);
			
			if(substr.empty())
			{
				break;
			}
			else
			{
				Tools::MakeKV(_head_kvmap, substr);
			}
			
			begin = pos + 1;
		}
		
		return true;
	}

	bool IsNeedRecvText()
	{
		if(strcasecmp(_method.c_str(),"POST")==0)
		{
			return true;
		}
		return false;
	}
	
    bool IsCgi()
    {
        return _cgi;
    }
	
	int GetResourceSize()
	{
		return _resource_size;
	}
	
	void SetResourceSize(int rs)
    {
        _resource_size = rs;
    }
	
	std::string& GetSuffix()
	{
		return _resource_suffix;
	}
	
	std::string& GetPath()
	{
		return _path;
	}
	
	int GetContentLength()
	{
		std::string cl = _head_kvmap["Content-Length"];
		if(!cl.empty())
		{
			std::stringstream ss(cl);
			ss >> _content_length;
		}
		return _content_length;
	}
	
	std::string& GetParam()
	{
		return _param;
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
	int _fd;
	
public:
	Response()
	{
		_blank = "\n";
		_code = OK;
		_fd = -1;
	}
		
	void MakeResponseLine()
	{
		_rsp_line = HTTP_VERSION;
		_rsp_line += " ";
		_rsp_line += Tools::IntToStr(_code);
		_rsp_line += " ";
		_rsp_line += Tools::GetCodeDesr(_code);
        _rsp_line += "\n";
	}
	
	void MakeResponseHead(Request* &rq)
	{
		_rsp_head = "Content-Length: ";
		_rsp_head += Tools::IntToStr(rq->GetResourceSize());
		_rsp_head += "\n";
        _rsp_head += "Content-Type: ";
		_rsp_head += Tools::SuffixToType(rq->GetSuffix());
        _rsp_head += "\n";
	}
	
	void OpenResource(Request* &rq)
	{
		_fd = open((rq->GetPath()).c_str(), O_RDONLY);
    }
	
    ~Response()
    {
        if(_fd > 0)
        {
            close(_fd);
        }
    }

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
	
	void RecvRequestHead(std::string& head)
	{
		head = "";
		std::string line;
		while(line != "\n")
		{
			line = "";
			RecvOneLine(line);
			head += line;
		}
	}
	
	void RecvRequestText(std::string& text, int Length, std::string& param)
	{
		char c;
		for(int i = 0; i < Length; ++i)
		{
			recv( _sock, &c, 1, 0);
			text.push_back(c);
		}
		param = text;
	}
	
	void SendResponse(Request* &rq, Response* &rsp, bool cgi)
	{
		std::string& rsp_line = rsp->_rsp_line;
		std::string& rsp_head = rsp->_rsp_head;
		std::string& rsp_blank = rsp->_blank;
		
        send(_sock, rsp_line.c_str(), rsp_line.size(), 0);
		send(_sock, rsp_head.c_str(), rsp_head.size(), 0);
        send(_sock, rsp_blank.c_str(), rsp_blank.size(), 0);
	
		if(cgi)
		{
			//cgi处理
			std::string& rsp_text = rsp->_rsp_text;
			send(_sock, rsp_text.c_str(), rsp_text.size(), 0);
		}
		else
		{
			 int &fd = rsp->_fd;
             sendfile(_sock, fd, NULL, rq->GetResourceSize());
		}
	}
	
};

class Entry
{
public:


	static void MakeResponseNonCgi(Connect* &conn, Request* &rq, Response* &rsp)
	{
		rsp->MakeResponseLine();
		rsp->MakeResponseHead(rq);
		rsp->OpenResource(rq);
		conn->SendResponse(rq, rsp, false);
	}
	
	static void MakeResponseCgi(Connect* &conn, Request* &rq, Response* &rsp)
	{
		int &code = rsp->_code;
		int input[2];
		int output[2];
		
		std::string param = rq->GetParam();
		std::string &rsp_text = rsp->_rsp_text;
		
		pipe(input);
		pipe(output);
		
		pid_t pid = fork();
		
		if(pid < 0)
		{
			code = SERVER_ERROR;
			return;
		}
		else if(pid == 0)
		{
			//child
			close(input[1]);
			close(output[2]);
			
			const std::string &path = rq->GetPath();
			
			dup2(input[0], 0);
            dup2(output[1], 1);
            execl(path.c_str(), path.c_str(), NULL);
            exit(1);
		}
		else
		{
			//parent
			
			close(input[0]);
            close(output[1]);

            size_t size = param.size();
            size_t total = 0;
            size_t curr = 0;
            const char *p = param.c_str();
            
            while( total < size &&(curr = write(input[1], p + total, size - total)) > 0 )
			{
                total += curr;
            }

			close(input[1]);
			
            char c;
            while(read(output[0], &c, 1) > 0)
			{
                rsp_text.push_back(c);
			}
			
			waitpid(pid, NULL, 0);
			
			close(output[0]);
			
			rsp->MakeResponseLine();
			rq->SetResourceSize(rsp_text.size());

			rsp->MakeResponseHead(rq);
			conn->SendResponse( rq, rsp, true);
		}


		
	}
	
	static void MakeResponse(Connect* &conn, Request* &rq, Response* &rsp)
	{
		if(rq->IsCgi())
		{
			MakeResponseCgi(conn, rq, rsp);
		}
		else
		{
			MakeResponseNonCgi(conn, rq, rsp);
		}
	}

	static void HandlerError(Connect* &conn, Request* &rq, Response* &rsp)
	{
		int &code = rsp->_code;
            switch(code){
                case 400:
                    break;
                case 404:
                    //Process404(conn, rq, rsp);
                    break;
                case 500:
                    break;
                case 503:
                    break;
            }
	}
	
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
			conn->RecvRequestHead(rq->_rq_head);
			code = BAD_REQUEST;
			goto end;

		}
		
		rq->UriParse();
		
		if(!rq->IsPathOK())
		{
			conn->RecvRequestHead(rq->_rq_head);
			code = NOT_FOUND;
			goto end;
		}
		
		LOG(INFO,"Request Path Is OK!!");
		conn->RecvRequestHead(rq->_rq_head);
		
		if(rq->RequestHeadParse())
		{
			LOG(INFO,"Request Head Parse Succes")
		}
		else
		{
			code = BAD_REQUEST;
			goto end;
		}
		
		if(rq->IsNeedRecvText())
		{
			conn->RecvRequestText(rq->_rq_text, rq->GetContentLength(),rq->GetParam());
		}
		
		MakeResponse(conn, rq, rsp);
					
end:
		if(code!=OK)
		{
			HandlerError(conn, rq, rsp);
		}

        delete conn;
        delete rq;
        delete rsp;

	}
};








#endif
