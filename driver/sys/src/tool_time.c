/*
 * @Author: jiamu 
 * @Date: 2018-07-16 18:00:35 
 * @Last Modified by: jiamu
 * @Last Modified time: 2018-07-16 18:01:55
 */
#include "tool_time.h"
#include <sys/sysinfo.h>
#include <sys/time.h>
#include <time.h>

void time_GetSysUptime(long *pulTime)
{
	 struct sysinfo stSysInfo;
     memset(&stSysInfo,0,sizeof(stSysInfo));
	 sysinfo(&stSysInfo);
	 *pulTime = (long)stSysInfo.uptime;
	// printf("stSysInfo.uptime = %d \n",stSysInfo.uptime);
}
void time_GetCurrentTimeus(char *buf, unsigned int buflen)
{
	struct tm *tp;
	struct timeval tv;

	gettimeofday(&tv, NULL);
	tp = localtime(&tv.tv_sec);
 
	snprintf(buf + 0, buflen, "%2dH.", tp->tm_hour);
	snprintf(buf + 3, buflen, "%2dM.", tp->tm_min);
	snprintf(buf + 6, buflen, "%2dS.", tp->tm_sec);
	snprintf(buf + 9, buflen, "%6dus", (UINT32)tv.tv_usec);
}

static UINT32 ulSmartDog = 0;
static INT8 ascAppName[MAX_STR_LEN_128] = {0};
static UINT32 ulFeedDogTime = 0;

static INT32 os_HandleDog(void)
{
	if(0 != ulSmartDog)
	{
		ulSmartDog = 0;
		alarm(ulFeedDogTime);
		SYS_TRACE(MID_APP_DEVMNG1,"Alarm time is coming \n");
	}
	else
	{
		SYS_TRACE(MID_APP_DEVMNG1,"%s application has been blocking,Now Reset it \n",ascAppName);
		exit(-1);
	}
	
}

void time_RegisterWatchDog(UINT32 ulInterval,STRING strAppName)
{
	 ulFeedDogTime = (ulInterval == 0 || ulInterval < 60) ? DEFAULT_FEED_TIME : ulInterval;
	 
	 memcpy(ascAppName, strAppName, strlen(strAppName));
	 signal(SIGALRM, os_HandleDog);
	 alarm(ulFeedDogTime);
	 SYS_CRIT("%s application has registered one watchdog,Time is %d \n",ascAppName,ulFeedDogTime);
}

void time_FeedDog(void)
{
	ulSmartDog += 1;
	SYS_CRIT("%s application come to feed dog\n",ascAppName);
}