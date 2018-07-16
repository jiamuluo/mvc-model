/*
 * @Author: jiamu 
 * @Date: 2018-07-16 13:34:57 
 * @Last Modified by: jiamu
 * @Last Modified time: 2018-07-16 13:35:39
 * @bref this task intern queue
 */
#ifndef _OS_QUEUE_MANAGE_H_
#define _OS_QUEUE_MANAGE_H_

#include "os_task_schedual.h"
#include "os_types.h"
#include "log.h"
#include "lock.h"
#include "list.h"



typedef struct
{
    struct list_head stListEntry;
    bool         isTimer;
    OS_TASK_ID   eTaskSrc;
    OS_TASK_ID   eTaskDst;
    MSG_TLV_INFO stMsgInfo;
}OS_MSG_Q;

typedef struct
{
    INT8             ascQueueName[64];
    UINT32           ulNbrEntriesSize;                    /* Maximum allowable number of entries in the queue       */
    UINT32           ulNbrEntries;                        /* Current number of entries in the queue                 */
    UINT32           ulNbrEntriesMax;                     /* Peak number of entries in the queue                    */
    pthread_mutex_t  *pstQueueMutexLock;                  /* Task mutex reference*/
    sem_t            *pstSemInfo;                         /* Task sem reference*/
    OS_MSG_Q stMsgQ;
}OS_QUEUE;


#define Q_OS_PEND(sem)  LOCK_SEM_WAIT(sem)


int os_create_taskque(OS_TASK_CTRL* pstTaskInfo);
int os_send_msg(OS_TASK_ID eDstTask ,bool isTimer,OS_QUE_MSG_TYPE eMsgType,UINT32 ulMsgSize,void *p_void);
int os_queue_handle_msg(OS_TASK_CTRL* pstTaskInfo);
void os_queue_init(void);
#endif
