/*
 * @Author: jiamu 
 * @Date: 2018-07-13 17:09:37 
 * @Last Modified by: jiamu
 * @Last Modified time: 2018-07-16 11:57:25
 */

#include "lock.h"
#include <time.h>

static MUTEX_INFO stMutexRecordInfo[MAX_MUTEX_NUM];


void lock_init(void)
{
	int i = 0;
	for(i = 0;i < MAX_MUTEX_NUM;i++)
	{
		stMutexRecordInfo[i].isAlive = false;
		stMutexRecordInfo[i].mutex   = NULL;
		bzero(stMutexRecordInfo[i].strName,sizeof(stMutexRecordInfo[i].strName));
	}
}
#ifdef DEBUG_LOCK
static int lock_getSelfInfo(pthread_mutex_t *mutex)
{
	int i = 0;
	for(i = 0;i < MAX_MUTEX_NUM;i++)
	{
		if(stMutexRecordInfo[i].isAlive == true && stMutexRecordInfo[i].mutex == mutex)
			return i;
	}
	return RT_ARG_INVALID;
}

static void log_recordRunInfo(pthread_mutex_t *mutex,const char * strFunName,UINT32 ulLine,const char * strAction)
{
	int i = lock_getSelfInfo(mutex);
	OS_TASK_CTRL stTaskInfo;
	bzero(&stTaskInfo,sizeof(stTaskInfo));
	if(os_getTaskSelfInfo(pthread_self(),&stTaskInfo) < 0 || i < 0)
		return;

}
#endif
INT32 lock_mutexInit(pthread_mutex_t *mutex,const char * strName)
{
    if(mutex && strName != NULL)
	{	
		int i = 0;
		for(i = 0;i < MAX_MUTEX_NUM;i++)
		{
			if(stMutexRecordInfo[i].isAlive == false && stMutexRecordInfo[i].mutex == NULL)
			{
				stMutexRecordInfo[i].isAlive = true;
				stMutexRecordInfo[i].mutex   = mutex;
				strncpy(stMutexRecordInfo[i].strName,strName,128);
				break;
			}
		}
		if(i == MAX_MUTEX_NUM)
		{
			SYS_TRACE("mutex %s create failed because the num of mutex is over %d ",MAX_MUTEX_NUM);
			return RT_ARG_INVALID;
		}
		return pthread_mutex_init(mutex,NULL);
	}
    else
        return RT_ARG_INVALID;
}

INT32 lock_mutexDetroy(pthread_mutex_t *mutex)
{
    if(mutex)
	{
		int i = 0;
		for(i = 0;i < MAX_MUTEX_NUM;i++)
		{
			if(stMutexRecordInfo[i].isAlive == true && stMutexRecordInfo[i].mutex == mutex)
			{
				stMutexRecordInfo[i].isAlive = false;
				stMutexRecordInfo[i].mutex   = NULL;
				bzero(stMutexRecordInfo[i].strName,sizeof(stMutexRecordInfo[i].strName));
				break;
			}
		}
		return pthread_mutex_destroy(mutex);
	}
    else
        return RT_ARG_INVALID;
}
INT32 lock_mutexLock(pthread_mutex_t *mutex,const char *  strFunName,UINT32 ulLine)
{
    if(mutex)
    {
		#ifdef DEBUG_LOCK
		log_recordRunInfo(mutex,strFunName,ulLine);
		#endif
        return pthread_mutex_lock (mutex); 
    }
    else
        return RT_ARG_INVALID;
}
INT32 lock_mutexUnLock(pthread_mutex_t *mutex,const char * strFunName,UINT32 ulLine)
{
	if(mutex)
    {
        return pthread_mutex_unlock (mutex); 
    }
    else
        return RT_ARG_INVALID;
	 
}
INT32 lock_mutexTryLock(pthread_mutex_t *mutex,const char * strFunName,UINT32 ulLine)
{
    if(mutex)
    {
        return pthread_mutex_trylock(mutex);
    }  
    else
        return RT_ARG_INVALID;
}

INT32 lock_mutexTimeLock(pthread_mutex_t *mutex,UINT32 ulTime/*us*/,const char * strFunName,UINT32 ulLine)
{
    struct timespec ts;
	if(clock_gettime(CLOCK_REALTIME,&ts) < 0)
	{
		return RT_FAILED;
	}
	
	UINT32 uiNanoseconds = ulTime * 1000;
	if(MAX_NANO_SEC < (ts.tv_nsec + uiNanoseconds))
	{
		ts.tv_sec += 1;
		ts.tv_nsec = ts.tv_nsec + uiNanoseconds - MAX_NANO_SEC;
	}
	else
	{
		ts.tv_nsec += uiNanoseconds;
	}
    
    return pthread_mutex_timedlock(mutex,&ts);
}


/*********************************************************************
 * 
 *  sem opration function
 * 
 * 
***********************************************************************/
void lock_SemCreate(sem_t* pstSem,const char * strName)
{
	if(pstSem)
	{
		sem_init(pstSem, 0 /*Only used by current process*/, 0 /*yes, it can run now.*/);
	}
}
void lock_semDetroy(sem_t* pstSem)
{
	sem_destroy(pstSem);
}
void lock_SemPost(sem_t* pstSem,const char * strFunName,UINT32 ulLine)
{
	if(pstSem)
	{
		#ifdef DEBUG_SEM

		#endif
		sem_post (pstSem);
	}
}

void lock_SemWait(sem_t* pstSem,const char * strFunName,UINT32 ulLine)
{
	if(pstSem)
	{
		#ifdef DEBUG_SEM

		#endif
		sem_wait (pstSem);
	}
}
void lock_SemTryWait(sem_t* pstSem,const char * strFunName,UINT32 ulLine)
{
	if(pstSem)
	{
		#ifdef DEBUG_SEM

		#endif
		sem_trywait(pstSem);
	}
}

int lock_SemTimeWait(sem_t* pstSem, UINT32 uiSeconds, UINT32 uiNanoseconds,const char * strFunName,UINT32 ulLine)
{
	#ifdef DEBUG_SEM

	#endif

	struct timespec ts;
	if(clock_gettime(CLOCK_REALTIME,&ts) < 0)
	{
		return RT_FAILED;
	}
	ts.tv_sec  += uiSeconds;
	if(MAX_NANO_SEC < (ts.tv_nsec + uiNanoseconds))
	{
		ts.tv_sec += 1;
		ts.tv_nsec = ts.tv_nsec + uiNanoseconds - MAX_NANO_SEC;
	}
	else
	{
		ts.tv_nsec += uiNanoseconds;
	}

	return  sem_timedwait(pstSem, &ts); 
}



/******************************************************************************************************
 * 
 * spinlock operation
 * 
 * 
 * 
*******************************************************************************************************/
int lock_SpinInit(pthread_spinlock_t *lock,const char * strName)
{
	return pthread_spin_init(lock,0);
}
int lock_SpinDeInit(pthread_spinlock_t *lock,const char * strFunName,UINT32 ulLine)
{
	return pthread_spin_init(lock,0);
}

int lock_SpinLock(pthread_spinlock_t *lock,const char * strFunName,UINT32 ulLine)
{
	return pthread_spin_lock(lock);
}
int lock_SpinTryLock(pthread_spinlock_t *lock,const char * strFunName,UINT32 ulLine)
{
	return pthread_spin_trylock(lock);
} 