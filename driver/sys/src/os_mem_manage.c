/*
 * @Author: jiamu 
 * @Date: 2018-06-29 15:42:38 
 * @Last Modified by: jiamu
 * @Last Modified time: 2018-07-16 11:21:28
 */


#include "os_mem_manage.h"
#include <stdlib.h>
#include "lock.h"
#include "tool_time.h"

static pthread_mutex_t stMemMutexLock;
static OS_MEM_POOL     stMemPool[OS_MEM_POOL_SIZE];

void *os_mem_malloc(UINT32 ulSize,const char* strFile,const char* strFun,const UINT32 ulLine)
{
    UINT32 ulHash = ulSize % OS_MEM_POOL_SIZE;
    OS_MEM_CTRL *pstMemInfo = NULL;
    LIST_S *pstTmp = NULL;

    //find hash table
   // LOCK_MUTEX_LOCK(&stMemMutexLock);
    pthread_mutex_unlock(&stMemMutexLock);
    if(!list_empty_careful(&stMemPool[ulHash].stMemMangeHead.stListEntry))
    {
       
        bool isFind = false;
        list_for_each(pstTmp,&stMemPool[ulHash].stMemMangeHead.stListEntry)
        {
            pstMemInfo = NULL;
            if(((pstMemInfo = (OS_MEM_CTRL *)list_entry(pstTmp,OS_MEM_CTRL,stListEntry)) != NULL)
                && pstMemInfo->isBusy != true 
                && pstMemInfo->ulSize >= ulSize)
            {
                isFind = true;
                break;
            }
        }
        if(true == isFind && pstMemInfo != NULL)
        {
           // list_del(pstMemInfo->stListEntry);

            pstMemInfo->isBusy = true;
            goto mem_malloc;
        }
    }
       
    if(stMemPool[ulHash].usCurrentEntry >= stMemPool[ulHash].usMaxEntry)
    {
        SYS_TRACE("Hash %d need to extend ",ulHash);
        //LOCK_MUTEX_UNLOCK(&stMemMutexLock);
        pthread_mutex_unlock(&stMemMutexLock);
        return NULL;
    }
    pstMemInfo = malloc(sizeof(OS_MEM_CTRL) + ulSize);
    if(NULL == pstMemInfo)
    {
        //LOCK_MUTEX_UNLOCK(&stMemMutexLock);
        pthread_mutex_unlock(&stMemMutexLock);
        return NULL;
    }
    memset(pstMemInfo,0,sizeof(OS_MEM_CTRL) + ulSize);
    pstMemInfo->isBusy  = true;
    pstMemInfo->ulSize = ulSize;
    pstMemInfo->ulHash = ulHash;
    bool isFind = false;

    OS_MEM_CTRL *pstNodeMemInfo = NULL;
    isFind = false;
    list_for_each(pstTmp,&stMemPool[ulHash].stMemMangeHead.stListEntry)
    {
        pstNodeMemInfo = NULL;
        if(((pstNodeMemInfo = list_entry(pstTmp,OS_MEM_CTRL,stListEntry)) != NULL) 
            && pstNodeMemInfo->ulSize < ulSize)
        {
            isFind = true;
            break;
        }
    }

    list_add_pre(&pstMemInfo->stListEntry,true == isFind ? &pstNodeMemInfo->stListEntry : &stMemPool[ulHash].stMemMangeHead.stListEntry);
    stMemPool[ulHash].usCurrentEntry++;

mem_malloc:
    #ifdef DEBUG_MEM
    memset(pstMemInfo->ascFuncName,0,sizeof(pstMemInfo->ascFuncName));
    memset(pstMemInfo->ascTime,0,sizeof(pstMemInfo->ascTime));
    strncpy(pstMemInfo->ascFuncName,strFun,sizeof(pstMemInfo->ascFuncName));
    time_GetCurrentTimeus(pstMemInfo->ascTime,sizeof(pstMemInfo->ascTime));
    pstMemInfo->ulLine = ulLine;
    SYS_CRIT("MEM POOL %d alloc a block size %d ,Line %d Function:%s Time:%s", pstMemInfo->ulHash,pstMemInfo->ulSize,ulLine,strFun,pstMemInfo->ascTime);
    #endif
    pstMemInfo->ulMagic = MEM_MAGIC_NUM;
    //LOCK_MUTEX_UNLOCK(&stMemMutexLock);
    pthread_mutex_unlock(&stMemMutexLock);
    memset(pstMemInfo->mem,ulSize,0);
    return pstMemInfo->mem;
}
/**
 * @brief 
 *  
 * @param ptr 
 * @param strFile 
 * @param strFun 
 * @param ulLine 
 */
