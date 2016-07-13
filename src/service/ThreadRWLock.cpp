/////////////////////////////////////////////////////////////////////////////////////////////////////
//
// ThreadRWLock.cpp : 线程读写锁ThreadRWLock的定义
//
// 版本：v1.0 - 2015.01.09 by 冯扬(fengyoung82@gmail.com) 
//
/////////////////////////////////////////////////////////////////////////////////////////////////////

#include "ThreadRWLock.h"
using namespace demeter_service; 
#include <string>
#include <iostream>
using namespace std; 
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include <errno.h>
#include <string.h>


/////////////////////////////////////////////////////////////////////////////////////////////////////
// Construction & Destruction 

// 构造函数 
ThreadRWLock::ThreadRWLock()
{
	int rc = -1;

	// 初始化
	rc = pthread_rwlock_init(&_rw_lock, NULL); 
	assert(rc == 0);

	if(rc != 0)
		throw "pthread_rwlock_init error"; 
}


// 析构函数 
ThreadRWLock::~ThreadRWLock()
{
	int rc = -1;
	rc = pthread_rwlock_destroy(&_rw_lock); 
	if(rc != 0)
	{
		cerr << "pthread_mutex_destroy error:" << string(strerror(rc)) << endl;
	}
}


/////////////////////////////////////////////////////////////////////////////////////////////////////
// Operations 

// 加锁(读)
void ThreadRWLock::RdLock() const
{
	int rc = pthread_rwlock_rdlock(&_rw_lock); 
	if(rc != 0)
	{
		throw "pthread_rwlock_rdlock error!"; 
	} 
}


// 加锁(写)
void ThreadRWLock::WrLock() const
{
	int rc = pthread_rwlock_wrlock(&_rw_lock); 
	if(rc != 0)
	{
		throw "pthread_rwlock_wrlock error!"; 
	} 
}


//尝试锁(读)
bool ThreadRWLock::TryRdLock() const
{
	int rc = pthread_rwlock_tryrdlock(&_rw_lock); 
	if(rc != 0)
	{
		throw "pthread_rwlock_tryrdlock error!"; 
	} 
	return (rc == 0);
}


//尝试锁(写)
bool ThreadRWLock::TryWrLock() const
{
	int rc = pthread_rwlock_trywrlock(&_rw_lock); 
	if(rc != 0)
	{
		throw "pthread_rwlock_trywrlock error!"; 
	} 
	return (rc == 0);
}


//解锁
void ThreadRWLock::Unlock() const
{
	int rc = pthread_rwlock_unlock(&_rw_lock);
	if(rc != 0)
	{
		throw "pthread_rwlock_unlock error!";
	} 
}



