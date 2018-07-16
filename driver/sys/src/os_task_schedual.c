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
   
    OS_TASK_CTRL *pstTaskInfo = (OS_TASK_CTRL *)data;
    char ascName[128] = {0};
    bzero(ascName,128);
    sprintf(ascName,"Task %s's mutex",pstTaskInfo->strTaskName);
    LOCK_MUTEX_INIT(&pstTaskInfo->stMutexLock,ascName);

    bzero(ascName,128);
    sprintf(ascName,"Task %s's sem",pstTaskInfo->strTaskName);
    LOCK_SEM_INIT(&pstTaskInfo->stSemInfo,ascName);
    LOCK_SEM_RESET(&pstTaskInfo->stSemInfo);

    bzero(ascName,128);
    sprintf(ascName,"Task %s's spinlock",pstTaskInfo->strTaskName);
    LOCK_SPIN_INIT(&pstTaskInfo->stSpinLock,ascName);

    printf("task %s is running \n",pstTaskInfo->strTaskName);
    log_setTaskLogLevel(pstTaskInfo->ulLogLevel,pstTaskInfo->eTaskId);

    os_create_taskque(pstTaskInfo);

    pstTaskInfo->funcInit(data);

    while(1)
    {
        printf("task %s is pending message  \n",pstTaskInfo->strTaskName);
        Q_OS_PEND(&pstTaskInfo->stSemInfo);

        os_queue_handle_msg(pstTaskInfo);
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

INT32 os_commonThreadCreate(UINT32 ulStackSize,INT32 slPriority,pthreadCb funcCb,void *threadData,pthread_t *id)
{

    INT32 slRet = RT_SUCCESS;
	pthread_attr_t stThreadAttr;
	pthread_t      stThreadId = 0;
    struct sched_param stSchedParam;

	UINT32 ulLeastSize = (PTHREAD_STACK_MIN <= ulStackSize) ? ulStackSize : PTHREAD_STACK_MIN; 
    stSchedParam.sched_priority = slPriority;

    pthread_attr_init(&stThreadAttr);
    if(pthread_attr_setstacksize (&stThreadAttr, ulLeastSize) != 0)
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
    *id = stThreadId;

    return slRet;
}


INT32 os_taskCreateInit(OS_TASK_CTRL *pstTaskInfo)
{
    if(NULL == pstTaskInfo)
    {
        return RT_ARG_INVALID;
    }   
  
    //1.任务资源
    printf("Now need create task %s \n",pstTaskInfo->strTaskName);
    pthread_t id;
    INT32 slRet = os_commonThreadCreate(pstTaskInfo->ulStatckSize,pstTaskInfo->slPriority,os_taskSelfShedual,(void *)pstTaskInfo,&id);
    if(slRet < 0)
    {
        SYS_TRACE("Task %s create failed",pstTaskInfo->strTaskName);
        exit(-1);
        return RT_ARG_INVALID;
    }

    stTaskInfo[pstTaskInfo->eTaskId].isAlive = true;
    stTaskInfo[pstTaskInfo->eTaskId].selfId  = (pthread_t)id;
    stTaskInfo[pstTaskInfo->eTaskId].strTaskName = pstTaskInfo->strTaskName;

    SYS_TRACE("Task %s create Success",pstTaskInfo->strTaskName);
    return RT_SUCCESS;
}