void os_mem_free(void *ptr,const char* strFile,const char* strFun,const UINT32 ulLine)
{
    if(NULL == ptr)
    {
        SYS_TRACE("%s %d free null mem ",strFun,ulLine);
        return;
    }
    OS_MEM_CTRL *pstFind = container_of(ptr,OS_MEM_CTRL,mem);
    if(NULL != pstFind && pstFind->isBusy == true &&
     pstFind->ulHash < OS_MEM_POOL_SIZE && 
     pstFind->ulMagic == MEM_MAGIC_NUM && 
     pstFind->mem == ptr)
     {
        #if 0
        int i = 0;
        bool isFind = false;
        UINT32 ulHash = pstFind->ulHash;
        OS_MEM_CTRL *pstMemInfo = NULL;
        LIST_S *pstTmp = NULL;

       
        list_for_each(pstTmp,stMemPool[ulHash].stListEntry)
        {
             pstMemInfo = NULL;
            if((pstMemInfo = list_entry(pstTmp,OS_MEM_CTRL,stListEntry) != NULL) 
                && pstMemInfo->ulSize < ulSize)
            {
                isFind = true;
                break;
            }
        }
        #endif
        
        //LOCK_MUTEX_LOCK(&stMemMutexLock);
        pthread_mutex_lock(&stMemMutexLock);
        pstFind->ulMagic = 0;
        pstFind->isBusy  = false;
        //list_add_pre(&pstFind->stListEntry,true == isFind ? &pstMemInfo->stListEntry : &stMemPool[ulHash].stListEntry);
        #ifdef DEBUG_MEM
        memset(pstFind->ascFuncName,0,sizeof(pstFind->ascFuncName));
        memset(pstFind->ascTime,0,sizeof(pstFind->ascTime));
        strncpy(pstFind->ascFuncName,strFun,sizeof(pstFind->ascFuncName));
        time_GetCurrentTimeus(pstFind->ascTime,sizeof(pstFind->ascTime));
        pstFind->ulLine = ulLine;
        SYS_CRIT("MEM POOL %d realse a block size %d ,Line %d Function:%s Time:%s", pstFind->ulHash,pstFind->ulSize,ulLine,strFun,pstFind->ascTime);
        #endif
        //LOCK_MUTEX_UNLOCK(&stMemMutexLock);
        pthread_mutex_unlock(&stMemMutexLock);
     }
     else
     {
         SYS_TRACE("%s %d free invalid mem ",strFun,ulLine);
     }
      
}

void os_mem_check(void)
{
    UINT32 i = 0,j = 0;
    UINT32 ulFreeNum = 0;
    OS_MEM_CTRL *pstMemInfo = NULL;
    LIST_S *pstTmp = NULL;

    LOCK_MUTEX_LOCK(&stMemMutexLock);
    for(i = 0;i < OS_MEM_POOL_SIZE;i++)
    {
        ulFreeNum  = 0;
        SYS_TRACE("******MEM POOL %d has total %d block,max is %d *********",i,stMemPool[i].usCurrentEntry,stMemPool[i].usMaxEntry);
        list_for_each(pstTmp,&stMemPool[i].stMemMangeHead.stListEntry)
        {
            pstMemInfo = NULL;
        
            if((pstMemInfo = list_entry(pstTmp,OS_MEM_CTRL,stListEntry)) != NULL) 
            {
                const char *strStatus = "Status Busy,alloced by";
                if(pstMemInfo->isBusy != true)
                {
                    strStatus = "Status Free,relesed by";
                    ulFreeNum++;
                }
                SYS_TRACE("%d: %s Line: %d Function:%s Time:%s",j,strStatus,pstMemInfo->isBusy,pstMemInfo->ulLine,pstMemInfo->ascFuncName,pstMemInfo->ascTime);
                j++;
            }
        }
        SYS_TRACE("********MEM POOL %d has free block %d ******************",ulFreeNum);
    }
    LOCK_MUTEX_UNLOCK(&stMemMutexLock);
}
INT32 os_mem_init(void)
{
    int i = 0;
    for(i = 0;i < OS_MEM_POOL_SIZE;i++)
    {
       stMemPool[i].usMaxEntry      = OS_MEM_LIST_SIZE;
       stMemPool[i].usCurrentEntry  = 0;
       INIT_LIST_HEAD(&stMemPool[i].stMemMangeHead.stListEntry);
    }

    LOCK_MUTEX_INIT(&stMemMutexLock,"Mem's mutex");
    return RT_SUCCESS;
}