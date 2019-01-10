#ifndef __Pthread_Pool__
#define __Pthread_Pool__

#include<iostream>
#include<queue>
#include<pthread.h>
#include"Log.hpp"

typedef int (*Handler)(int);

class Task
{
private:
	int _sock;
	Handler _handler;
public:
	Task()
	{
		_sock = -1;
		_handler = NULL;
	}
	void SetTask(int sock, Handler handler)
	{
		_sock = sock;
		_handler = handler;
	}
	void Run()
	{
		_handler(_sock);
	}
	~Task()
	{}
};

#define NUM 5

class ThreadPool
{
private:
	int _thread_total_num;
	int _thread_idle_num;
	std::queue<Task> _task_queue;
	pthread_mutex_t _lock;
	pthread_cond_t _cond;

public:
	void LockQueue()
	{
		pthread_mutex_lock(&_lock);
	}
	void UnlockQueue()
	{
		pthread_mutex_unlock(&_lock);
	}
	
	bool IsEmpty()
	{
		return _task_queue.empty();
	}
	
	void ThreadIdle()
	{
		_thread_idle_num++;
		pthread_cond_wait(&_cond, &_lock);
		_thread_idle_num--;
	}
	
	void WeakOneThread()
	{
		pthread_cond_signal(&_cond);
	}
	
	static void* ThreadRoutine(void* arg)
	{
		ThreadPool* tp = (ThreadPool*)arg;
		pthread_detach(pthread_self());
		
		while(1)
		{
			tp->LockQueue();
			while(tp->IsEmpty())
			{
				tp->ThreadIdle();
			}
			Task t;
			tp->PopTask(t);
			tp->UnlockQueue();
			LOG(INFO,"Task has be taken, handler...");
			std::cout<<"Thread id is :"<<pthread_self()<<std::endl;
			t.Run();
		}
	}
	
	
public:

	ThreadPool(int num = NUM)
		:_thread_total_num(num),_thread_idle_num(0)
	{
		pthread_mutex_init(&_lock, NULL);
		pthread_cond_init(&_cond, NULL);
	}
	
	void InitThreadPool()
	{
		for(int i = 0; i < _thread_total_num; ++i)
		{
			pthread_t tid;
			pthread_create(&tid, NULL, ThreadRoutine, this);
		}
	}
	
	void PushTask(Task &t)
	{
		LockQueue();
		_task_queue.push(t);
		WeakOneThread();
		UnlockQueue();
	}
	
	void PopTask(Task &t)
	{
		t = _task_queue.front();
		_task_queue.pop();
	}
	
	~ThreadPool()
	{
		pthread_mutex_destroy(&_lock);
		pthread_cond_destroy(&_cond);
	}
};


#endif
