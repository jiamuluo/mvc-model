/*
 * @Author: jiamu 
 * @Date: 2018-07-15 16:28:56 
 * @Last Modified by: jiamu
 * @Last Modified time: 2018-07-16 13:37:51
 */

#include "os_selinit.h"
#include "log.h"
#include <fcntl.h>

extern INT32 os_mem_init(void);
extern INT32 os_timerSchedualInit(UINT32 ulTimerRateMs);
extern void log_init(const char * strAppName);
extern void os_queue_init(void);

void os_checkExist(void)
{
    INT32 fd = 0, lRet = 0;
	struct flock fl;
	INT8  buf[128]	= {0};

	sprintf(buf, "/tmp/%s_pid", APP_NAME);	
	fd = open(buf, O_RDWR|O_CREAT, S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);
	if (fd == -1)
	{
		exit(-1);
	}

	fl.l_type  = F_WRLCK;
	fl.l_start	= 0;
	fl.l_whence = SEEK_SET;
	fl.l_len = 0;
	lRet = fcntl(fd, F_SETLK, &fl);
	if (lRet < 0)
	{
		SYS_TRACE("app %s is Already Running !", APP_NAME);
		close(fd);
		exit(-1);
	}

	ftruncate(fd, 0);
	memset(buf, 0, sizeof(buf));
	snprintf(buf, sizeof(buf), "%ld", (long)getpid());
	write(fd, buf, sizeof(buf));

	return;
}

void os_deamon_init(void)
{
     pid_t  pid;

    if ((pid = fork()) < 0) 
	{
		fprintf(stderr, "Fork error, program will exit!\n");
		exit(0);
	}
	else if (pid != 0)
	{
		fprintf(stderr, "father PID %d exit!\n", getpid());
		exit(0);
	}
	else 
	{
		(void)setsid();
		if (chdir("/") != 0) 
		{
			fprintf(stderr, "Chdir error, program will exit!\n");
			exit(0);
		}
		fprintf(stderr, "Deamon PID %d doing!\n", getpid());
	} 
}
void os_init(void)
{
    //check this app is if running now 
    os_checkExist();

    //let app becomes a deamon 
    os_deamon_init();

    //init log system
    log_init(APP_NAME);

	//init queue manage system
	os_queue_init();

    //init memory mannage system
    os_mem_init();
    
    //init timer driver system
    os_timerSchedualInit(OS_TIMER_RATE);
}
/**
 * @brief you can do any thing here but exit
 * 
 */
void os_hook(void)
{
	while(1)
	{
		sleep(10);
	}
}