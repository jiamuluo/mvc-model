/*
 * @Author: jiamu 
 * @Date: 2018-07-13 15:40:25 
 * @Last Modified by: jiamu
 * @Last Modified time: 2018-07-16 11:35:33
 */

#include <limits.h>

#include "os_task_schedual.h"
#include "os_queue_manage.h"
#include "log.h"
#include "lock.h"

static TASK_RECORD_INFO stTaskInfo[_TASK_MAX];

void os_task_shedualInit(void)
{
    bzero(&stTaskInfo,sizeof(stTaskInfo));
}

static void *os_taskSelfShedual(void *data)
{
    OS_TASK_CTRL *taskInfo = (OS_TASK_CTRL *)data;

    log_setTaskLogLevel(taskInfo->ulLogLevel,taskInfo->eTaskId);
    taskInfo->funcInit(data);

    while(1)
    {
        Q_OS_PEND(&taskInfo->stSemInfo);

        
    }
    return NULL;
}
INT32 os_getTaskSelfInfo(const pthread_t id,OS_TASK_CTRL *pstTask)
{
    if(NULL == pstTask)
    {
        return RT_ARG_INVALID;
    }
    int i = 0;
    for(i = 0;i < _TASK_MAX;i++)
    {
        if(stTaskInfo[i].isAlive != false && stTaskInfo[i].selfId == id)
        {
            memcpy(pstTask,&stTaskInfo[i],sizeof(OS_TASK_CTRL));
            return RT_SUCCESS;
        }
    }
    return RT_FAILED;
}

INT32 os_commonThreadCreate(UINT32 ulStackSize,INT32 slPriority,pthreadCb funcCb,void *threadData)
{

    INT32 slRet = RT_SUCCESS;
	void *paStackAddr = NULL;
	pthread_attr_t stThreadAttr;
	pthread_t      stThreadId = 0;
    struct sched_param stSchedParam;

	UINT32 ulLeastSize = (PTHREAD_STACK_MIN <= ulStackSize) ? ulStackSize : PTHREAD_STACK_MIN; 
    stSchedParam.sched_priority = slPriority;

    pthread_attr_init(&stThreadAttr);
    if(pthread_attr_setstack (&stThreadAttr, paStackAddr, ulLeastSize) != 0)
    {
        printf("set statck size failed \n");
    }

    pthread_attr_setschedpolicy(&stThreadAttr, SCHED_RR);
	pthread_attr_setdetachstate(&stThreadAttr, PTHREAD_CREATE_DETACHED);
	pthread_attr_setschedparam(&stThreadAttr, &stSchedParam);

    if(pthread_create(&stThreadId, &stThreadAttr, funcCb, threadData) < 0)
    {
        slRet = RT_FAILED;
        printf("Create thread failed \n");
    }
    pthread_attr_destroy(&stThreadAttr);

    return slRet < 0 ? slRet : (INT32)stThreadId;
}


INT32 os_taskCreateInit(OS_TASK_CTRL *pstTaskInfo)
{
    if(NULL == pstTaskInfo)
    {
        return RT_ARG_INVALID;
    }   
    //1.队列的初始化
    //队列怎么设计????????

    //2.任务资源
    char ascName[128] = {0};
    bzero(ascName,128);
    sprintf(ascName,"Task %s's mutex",pstTaskInfo->strTaskName);
    LOCK_MUTEX_INIT(&pstTaskInfo->stMutexLock,ascName);

    bzero(ascName,128);
    sprintf(ascName,"Task %s's sem",pstTaskInfo->strTaskName);
    LOCK_SEM_INIT(&pstTaskInfo->stSemInfo,ascName);

    bzero(ascName,128);
    sprintf(ascName,"Task %s's spinlock",pstTaskInfo->strTaskName);
    LOCK_SPIN_INIT(&pstTaskInfo->stSpinLock,ascName);
    
    INT32 slRet = os_commonThreadCreate(pstTaskInfo->ulStatckSize,pstTaskInfo->slPriority,os_taskSelfShedual,(void *)&pstTaskInfo);
    if(slRet < 0)
    {
        LOCK_MUTEX_DEINIT(&pstTaskInfo->stMutexLock);
        LOCK_SEM_DEINIT(&pstTaskInfo->stSemInfo);
        LOCK_SPIN_DEINIT(&pstTaskInfo->stSpinLock);
        SYS_TRACE("Task %s create failed",pstTaskInfo->strTaskName);
        return RT_ARG_INVALID;
    }

    stTaskInfo[pstTaskInfo->eTaskId].isAlive = true;
    stTaskInfo[pstTaskInfo->eTaskId].selfId  = (pthread_t)slRet;
    stTaskInfo[pstTaskInfo->eTaskId].strTaskName = pstTaskInfo->strTaskName;

    SYS_TRACE("Task %s create Success",pstTaskInfo->strTaskName);
    return RT_SUCCESS;
}