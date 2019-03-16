#include"HttpdServer.hpp"
#include<unistd.h>
#include<signal.h>
void Usage(std::string procname)
{
	std::cout<< "How to use: " << procname << " port" << std::endl;
}

int main(int argc, char *argv[])
{
	if(argc != 2)
	{
		Usage(argv[0]);	//打印使用手册
		exit(1);
	}

    signal(SIGPIPE, SIG_IGN);

	HttpdServer* server = new HttpdServer(atoi(argv[1]));	//new HttpdServer对象
	server->Init();		//对象初始化

    server->Start();	//启动server
	
	delete server;		
	return 0;
}
