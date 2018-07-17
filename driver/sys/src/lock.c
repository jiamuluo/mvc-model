/*
 * @Author: jiamu 
 * @Date: 2018-07-13 17:09:37 
 * @Last Modified by: jiamu
 * @Last Modified time: 2018-07-16 11:57:25
 */

#include "lock.h"
#include <time.h>

static pthread_mutex_t stCritical;
static int lock_RecordInfo(LOCK_TYPE eType,LOCK_ACTION eAction,void *lock,const char *strFun,UINT32 ulLine,const char *strName);

static LOCK_INFO stMutexRecordInfo[MAX_MUTEX_NUM];
static LOCK_INFO stSemRecordInfo[MAX_SEM_NUM];
static LOCK_INFO stSpinRecordInfo[MAX_SPIN_NUM];



INT32 lock_mutexInit(pthread_mutex_t *mutex,const char * strName)
{
    if(mutex && strName != NULL)
	{	
		#ifdef DEBUG_LOCK
		if(lock_RecordInfo(_LOCK_MUTEX,_ACTION_INIT,mutex,NULL,0,strName) < 0)
		{
			SYS_TRACE("%s Init failed,May need extend mutex num",strName);
			exit(-1);
		}
		#endif
		return pthread_mutex_init(mutex,NULL);
	}
    else
        return RT_ARG_INVALID;
}

