/*
 * @Author: jiamu 
 * @Date: 2018-07-13 17:05:39 
 * @Last Modified by: jiamu
 * @Last Modified time: 2018-07-16 13:21:49
 */
#include <syslog.h>
#include "log.h"
#include <stdarg.h>

#define MAX_TRACE_LEN 256

static pthread_mutex_t stMemMutexLock;
static LOG_LEVEL eTaskLogLevel[_TASK_MAX];

void log_init(const char * strAppName)
{
    int i = 0;
    for(i = 0;i < _TASK_MAX;i++)
    {
        eTaskLogLevel[i] = LOG_INFO;
    }
    openlog(strAppName, LOG_PID|LOG_NDELAY|LOG_CONS, LOG_USER);

    eTaskLogLevel[_TASK_SYS] = OS_DETAIL;
    pthread_mutex_init(&stMemMutexLock,NULL);
}

int log_setTaskLogLevel(LOG_LEVEL eLogLevel,OS_TASK_ID eTask)
{
    if(eTask >= _TASK_MAX || eLogLevel > OS_DETAIL)
        return RT_ARG_INVALID;

    eTaskLogLevel[eTask] = eLogLevel;
    return RT_SUCCESS;
}

void log_trace(OS_TASK_ID eTask,LOG_LEVEL eLogLevel, const char * pstrFormat, ...)
{
	va_list listVars;
  	char strInfo[MAX_TRACE_LEN] = {0};
    /*do not need record*/
    if(eTask >= _TASK_MAX || eLogLevel > eTaskLogLevel[eTask] || eLogLevel > OS_DETAIL)
        return;
    
    pthread_mutex_lock(&stMemMutexLock);

    if(strlen(pstrFormat) < MAX_TRACE_LEN)
	{
		va_start (listVars, pstrFormat);
        vsprintf(strInfo, pstrFormat, listVars);
		va_end (listVars);

		printf("strInfo = %s\n",strInfo);
		syslog(eLogLevel,"%s", strInfo);
	}
	else
	{
		printf("str len large than %d!\n", MAX_TRACE_LEN);
	}
    sync();
    pthread_mutex_unlock(&stMemMutexLock);
}
