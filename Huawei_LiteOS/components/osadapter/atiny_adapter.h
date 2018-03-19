#ifndef _OS_ADAPTER_H_
#define _OS_ADAPTER_H_

#include <stdio.h>

/************************time function**********************/

// 获取自系统启动后经过的毫秒数
unsigned long atiny_gettime_ms(void);

// 当前任务休眠usec微秒
void atiny_usleep(unsigned long usec);


/************************random function**********************/

int atiny_hardware_poll(unsigned char *output, size_t len, size_t *olen);


/************************memory function**********************/

void* atiny_malloc(size_t size);
void* atiny_calloc(size_t n, size_t size);
void atiny_free(void *ptr);


/************************string function**********************/

int atiny_snprintf(char *buf, unsigned int size, const char *format, ...);
int atiny_printf(const char *format, ...);
int atiny_fprintf(FILE *stream, const char *format, ...);


/************************lock function**********************/

void *atiny_mutex_create(void);
void atiny_mutex_destroy(void *mutex);
void atiny_mutex_lock(void *mutex);
void atiny_mutex_unlock(void *mutex);

#endif

