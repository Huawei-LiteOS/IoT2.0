#ifndef _ATINY_ADAPTER_H_
#define _ATINY_ADAPTER_H_

#if defined(__cplusplus)
extern "C" {
#endif

#include <stdint.h>
#include <stdio.h>

/************************time function**********************/

// 获取自系统启动后经过的毫秒数
uint64_t atiny_gettime_ms(void);

// 当前任务休眠usec微秒
void atiny_usleep(unsigned long usec);


/************************random function**********************/

int atiny_random(unsigned char *output, size_t len);


/************************memory function**********************/

void* atiny_malloc(size_t size);
void atiny_free(void *ptr);


/************************string function**********************/

int atiny_snprintf(char *buf, unsigned int size, const char *format, ...);
int atiny_printf(const char *format, ...);


/************************lock function**********************/

void *atiny_mutex_create(void);
void atiny_mutex_destroy(void *mutex);
void atiny_mutex_lock(void *mutex);
void atiny_mutex_unlock(void *mutex);

#if defined(__cplusplus)
}
#endif

#endif

