/*
 * @Author: jiamu 
 * @Date: 2018-06-29 14:57:02 
 * @Last Modified by: jiamu
 * @Last Modified time: 2018-07-16 11:56:18
 */


#include "os_timer_schedual.h"
#include "list.h"
#include "log.h"
#include "lock.h"

static UINT32 ulTimerCurTick = 0;
static UINT32 ulTimeRate = 0;
static OS_TIMER_POOL stTimerPool[OS_TIMER_POOL_SIZE];
static pthread_mutex_t  stMutexForTimer;


static INT32 os_TimerPutToPool(OS_TIMER_SCHEDUAL_INFO *pstTimerInfo);
static void os_TimerQueryExpire(void);

void *os_timerSchedualTask(void *data)
{

    while(1)
    {
        usleep(ulTimeRate);
		LOCK_MUTEX_LOCK(&stMutexForTimer);
		ulTimerCurTick++;
		os_TimerQueryExpire();
		LOCK_MUTEX_UNLOCK(&stMutexForTimer);
    }
}

INT32 os_timerSchedualInit(UINT32 ulTimerRateMs)
{
    UINT32 i = 0;
	pthread_t  timerThreadId = 0;
    bzero(stTimerPool,sizeof(stTimerPool));

	if(0 == ulTimeRate)
	{
		return RT_SUCCESS;
	}

	for(i = 0;i < OS_TIMER_POOL_SIZE;i++)
	{
		INIT_LIST_HEAD(&stTimerPool[i].stTimerSchedual.stListEntry);
		stTimerPool[i].usMaxEntry  = OS_TIMER_LIST_SIZE;
	}

	LOCK_MUTEX_INIT(&stMutexForTimer,"Timer’s mutex");
    ulTimerCurTick = 0;
    ulTimeRate     = (ulTimerRateMs > 1000) ? (500 * 1000) : (ulTimerRateMs * 1000);
    //if (FUNC_NULL == func_CreateAThread(PRIORITY_TIMER, 16*1024, os_timerSchedualTask, NULL, MID_APP_TIMER))
	if(pthread_create(&timerThreadId,NULL,os_timerSchedualTask,NULL) !=0 )
    {
		SYS_TRACE("Create timer sys failed");
    	return RT_FAILED;
    } 
	return RT_SUCCESS;
}
static void os_TimerQueryExpire(void)
{
	UINT32 i = 0,j = 0;
	UINT32 ulExpireNum = 0;
	OS_TIMER_SCHEDUAL_INFO *pstTimerNode = NULL;
	OS_TIMER_SCHEDUAL_INFO *stExpireTime[OS_TIMER_POOL_SIZE * OS_TIMER_LIST_SIZE] = {NULL};
	

	for(i = 0;i < OS_TIMER_POOL_SIZE;i++)
	{
		for(j = 0;j < stTimerPool[i].usCurrentEntry;j++)
		{
			//UINT8 ucFindNode = V_DISABLE;
			LIST_S *pstTmp = NULL;
		

			list_for_each(pstTmp,&stTimerPool[i].stTimerSchedual.stListEntry)
			{
				pstTimerNode = NULL;
				pstTimerNode = list_entry(pstTmp,OS_TIMER_SCHEDUAL_INFO,stListEntry);
				if(NULL == pstTimerNode)
				{
					break;
				}
				if(0 == pstTimerNode->ulRemain)
				{
					stExpireTime[ulExpireNum++] = pstTimerNode;
				}
				pstTimerNode->ulRemain--; //Need query all node
			}
		}
	}

	for(i = 0;i < ulExpireNum;i++)
	{
		pstTimerNode = stExpireTime[i];
		list_del(&pstTimerNode->stListEntry);
		
		#if 0
		FUNC_MSG_S *pstMsg	= NULL;
		pstMsg = (FUNC_MSG_S *)OS_MEM_MALLOC(sizeof(FUNC_MSG_S) + 4);	
		//FUNC_ASSERT(FUNC_NULL != pstMsg);
		func_MemSet(pstMsg, 0x00, sizeof(FUNC_MSG_S) + 4);
	
		pstMsg->eMidTo = pstTimerNode->eHandleTaskId;
		pstMsg->eMidFrom = MID_APP_TIMER;
		pstMsg->ulMessageId = pstTimerNode->eTimerMsgType;
		pstMsg->ulMsgType   = FUNC_MESSAGE_TYPE_TIMER;
		pstMsg->usLength    = sizeof(UINT32);
		memcpy(pstMsg->ucBody,&pstTimerNode->pData,sizeof(UINT32));
		func_SendMessage(pstMsg);
		#endif
		os_send_msg(pstTimerNode->eHandleTaskId,true,pstTimerNode->eTimerMsgType,pstTimerNode->ulMsgSize,pstTimerNode->pData);

		if(OS_OPT_TMR_ONE_SHOT == pstTimerNode->eTimerType)
		{
			OS_MEM_FREE(pstTimerNode);
			stExpireTime[i] = NULL;
		}
	}
	
	for(i = 0;i < ulExpireNum && stExpireTime[i] != NULL;i++)
	{
		pstTimerNode = stExpireTime[i];
		pstTimerNode->ulRemain = pstTimerNode->ulDelay;
		os_TimerPutToPool(pstTimerNode);
	}
}
static INT32 os_TimerPutToPool(OS_TIMER_SCHEDUAL_INFO *pstTimerInfo)
{
	UINT32 ulIndex = pstTimerInfo->ulRemain % OS_TIMER_POOL_SIZE;
	
	if(list_empty_careful(&stTimerPool[ulIndex].stTimerSchedual.stListEntry))
	{
		list_add_tail(&pstTimerInfo->stListEntry,&stTimerPool[ulIndex].stTimerSchedual.stListEntry);
		stTimerPool[ulIndex].usCurrentEntry = 1;
	}
	else
	{
		UINT8 ucFindNode = V_DISABLE;
		LIST_S *pstTmp = NULL;
		OS_TIMER_SCHEDUAL_INFO *pstTimerNode = NULL;
		if(stTimerPool[ulIndex].usCurrentEntry > OS_TIMER_LIST_SIZE)
		{
			return RT_FAILED;
		}
		list_for_each(pstTmp,&stTimerPool[ulIndex].stTimerSchedual.stListEntry)
		{
			pstTimerNode = list_entry(pstTmp,OS_TIMER_SCHEDUAL_INFO,stListEntry);
			if(NULL == pstTimerNode)
			{
				return RT_FAILED;
			}

			if(pstTimerNode->ulRemain > pstTimerInfo->ulRemain) /*This way can avoid value of uint32 overflow*/
			{
				continue;
			}
			else //if(pstTimerNode->ulRemain == pstTimerInfo->ulRemain)  //
			{
				ucFindNode = V_ENABLE;
				break;
			}
			
		}
		if(V_ENABLE == ucFindNode)
		{
			list_add_pre(&pstTimerInfo->stListEntry,&pstTimerNode->stListEntry);
		}
		else
		{
			list_add_tail(&pstTimerInfo->stListEntry,&stTimerPool[ulIndex].stTimerSchedual.stListEntry);
		}
		stTimerPool[ulIndex].usCurrentEntry++;
	}

	return RT_SUCCESS;
}
/**
 * @brief 
 * 
 * @param ulTimeMs 
 * @param eTimerType 
 * @param eTimerMsgType 
 * @param eTaskId 
 * @param pData 
 * @return INT32 
 */
