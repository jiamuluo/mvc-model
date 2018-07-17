/*
 * @Author: jiamu 
 * @Date: 2018-07-13 15:38:28 
 * @Last Modified by: jiamu
 * @Last Modified time: 2018-07-16 13:26:10
 */

#ifndef _OS_TASK_H_
#define _OS_TASK_H_

#include "os_types.h"

//Need user define
typedef enum
{ 
	_TASK_JIAMU = 0,


	
	_TASK_SYS,//add view task before this,other common thread after this

	_TASK_MAX
}OS_TASK_ID;

typedef struct
{
    UINT32  eMsgType; //nedd compate queue msg and timer msg
    UINT32 ulMsgSize;
    void *p_void;
}MSG_TLV_INFO;


typedef void (*taskInit)(void *);
typedef void (*taskHandleMsg)(void *,void *);
typedef void (*taskHandleTimer)(void *, void *);

typedef struct
{
	const char *    strTaskName;
	OS_TASK_ID eTaskId;

	INT32     slPriority;
	UINT32    ulStatckSize;
	UINT16    ulQueLen;
    UINT32    ulLogLevel;
	
	taskInit        funcInit;
	taskHandleMsg   funcMsg;
	taskHandleTimer funcTimer;

	void *pTaskPri;

	pthread_spinlock_t stSpinLock;
	//for message queue,not provide to user
	pthread_mutex_t stMutexLock;
	sem_t           stSemInfo;
}OS_TASK_CTRL;

typedef struct
{
	char strTaskName[64];
	pthread_t  selfId;
	bool       isAlive;
}TASK_RECORD_INFO;


typedef void *(pthreadCb)(void *);

INT32 os_getTaskSelfInfo(const pthread_t id,OS_TASK_CTRL *pstTask);
INT32 os_commonThreadCreate(UINT32 ulStackSize,INT32 slPriority,pthreadCb funcCb,void *threadData,pthread_t *id,const char *name);
INT32 os_taskCreateInit(OS_TASK_CTRL *pstTaskInfo);
#endif
