/*
 * @Author: jiamu 
 * @Date: 2018-06-29 14:39:25 
 * @Last Modified by: jiamu
 * @Last Modified time: 2018-07-16 11:06:33
 */


#ifndef _OS_TIMER_CONFIG_H_
#define _OS_TIMER_CONFIG_H_

#define APP_NAME "jiamu"

/***********************************************************************
 * 
 *                  system debug config
 * 
***********************************************************************/
#define DEBUG_MEM
#define DEBUG_LOCK
#define DEBUG_SEM
#define DEBUG_SPIN
/************************************************************************
 *  timer manager 
 *
 * 
 * 
**************************************************************************/
#define OS_TIMER_RATE   10           /***********10ms********/
#define OS_TIMER_POOL_SIZE 11    
#define OS_TIMER_LIST_SIZE 20    

/************************************************************************
 * mem manager
 * 
 * 
 * 
*****************************************************************************/
#define OS_MEM_POOL_SIZE 50    
#define OS_MEM_LIST_SIZE 50   


/************************************************************************
 * queue manager
 * 
 * 
 * 
*****************************************************************************/
#define OS_QUEUE_MAX   256
#define OS_QIEIE_QUOTE 10 //peak num of one handle time,avoid to take too much time
/*********************************user define********************************/
typedef enum
{

    TIMER_OUTPUT = 0,
    //add you timer msg before this
    TIMER_SYS_MAX ,
}OS_TIMER_MSG_TYPE;


//need user define
typedef enum
{
    _MSG_OUT,
}OS_QUE_MSG_TYPE;

#endif
