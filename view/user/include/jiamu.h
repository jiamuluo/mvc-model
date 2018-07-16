/*
 * @Author: jiamu 
 * @Date: 2018-07-15 17:00:49 
 * @Last Modified by: jiamu
 * @Last Modified time: 2018-07-16 13:08:36
 */

#ifndef _JIAMU_TEST_H_
#define _JIAMU_TEST_H_

#include "os_types.h"

void jiamu_init(void *arg);
void jiamu_handle_msg(void *data,void *arg);
void jiamu_handle_timer(void *data,void *arg);
#endif
