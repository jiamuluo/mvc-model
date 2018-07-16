/*
 * @Author: jiamu 
 * @Date: 2018-07-13 17:09:46 
 * @Last Modified by: jiamu
 * @Last Modified time: 2018-07-16 11:36:30
 */

#ifndef _LOCK_H_
#define _LOCK_H_
#include "os_types.h"
#include "log.h"


#define MAX_MUTEX_NUM 20

typedef struct
{
    bool isAlive;
    pthread_mutex_t *mutex;
    INT8 strName[128];
}MUTEX_INFO;

INT32 lock_mutexInit(pthread_mutex_t *mutex,const char * strName);
INT32 lock_mutexDetroy(pthread_mutex_t *mutex);
INT32 lock_mutexLock(pthread_mutex_t *mutex,const char *  strFunName,UINT32 ulLine);
INT32 lock_mutexUnLock(pthread_mutex_t *mutex,const char * strFunName,UINT32 ulLine);
INT32 lock_mutexTryLock(pthread_mutex_t *mutex,const char * strFunName,UINT32 ulLine);
INT32 lock_mutexTimeLock(pthread_mutex_t *mutex,UINT32 ulTime/*us*/,const char * strFunName,UINT32 ulLine);

#define LOCK_MUTEX_INIT(mutex,name)             lock_mutexInit(mutex,name)
#define LOCK_MUTEX_DEINIT(mutex)                lock_mutexDetroy(mutex)
#define LOCK_MUTEX_UNLOCK(mutex)                lock_mutexUnLock(mutex,__FUNCTION__,__LINE__)
#define LOCK_MUTEX_LOCK(mutex)                  lock_mutexLock(mutex,__FUNCTION__,__LINE__)
#define LOCK_MUTEX_TRYLOCK(mutex)               lock_mutexTryLock(mutex,__FUNCTION__,__LINE__)
#define LOCK_MUTEX_TIMELOCK(mutex,time)         lock_mutexTimeLock(mutex,time,__FUNCTION__,__LINE__)


void lock_SemCreate(sem_t* pstSem,const char * strName);
void lock_semDetroy(sem_t* pstSem);
void lock_SemPost(sem_t* pstSem,const char * strFunName,UINT32 ulLine);
void lock_SemWait(sem_t* pstSem,const char * strFunName,UINT32 ulLine);
void lock_SemTryWait(sem_t* pstSem,const char * strFunName,UINT32 ulLine);
int lock_SemTimeWait(sem_t* pstSem, UINT32 uiSeconds, UINT32 uiNanoseconds,const char * strFunName,UINT32 ulLine);

#define LOCK_SEM_INIT(sem,name)                 lock_SemCreate(sem,name)
#define LOCK_SEM_DEINIT(sem)               lock_semDetroy(sem)
#define LOCK_SEM_POST(sem)                      lock_SemPost(sem,__FUNCTION__,__LINE__)
#define LOCK_SEM_WAIT(sem)                      lock_SemWait(sem,__FUNCTION__,__LINE__)
#define LOCK_SEM_TRY(sem)                       lock_SemTryWait(sem,__FUNCTION__,__LINE__)
#define LOCK_SEM_TIME_WAIT(sem,secons,nano)     lock_SemTimeWait(sem,secons,nano,__FUNCTION__,__LINE__)
#define LOCK_SEM_RESET(sem)                     LOCK_SEM_TRY(sem)  


int lock_SpinInit(pthread_spinlock_t *lock,const char * strName);
int lock_SpinDeInit(pthread_spinlock_t *lock,const char * strFunName,UINT32 ulLine);
int lock_SpinLock(pthread_spinlock_t *lock,const char * strFunName,UINT32 ulLine);
int lock_SpinTryLock(pthread_spinlock_t *lock,const char * strFunName,UINT32 ulLine);


#define LOCK_SPIN_INIT(lock,name)             lock_SpinInit(lock,name)
#define LOCK_SPIN_DEINIT(lock)                lock_SpinDeInit(lock,__FUNCTION__,__LINE__)
#define LOCK_SPIN_LOCK(lock)                  lock_SpinLock(lock,__FUNCTION__,__LINE__)
#define LOCK_SPIN_TRYLOCK(lock)               lock_SpinTryLock(lock,__FUNCTION__,__LINE__)

void lock_init(void);   
#endif


