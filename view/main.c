/*
 * @Author: jiamu 
 * @Date: 2018-07-13 15:40:06 
 * @Last Modified by: jiamu
 * @Last Modified time: 2018-07-16 13:23:03
 */


#include "os_selinit.h"
#include "os_task_schedual.h"
#include "log.h"
#include "jiamu.h"

OS_TASK_CTRL gstAppTaskInfo[] = 
{
    {"Jiamu",_TASK_JIAMU,1,16 * 1024,128,OS_WARN,jiamu_init,jiamu_handle_msg,jiamu_handle_timer,NULL}
};

int main(int argc,char **argv)
{
    int i = 0;
    os_init();

    for(i = 0;i < sizeof(gstAppTaskInfo) / sizeof(gstAppTaskInfo[0]);i++)
    {
        os_taskCreateInit(&gstAppTaskInfo[i]);
    }
    SYS_TRACE("Now app include %d tasks is running ",i);

    os_hook();

    //Never be here
    return RT_FAILED;
}