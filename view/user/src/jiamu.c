/*
 * @Author: jiamu 
 * @Date: 2018-07-16 13:08:13 
 * @Last Modified by: jiamu
 * @Last Modified time: 2018-07-16 13:09:07
 */

#include "jiamu.h"
#include "log.h"
#include "os_mem_manage.h"
#include "os_timer_schedual.h"

void jiamu_init(void *arg)
{
    char *pucMsg = OS_MEM_MALLOC(64);
    memset(pucMsg,0,64);
    strcpy(pucMsg,"This is jiamu timer");

    printf("Init jiamu task \n");
    os_timerAddNewOne(5000,
                    OS_OPT_TMR_PERIODIC,
                    TIMER_OUTPUT,
                    _TASK_JIAMU,
                    64,
                    pucMsg
                    );
}

void jiamu_handle_msg(void *data,void *arg)
{

}

void jiamu_handle_timer(void *data,void *arg)
{
    if(NULL == data && NULL == arg)
    {
        SYS_TRACE("Handle timer message ,but data is null");
        return;
    }
    MSG_TLV_INFO *pstGetMsg = (MSG_TLV_INFO *)data;

    switch(pstGetMsg->eMsgType)
    {
        case TIMER_OUTPUT:
            printf("Get message to output:size %d ,%s \n",pstGetMsg->ulMsgSize,(char *)pstGetMsg->p_void);
        break;
        default:
            printf("Get incorrect timer msg type %d\n",pstGetMsg->eMsgType);
        break;
    }
}