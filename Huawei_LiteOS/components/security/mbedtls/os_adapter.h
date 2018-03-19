#ifndef _OS_ADAPTER_H_
#define _OS_ADAPTER_H_

#include <stdio.h>

/************************time function**********************/

struct atiny_time_st {
    unsigned char opaque[32];
};

// 返回自系统启动后经过的时间，单位为秒
unsigned long atiny_gettime(void);

// 返回自系统启动后经过的时间，单位为毫秒；reset为1，则重新计时
unsigned long atiny_timing_get_timer(struct atiny_time_st *val, int reset);

// 返回CPU的cycle数
unsigned long atiny_timing_hardclock(void);


/************************random function**********************/

int atiny_hardware_poll(void *data, unsigned char *output, size_t len, size_t *olen);


/************************memory function**********************/

void* atiny_malloc(size_t size);
void* atiny_calloc(size_t n, size_t size);
void atiny_free(void *ptr);

#ifdef WITH_POSIX
#include <string.h>
#define ATINY_TICKS_PER_SECOND 1000
typedef unsigned int atiny_time_t;
typedef time_t atiny_time_t;
typedef unsigned int atiny_tick_t;

void atiny_rcv_frame(u8 *buffer, unsigned int len);

static int atiny_prng(unsigned char *buf, size_t len);
void atiny_clock_start(void);
atiny_time_t atiny_ticks_to_rt(atiny_tick_t t);
void atiny_ticks(atiny_tick_t *t);
#endif

extern void* (*atiny_memcpy)(void *dst, const void *src, unsigned int len);
extern void* (*atiny_memset)(void *dst, int c, unsigned int len);
extern int (*atiny_memcmp)(const void *buf1, const void *buf2, unsigned int len);
extern void* (*atiny_memmove)( void* dest, const void* src, size_t count);
extern void* (*atiny_memchr)(const void *buf, int ch, size_t count);



/************************string function**********************/

extern int (*atiny_snprintf)(char *buf, unsigned int size, const char *format, ...);
extern int (*atiny_printf)(const char *format, ...);
extern int (*atiny_fprintf)(FILE *stream, const char *format, ...);
extern int (*atiny_strncmp)(const char *str1, const char *str2, unsigned int len);


#endif

