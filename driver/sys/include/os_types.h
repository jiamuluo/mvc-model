/*
 * @Author: jiamu 
 * @Date: 2018-07-13 15:32:33 
 * @Last Modified by: jiamu
 * @Last Modified time: 2018-07-16 11:55:19
 */


#ifndef _OS_TYPES_H_
#define _OS_TYPES_H_

#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <signal.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "os_config.h"

typedef void				VOID;
typedef void*				HANDLE;

typedef char				INT8;		//c
typedef short				INT16;	   //s
typedef int				    INT32;	   //l
typedef float				FLOAT32;	   //f

typedef unsigned char		UINT8;	  //uc
typedef unsigned short		UINT16;	   //us
typedef unsigned int		UINT32;	  //ul
typedef unsigned int		IPADDR;
typedef char *              STRING;

#define MAX_STR_LEN_8		8
#define MAX_STR_LEN_16		16
#define MAX_STR_LEN_32		32
#define MAX_STR_LEN_48		48
#define MAX_STR_LEN_64		64
#define MAX_STR_LEN_128		128
#define MAX_STR_LEN_160		160
#define MAX_STR_LEN_256		256
#define MAX_STR_LEN_512		512
#define MAX_STR_LEN_640		640
#define MAX_STR_LEN_768		768
#define MAX_STR_LEN_1024	1024
#define MAX_STR_LEN_1536	1536
#define MAX_STR_LEN_2048	2048
#define MAX_STR_LEN_4096	4096
#define MAX_STR_LEN_10240	10240
#define MAX_STR_LEN_51200	51200
#define MAX_STR_LEN_102400	102400

#define MAX_NANO_SEC 999999999

typedef enum 
{
    V_DISABLE = 0,
    V_ENABLE
}ENUM_ENABLE;
typedef enum
{
    RT_FAILED = -20,
    RT_MALLOC_FAILD,
    RT_MEM_OVER,
    RT_ARG_INVALID,
    RT_SUCCESS = 0,
}RT_STATUS_ENUM;
#endif
