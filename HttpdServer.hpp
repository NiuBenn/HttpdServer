#ifndef __HTTPD_SERVER_HPP__
#define __HTTPD_SERVER_HPP__

#include<pthread.h>
#include"ProtocolUtil.hpp"


class HttpdServer
{
private:	
	int _listen_sock;
	int _prot;
	
public:
	HttpdServer(int prot)
	{
		_prot = prot;
		_listen_sock = -1;
	}
	
	void Init()
	{
		// 创建 socket 文件描述符 (TCP/UDP, 客户端 + 服务器) 
		//int socket(int domain, int type, int protocol);

		_listen_sock = socket(AF_INET, SOCK_STREAM, 0);
		if(_listen_sock < 0)
		{
			LOG(ERROR, "Creat Socket Error!");
			exit(2);
		}
		
		/*int setsockopt(
		SOCKET s,
		int level,
		int optname,
		const char* optval,
		int optlen
		);*/
		bool Reuseaddr = true;
		setsockopt(_listen_sock, SOL_SOCKET, SO_REUSEADDR, (const char*)&Reuseaddr, sizeof(Reuseaddr));
		
		sockaddr_in local;
		local.sin_family = AF_INET;
		local.sin_port = htons(_prot);
		local.sin_addr.s_addr = INADDR_ANY;
		
		// 绑定端⼝口号 (TCP/UDP, 服务器)      
		//int bind(int socket, const struct sockaddr *address,socklen_t address_len);

		if( bind( _listen_sock, (const struct sockaddr*)&local, sizeof(local)) < 0)
		{
			LOG(ERROR, "Bind Socket Error!!");
			exit(3);
		}
		
		// 开始监听socket (TCP, 服务器) 
		//int listen(int socket, int backlog);
		
		if( listen(_listen_sock, 5) < 0)
		{
			LOG(ERROR, "Listen Socket Error!!");
			exit(4);
		}
		
		LOG(INFO, "Init Server Success!!");
	}
	
	void Start()
	{
		LOG(INFO, "Start Server Begin!!");
		while(1)
		{
			//接收请求 (TCP, 服务器) 
			//int accept(int socket, struct sockaddr* address, socklen_t* address_len);
			struct sockaddr_in client;
			socklen_t len = sizeof(client);
			int sock = accept(_listen_sock, (struct sockaddr*)&client, &len);
			if(sock < 0)
			{
				LOG(WARNING, "Accept Error!!");
				continue;
			}
			LOG(INFO, "Get New Client, Create Thread Handler Rquest...");
			
			pthread_t tid;
			int *sock_p = new int;
			*sock_p = sock;
			
			//int pthread_create(pthread_t *thread, const pthread_attr_t *attr, void *(*star t_routine)(void*), void *arg); 
			//pthread_creat(&tid, NULL, Entry::HandlerRequest, (void*)sock_p);
			
			delete sock_p;
		}
	}
	
	~HttpdServer()
	{
		if(_listen_sock != -1)
		{
			close(_listen_sock);
		}
		_prot = -1;
	}

};







#endif
