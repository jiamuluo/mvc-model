/*
 * @Author: jiamu 
 * @Date: 2018-07-13 16:36:20 
 * @Last Modified by: jiamu
 * @Last Modified time: 2018-07-16 13:22:05
 */

#ifndef _LOG_H_
#define _LOG_H_

#include "os_types.h"
#include "os_task_schedual.h"

typedef enum
{
    OS_EMERG,
    OS_ALERT,
    OS_CRIT,
    OS_ERR,
    OS_WARN,
    OS_INFO,
    OS_DETAIL,
}LOG_LEVEL;


void log_init(const char * strAppName);
int log_setTaskLogLevel(LOG_LEVEL eLogLevel,OS_TASK_ID eTask);
void log_trace(OS_TASK_ID eTask,LOG_LEVEL eLogLevel, const char * pstrFormat, ...);

#define LOG_TRACE(eMid, eLeavel, format,...)            log_trace(eMid, eLeavel, "[%04d %-10s]" format, __LINE__, __func__, ## __VA_ARGS__)

#define SYS_CRIT(format,...)                            log_trace(_TASK_SYS,OS_CRIT, "[%04d %-10s]" format, __LINE__, __func__, ## __VA_ARGS__)
#define SYS_TRACE(format,...)                           log_trace(_TASK_SYS,OS_ALERT, "[%04d %-10s]" format, __LINE__, __func__, ## __VA_ARGS__)

#endif