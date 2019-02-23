#ifndef __LOG_HPP__
#define __LOG_HPP__

#include<iostream>
#include<string>
#include<sys/time.h>
#include<fstream>

#define INFO 0
#define WARNING 1
#define ERROR 2

std::string LogFilePath("./Log/Log.log");

std::string GetNowTime()
{
	struct timeval nowtime;
	gettimeofday(&nowtime, NULL);
	char ntime[32];
    time_t tt = nowtime.tv_sec;
    struct tm* ttime;
    ttime = localtime(&tt);
    strftime(ntime,64,"%Y-%m-%d %H:%M:%S",ttime);
    std::string result = ntime;
    return result;
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

    std::ofstream logfile;
    logfile.open(LogFilePath.c_str(),std::ios::app);
	logfile << "[ " << GetNowTime() << " ] [ " << GetLogLevel(level) << " ] [ " << file << " ] [ " << line << " ]";
	logfile << " [ " << message << " ]" << std::endl;
    logfile.close();
}

#define LOG(level,message) Log(level,message,__FILE__,__LINE__);

#endif
