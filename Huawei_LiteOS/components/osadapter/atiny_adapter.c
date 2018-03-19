#include <string.h>
#include <stdio.h>
#include "atiny_adapter.h"
#include "stm32f4xx.h"
#include "stm32f4xx_conf.h"
#include "dwt.h"
#include "los_base.h"
#include "los_sys.h"
#include "los_memory.h"
#include "cmsis_os.h"

#define ATINY_CNT_MS_PER_SECOND 1000
#define ATINY_CNT_US_PER_MS 1000
#define ATINY_CNT_US_PER_SECOND ATINY_CNT_MS_PER_SECOND * ATINY_CNT_US_PER_MS
#define ATINY_CNT_MAX_RETRY 1000


/************************time function**********************/

unsigned long atiny_gettime_ms(void)
{
	unsigned long long ticks = DWT_CYCCNT;
	unsigned long uscount = ticks / (SystemCoreClock / ATINY_CNT_US_PER_SECOND);
	return uscount / ATINY_CNT_US_PER_MS;
}

void atiny_usleep(unsigned long usec)
{
    Delayus((uint32_t)usec);
}


/************************random function**********************/

int atiny_hardware_poll(unsigned char *output, size_t len, size_t *olen)
{
    size_t i;
    uint32_t random_number;
    uint32_t reId = 0;
    RNG_DeInit();
    RCC_AHB2PeriphClockCmd(RCC_AHB2Periph_RNG, ENABLE); // Å²µØ·½
    RNG_Cmd(ENABLE);
    *olen = 0;

    for (i = 0; i < len; i += sizeof(uint32_t))
    {
        reId = 0;
        while ((RNG_GetFlagStatus(RNG_FLAG_DRDY) == RESET) && reId++ < ATINY_CNT_MAX_RETRY)
            atiny_usleep(1000);
        random_number = RNG_GetRandomNumber();
        memcpy(output + i, &random_number,
            sizeof(uint32_t) > len - i ? len - i : sizeof(uint32_t));
    }
    *olen = len;

    return 0;
}


/************************memory function**********************/

void* atiny_malloc(size_t size)
{
    return LOS_MemAlloc(m_aucSysMem0, size);
}

void* atiny_calloc(size_t n, size_t size)
{
    void *ptr = LOS_MemAlloc(m_aucSysMem0, n*size);
    if (NULL != ptr)
        memset(ptr, 0, n*size);
    return ptr;
}

void atiny_free(void *ptr)
{
    LOS_MemFree(m_aucSysMem0, ptr);
}


/************************string function**********************/

int atiny_snprintf(char *buf, unsigned int size, const char *format, ...)
{
    int     ret;
    va_list args;

    va_start(args, format);
    ret = vsnprintf(buf, size, format, args);
    va_end(args);

    return ret;
}

int atiny_printf(const char *format, ...)
{
    int ret;
    va_list args;

    va_start(args, format);
    ret = vprintf(format, args);
    va_end(args);

    fflush(stdout);
    return ret;
}

int atiny_fprintf(FILE *stream, const char *format, ...)
{
    int ret = 0;   
    va_list args;
    va_start(args,format);
    ret = vfprintf(stream, format, args);  
    va_end(args);
    return ret;
}


/************************lock function**********************/

void *atiny_mutex_create(void)
{
    return osSemaphoreNew(1, 1, NULL);
}

void atiny_mutex_destroy(void *mutex)
{
    osSemaphoreDelete(mutex);
}

void atiny_mutex_lock(void *mutex)
{
    osSemaphoreAcquire(mutex, 0xFFFFFFFF);
}

void atiny_mutex_unlock(void *mutex)
{
    osSemaphoreRelease(mutex);
}

