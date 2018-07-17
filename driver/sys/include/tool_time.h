/*
 * @Author: jiamu 
 * @Date: 2018-07-16 18:00:31 
 * @Last Modified by: jiamu
 * @Last Modified time: 2018-07-16 18:01:31
 */
#ifndef _TOOL_TIME_H_
#define _TOOL_TIME_H_

#include "os_types.h"


void time_GetSysUptime(long *pulTime);
void time_GetCurrentTimeus(char *buf, unsigned int buflen);
#endif
