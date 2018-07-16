/*
 * @Author: jiamu 
 * @Date: 2018-07-13 15:42:28 
 * @Last Modified by: jiamu
 * @Last Modified time: 2018-07-16 11:06:08
 */



#ifndef _OS_MEM_MANAGE_H_
#define _OS_MEM_MANAGE_H_
#include "os_types.h"
#include "list.h"

#define MEM_MAGIC_NUM 0xaa551122

typedef struct
{
    bool isBusy;
    UINT32 ulSize;
    UINT32 ulHash;
    UINT32 ulMagic;
    struct list_head stListEntry;
    #ifdef MDEBUG_MEM
    UINT32 ulLine;
    INT8 ascFuncName[64];
    INT8 ascTime[64];
    #endif
    UINT8 mem[0];
}OS_MEM_CTRL;

typedef struct
{
    UINT16 usMaxEntry;
    UINT16 usCurrentEntry;
    OS_MEM_CTRL stMemMangeHead;
}OS_MEM_POOL;


void *os_mem_malloc(UINT32 ulSize,const char* strFile,const char* strFun,const UINT32 ulLine);
void os_mem_free(void *ptr,const char* strFile,const char* strFun,const UINT32 ulLine);

#define OS_MEM_MALLOC(size)     os_mem_malloc(size,__FILE__,__FUNCTION__,__LINE__)
#define OS_MEM_FREE(ptr)      os_mem_free(ptr,__FILE__,__FUNCTION__,__LINE__)

#endif

