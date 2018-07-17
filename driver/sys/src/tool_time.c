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