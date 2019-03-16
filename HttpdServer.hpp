#ifndef __HTTPD_SERVER_HPP__
#define __HTTPD_SERVER_HPP__

#include<pthread.h>
#include<poll.h>
#include<vector>
#include"ProtocolUtil.hpp"
#include"ThreadPool.hpp"

typedef std::vector<struct pollfd> PollFdList;

class HttpdServer
{
private:
	struct pollfd  _listen_fd;
    PollFdList _pollfds;
	int _prot;
    ThreadPool *_tp;
public:
	HttpdServer(int prot)
	{
		_prot = prot;
		_listen_fd.fd = -1;
	}
	
	void Init()
	{
		// 创建 socket 文件描述符 (TCP/UDP, 客户端 + 服务器) 
		//int socket(int domain, int type, int protocol);

        _listen_fd.events = POLLIN;
		_listen_fd.fd = socket(AF_INET, SOCK_STREAM, 0);
		if(_listen_fd.fd < 0)
		{
			LOG(ERROR, "Creat Socket Error!");
			exit(2);
		}
		_pollfds.push_back(_listen_fd);

		/*int setsockopt(
		SOCKET s,
		int level,
		int optname,
		const char* optval,
		int optlen
		);*/

		int opt = 1;
		setsockopt(_listen_fd.fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
		
		sockaddr_in local;
		local.sin_family = AF_INET;
		local.sin_port = htons(_prot);
		local.sin_addr.s_addr = INADDR_ANY;
		
		// 绑定端⼝口号 (TCP/UDP, 服务器)      
		//int bind(int socket, const struct sockaddr *address,socklen_t address_len);

		if( bind( _listen_fd.fd, (const struct sockaddr*)&local, sizeof(local)) < 0)
		{
			LOG(ERROR, "Bind Socket Error!!");
			exit(3);
		}
		
		// 开始监听socket (TCP, 服务器) 
		//int listen(int socket, int backlog);
		
		if( listen(_listen_fd.fd, 5) < 0)
		{
			LOG(ERROR, "Listen Socket Error!!");
			exit(4);
		}
        
        _tp = new ThreadPool();
        _tp->InitThreadPool();
        LOG(INFO, "Init Server Success!!");
	}
	
	void Start()
	{
        LOG(INFO, "Start Server Begin!!");
        while(1)
        {
            int ret = poll(&*_pollfds.begin(), _pollfds.size(), -1);
            
            if(ret < 0)
            {
                LOG(WARNING,"Poll Error!!");
                continue;
            }
            if(ret == 0)
            {
                LOG(INFO,"Poll TimeOut!!");
                continue;
            }
            
            if(_pollfds[0].revents & POLLIN)
            {
                //接收请求 (TCP, 服务器)
                //int accept(int socket, struct sockaddr* address, socklen_t* address_len);
                struct pollfd client_fd;
                client_fd.events = POLLIN;
                struct sockaddr_in client;
                socklen_t len = sizeof(client);
                client_fd.fd = accept(_listen_fd.fd, (struct sockaddr*)&client, &len);
                if(client_fd.fd < 0)
                {
                    LOG(WARNING, "Accept Error!!");
                    continue;
                }

                _pollfds.push_back(client_fd);
                LOG(INFO, "Get New Client, Create Thread Handler Rquest...");
            }
            
            for(size_t i = 1; i < _pollfds.size(); ++i)
            {
                if(_pollfds[i].revents & POLLIN)
                {
                    Task t;
                    t.SetTask(_pollfds[i].fd,Entry::HandlerRequest);
                    _tp->PushTask(t);
                    _pollfds.erase(_pollfds.begin()+i);
                }
            }
           
            // pthread_t tid;
            //int *sock_p = new int;
            //*sock_p = sock;
            //int pthread_create(pthread_t *thread, const pthread_attr_t *attr, void *(*star t_routine)(void*), void *arg);
            //pthread_create(&tid, NULL, Entry::HandlerRequest, (void*)sock_p);
        }
    }
    
    ~HttpdServer()
    {
        if(_listen_fd.fd != -1)
        {
            close(_listen_fd.fd);
        }
        _prot = -1;
        delete _tp;
    }
};


#endif
