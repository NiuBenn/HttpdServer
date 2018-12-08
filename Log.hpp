#ifndef __LOG_HPP__
#define __LOG_HPP__

#include<iostream>
#include<string>
#include<sys/time.h>

#define INFO 0
#define WARNING 1
#define ERROR 2



uint64_t GetNowTimeSteamp()
{
	struct timeval nowtime;
	gettimeofday(&nowtime, NULL);
	return nowtime.tv_sec;
}

std::string GetLogLevel(int level)
{
	switch(level)
	{
		case 0:
			return "INFO";
		case 1:
			return "WARNING";
		case 2:
			return "ERROR";
		default:
			return "UNKNOW";
	}
}

void Log(int level, std::string message, std::string file, int line)
{
	std::cout << "[ " << GetNowTimeSteamp() << " ] [ " << GetLogLevel(level) << " ] [ " << file << " ] [ " << line << " ]";
	std::cout << " [ " << message << " ]" << std::endl;
}

#define LOG(level,message) Log(level,message,__FILE__,__LINE__);

#endif