INT32 lock_mutexDetroy(pthread_mutex_t *mutex)
{
    if(mutex)
	{
		#ifdef DEBUG_LOCK
		if(lock_RecordInfo(_LOCK_MUTEX,_ACTION_DEINIT,mutex,NULL,0,NULL) < 0)
		{
			SYS_TRACE("DeInit failed,May meet a bug");
		}
		#endif
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
		if(lock_RecordInfo(_LOCK_MUTEX,_ACTION_LOCK,mutex,strFunName,ulLine,NULL) < 0)
		{
			SYS_TRACE("Lock failed,May meet a bug");
		}
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
		#ifdef DEBUG_LOCK
		if(lock_RecordInfo(_LOCK_MUTEX,_ACTION_UNLOCK,mutex,strFunName,ulLine,NULL) < 0)
		{
			SYS_TRACE("UnLock failed,May meet a bug");
		}
		#endif
        return pthread_mutex_unlock (mutex); 
    }
    else
        return RT_ARG_INVALID;
	 
}
INT32 lock_mutexTryLock(pthread_mutex_t *mutex,const char * strFunName,UINT32 ulLine)
{
    if(mutex)
    {
		#ifdef DEBUG_LOCK
		if(lock_RecordInfo(_LOCK_MUTEX,_ACTION_LOCK,mutex,strFunName,ulLine,NULL) < 0)
		{
			SYS_TRACE("Lock failed,May meet a bug");
		}
		#endif
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
    #ifdef DEBUG_LOCK
	if(lock_RecordInfo(_LOCK_MUTEX,_ACTION_LOCK,mutex,strFunName,ulLine,NULL) < 0)
	{
		SYS_TRACE("Lock failed,May meet a bug");
	}
	#endif
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
		//SYS_CRIT();
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


/**
 * @brief 
 * 	record lock running information but exclude system lock(task's timer's memory's)
 * @param eType 
 * @param eAction 
 * @param lock 
 * @param strFun 
 * @param ulLine 
 * @param strName 
 * @return int 
 */
static int lock_RecordInfo(LOCK_TYPE eType,LOCK_ACTION eAction,void *lock,const char *strFun,UINT32 ulLine,const char *strName)
{
	int i = 0;
	//UINT32 ulNub = _LOCK_MUTEX == eType ? MAX_MUTEX_NUM : (_LOCK_SEM == eType ? MAX_SEM_NUM : MAX_SPIN_NUM);
	//LOCK_INFO *pstLockInfo = _LOCK_MUTEX == eType ? &stMutexRecordInfo : (_LOCK_SEM == eType ? &stSemRecordInfo : &stSpinRecordInfo);

	LOCK_INFO *pstLockInfo = NULL;
	UINT32 ulNub = 0;

	#ifdef DEBUG_LOCK
	if( _LOCK_MUTEX == eType )
	{
		ulNub = MAX_MUTEX_NUM;
		pstLockInfo = (LOCK_INFO *)&stMutexRecordInfo;
	}
	#endif

	#ifdef DEBUG_SEM
	if( _LOCK_SEM == eType )
	{
		ulNub = MAX_SEM_NUM;
		pstLockInfo = (LOCK_INFO *)&stSemRecordInfo;
	}
	#endif

	#ifdef DEBUG_SPIN
	if( _LOCK_SEM == eType )
	{
		ulNub = MAX_SPIN_NUM;
		pstLockInfo = (LOCK_INFO *)&stSpinRecordInfo;
	}
	#endif

	if(NULL == pstLockInfo || 0 == ulNub)
	{
		return RT_ARG_INVALID;
	}
    
	const char *strAction[_ACTION_ALL + 1] = {[1] = "Init","Deinit","Lock","Unlock"};

	if(eAction > _ACTION_DEINIT)
	{
		for(i = 0;i < ulNub;i++)
		{
			if(pstLockInfo[i].isAlive != false && pstLockInfo[i].lock == lock)
				break;
		}
		if(i == ulNub)
			return RT_ARG_INVALID;
		
		SYS_CRIT("Lock:%s Action:%s Fun:%s Line:%d",pstLockInfo[i].strName,strAction[eAction],strFun,ulLine);

	}
	else
	{
		if(_ACTION_DEINIT == eAction)
		{
			for(i = 0;i < ulNub;i++)
			{
				if(pstLockInfo[i].isAlive == true && pstLockInfo[i].lock == lock)
				{
					pstLockInfo[i].isAlive = false;
					pstLockInfo[i].lock   = NULL;
					bzero(pstLockInfo[i].strName,sizeof(pstLockInfo[i].strName));
					break;
				}
			}
			if(i == ulNub)
				return RT_ARG_INVALID;
		}
		else
		{
			for(i = 0;i < ulNub;i++)
			{
				if(pstLockInfo[i].isAlive == false && pstLockInfo[i].lock == NULL)
				{
					pstLockInfo[i].isAlive = true;
					pstLockInfo[i].lock    = lock;
					strncpy(pstLockInfo[i].strName,strName,128);
					break;
				}
			}
			if(i == ulNub)
			{
				SYS_TRACE("Lock is full");
				for(i = 0;i < ulNub;i++)
				{
					SYS_TRACE("%d:%s",i,pstLockInfo[i].strName);
				}
				return RT_ARG_INVALID;
			}
				
		}
	}
		
	return RT_SUCCESS;
}

void lock_init(void)
{
	int i = 0;
	

	#ifdef DEBUG_LOCK
	
	for(i = 0;i < MAX_MUTEX_NUM;i++)
	{
		stMutexRecordInfo[i].isAlive = false;
		stMutexRecordInfo[i].lock   = NULL;
		bzero(stMutexRecordInfo[i].strName,sizeof(stMutexRecordInfo[i].strName));
	}
	#endif

	#ifdef DEBUG_SEM
	for(i = 0;i < MAX_SEM_NUM;i++)
	{
		stSemRecordInfo[i].isAlive = false;
		stSemRecordInfo[i].lock   = NULL;
		bzero(stSemRecordInfo[i].strName,sizeof(stSemRecordInfo[i].strName));
	}
	#endif

	#ifdef DEBUG_SPIN
	for(i = 0;i < MAX_SPIN_NUM;i++)
	{
		stSpinRecordInfo[i].isAlive = false;
		stSpinRecordInfo[i].lock   = NULL;
		bzero(stSpinRecordInfo[i].strName,sizeof(stSpinRecordInfo[i].strName));
	}

	#endif

	pthread_mutex_init(&stCritical,NULL);
}

void os_enter_critical(const char * strFunName,UINT32 ulLine)
{
	pthread_mutex_lock (&stCritical); 
	SYS_CRIT("%s %d enter critical",strFunName,ulLine);
}

void os_levea_critical(const char * strFunName,UINT32 ulLine)
{
	pthread_mutex_unlock (&stCritical); 
	SYS_CRIT("%s %d leave critical",strFunName,ulLine);
}