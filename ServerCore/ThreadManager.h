#pragma once
#include "stdafx.h"

/*------------------
	ThreadManager
-------------------*/

class ThreadManager
{
public:
	ThreadManager();
	~ThreadManager();

	void	Launch(function<void(void)> callback);
	void	Join();

	static void InitTLS();
	static void DestroyTLS();

private:
	Mutex			mLock;
	vector<thread>	mThreads;
};

