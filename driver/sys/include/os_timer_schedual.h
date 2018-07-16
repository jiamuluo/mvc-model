/*
 * @Author: jiamu 
 * @Date: 2018-06-29 14:18:45 
 * @Last Modified by: jiamu
 * @Last Modified time: 2018-07-16 11:48:43
 */


#ifndef _OS_SCHEDUAL_H_
#define _OS_SCHEDUAL_H_

#include "os_types.h"
#include "os_mem_manage.h"
#include "os_config.h"
#include "list.h"
#include "os_task_schedual.h"

typedef enum
{
     OS_OPT_TMR_ONE_SHOT   = 0, /* Timer will not auto restart when it expires        */
     OS_OPT_TMR_PERIODIC        /* Timer will     auto restart when it expires        */
}OS_TIMER_TYPE;

typedef enum
{
    OS_TMR_STATE_UNUSED = 0,
    OS_TMR_STATE_STOPPED,
    OS_TMR_STATE_RUNNING,
    OS_TMR_STATE_COMPLETED
}OS_TIMER_STATE;

typedef struct
{
    UINT32              ulDelay;        /*Timer to delay*/
    UINT32              ulExpire;       /*Timer absolutely expire time*/
    UINT32              ulRemain;       /*Timer time to live*/
           
    OS_TIMER_TYPE       eTimerType;     /*Timer type is one-time or not*/
    OS_TIMER_STATE      eTimerState;    /*Timer current state*/
    OS_TASK_ID          eHandleTaskId;  /*which task will handle it*/

    OS_TIMER_MSG_TYPE   eTimerMsgType;  /*Timer message type */
    UINT32              ulMsgSize;      /*Msg size*/
    void*               pData;          /*Timer's message*/
    UINT8               ucTimerBirthInfo[MAX_STR_LEN_64];/*Timer's birth info ,file,func,line*/
    struct list_head           stListEntry;    /*Timer's list*/
}OS_TIMER_SCHEDUAL_INFO;


typedef struct
{
    UINT16 usMaxEntry;
    UINT16 usCurrentEntry;
    OS_TIMER_SCHEDUAL_INFO stTimerSchedual;
}OS_TIMER_POOL;

INT32 os_timerAddNewOne(UINT32 ulTimeMs,OS_TIMER_TYPE eTimerType,OS_TIMER_MSG_TYPE eTimerMsgType,OS_TASK_ID eTaskId,UINT32 ulMsgSize,void *pData);

#endif

