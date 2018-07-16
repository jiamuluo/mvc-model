/*
 * @Author: jiamu 
 * @Date: 2018-07-16 13:35:59 
 * @Last Modified by: jiamu
 * @Last Modified time: 2018-07-16 13:36:19
 */

#include "os_queue_manage.h"
#include "os_mem_manage.h"

static OS_QUEUE stQueueManager[_TASK_MAX];

int os_create_taskque(OS_TASK_CTRL* pstTaskInfo)
{
    if(pstTaskInfo->eTaskId >= _TASK_MAX)
    {
        SYS_TRACE("Create inccrect task %d queue ",pstTaskInfo->eTaskId);
        return RT_ARG_INVALID;
    }
    stQueueManager[pstTaskInfo->eTaskId].ulNbrEntriesSize   = pstTaskInfo->ulQueLen;
    stQueueManager[pstTaskInfo->eTaskId].ulNbrEntries       = 0;
    stQueueManager[pstTaskInfo->eTaskId].ulNbrEntriesMax    = 0;
    stQueueManager[pstTaskInfo->eTaskId].pstQueueMutexLock  = &pstTaskInfo->stMutexLock;
	stQueueManager[pstTaskInfo->eTaskId].pstSemInfo 		= &pstTaskInfo->stSemInfo;

    snprintf(stQueueManager[pstTaskInfo->eTaskId].ascQueueName,sizeof(stQueueManager[pstTaskInfo->eTaskId].ascQueueName),"Task %s's queue",pstTaskInfo->strTaskName);

    INIT_LIST_HEAD(&stQueueManager[pstTaskInfo->eTaskId].stMsgQ.stListEntry);

    return RT_SUCCESS;
}
/**
 * @brief 
 * 	queue is a simple fifo
 * @param eDstTask 
 * @param isTimer 
 * @param eMsgType 
 * @param ulMsgSize 
 * @param p_void 
 * @return int 
 */
int os_send_msg(OS_TASK_ID eDstTask ,bool isTimer,OS_QUE_MSG_TYPE eMsgType,UINT32 ulMsgSize,void *p_void)
{
    if(eDstTask >= _TASK_MAX)
    {
        SYS_TRACE("Send message to incorrect dst %d ",eDstTask);
        return RT_ARG_INVALID;
    }
    OS_QUEUE *pstQue = &stQueueManager[eDstTask];
    OS_MSG_Q *pstMsg =  OS_MEM_MALLOC(sizeof(OS_MSG_Q));

    LOCK_MUTEX_LOCK(pstQue->pstQueueMutexLock);

    if(pstQue->ulNbrEntries >= pstQue->ulNbrEntriesSize)
    {
        SYS_TRACE("Send message to  dst %d ,but task queue is fill,msg is %d ",eDstTask,eMsgType);
        LOCK_MUTEX_UNLOCK(pstQue->pstQueueMutexLock);
        OS_MEM_FREE(pstMsg);
        return RT_ARG_INVALID;
    }
    pstMsg->eTaskDst  = eDstTask;
    pstMsg->isTimer   = isTimer;
	
	pstMsg->stMsgInfo.eMsgType  = eMsgType;
    pstMsg->stMsgInfo.ulMsgSize = ulMsgSize;
    pstMsg->stMsgInfo.p_void    = p_void;

    list_add_tail(&pstMsg->stListEntry,&pstQue->stMsgQ.stListEntry);
    pstQue->ulNbrEntries++;
    if(pstQue->ulNbrEntries > pstQue->ulNbrEntriesMax)
    {
         pstQue->ulNbrEntriesMax = pstQue->ulNbrEntries;
    }
    LOCK_MUTEX_UNLOCK(pstQue->pstQueueMutexLock);

    LOCK_SEM_POST(pstQue->pstSemInfo);

    return RT_SUCCESS;
}
int os_queue_handle_msg(OS_TASK_CTRL* pstTaskInfo)
{
    if(pstTaskInfo->eTaskId >= _TASK_MAX)
    {
        SYS_TRACE("Handle incocrect task %d queue ",pstTaskInfo->eTaskId);
        return RT_ARG_INVALID;
    }
    OS_QUEUE *pstQue = &stQueueManager[pstTaskInfo->eTaskId];
	LIST_S *pstTmp   = NULL;
	OS_MSG_Q *pstMsg = NULL;

	UINT32 ulIndex = 0;
	UINT32 ulQuote = 5;
	OS_MSG_Q *pstRecord[OS_QUEUE_MAX] = {NULL};

	LOCK_MUTEX_LOCK(pstQue->pstQueueMutexLock);
	list_for_each(pstTmp,&pstQue->stMsgQ.stListEntry)
	{
		pstMsg = NULL;
		if((pstMsg = (OS_MSG_Q *)list_entry(pstTmp,OS_MSG_Q,stListEntry)) != NULL)
		{
			if(pstMsg->isTimer == false)
			{
				pstTaskInfo->funcMsg(&pstMsg->stMsgInfo,pstTaskInfo->pTaskPri);
			}
			else
			{
				pstTaskInfo->funcTimer(&pstMsg->stMsgInfo,pstTaskInfo->pTaskPri);
			}
		}
		pstRecord[ulIndex++] = pstMsg;
		if(OS_QIEIE_QUOTE < ulIndex)
		{
			usleep(10);
		}
		//OS_MEM_FREE(pstMsg->stMsgInfo.p_void); free by user
	}
	UINT32 i = 0;
	for(i = 0;i < ulIndex && pstRecord[i] != NULL;i++)
	{
		list_del(&pstRecord[i]->stListEntry);
		OS_MEM_FREE(pstRecord[i]);
		pstRecord[i] = NULL;
	}
	LOCK_MUTEX_UNLOCK(pstQue->pstQueueMutexLock);

    return RT_SUCCESS;
}

void os_queue_init(void)
{
    bzero(&stQueueManager,sizeof(stQueueManager));
}