INT32 os_timerAddNewOne(UINT32 ulTimeMs,OS_TIMER_TYPE eTimerType,OS_TIMER_MSG_TYPE eTimerMsgType,OS_TASK_ID eTaskId,UINT32 ulMsgSize,void *pData)
{
	if(0 == ulTimeMs || 0 == ulTimeRate)
	{
		return RT_FAILED;
	}
	INT32 slRet = RT_SUCCESS;

	UINT32 ulDelay = ulTimeMs < ulTimeRate ? 1 : (ulTimeMs / ulTimeRate);

	OS_TIMER_SCHEDUAL_INFO *pstTimerInfo = OS_MEM_MALLOC(sizeof(OS_TIMER_SCHEDUAL_INFO));
	if(NULL == pstTimerInfo)
	{
		return RT_MALLOC_FAILD;
	}
	memset(pstTimerInfo,0,sizeof(OS_TIMER_SCHEDUAL_INFO));

	LOCK_MUTEX_LOCK(&stMutexForTimer);
	pstTimerInfo->ulDelay  = ulDelay;
	pstTimerInfo->ulExpire = ulDelay + ulTimerCurTick;
	pstTimerInfo->ulRemain = ulDelay;
	pstTimerInfo->eTimerType = eTimerType;
	pstTimerInfo->eTimerState = OS_TMR_STATE_STOPPED;
	pstTimerInfo->eTimerMsgType = eTimerMsgType;
	pstTimerInfo->eHandleTaskId = eTaskId;
	pstTimerInfo->pData         = pData;
	slRet = os_TimerPutToPool(pstTimerInfo);
	LOCK_MUTEX_UNLOCK(&stMutexForTimer);

	if(slRet < 0)
	{
		OS_MEM_FREE(pstTimerInfo);
	}
	return slRet;
}