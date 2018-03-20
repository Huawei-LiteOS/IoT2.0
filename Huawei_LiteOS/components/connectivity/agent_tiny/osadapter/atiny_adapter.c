#include <string.h>
#include <stdio.h>
#include "atiny_adapter.h"
#include "stm32f4xx.h"
#include "stm32f4xx_conf.h"
#include "los_memory.h"
#include "los_sys.ph"
#include "los_sem.ph"
#include "los_tick.ph"
#include "dwt.h"

#define ATINY_CNT_MAX_RETRY 1000
#define ATINY_CNT_MAX_WAITTIME 0xFFFFFFFF


/************************time function**********************/

static uint64_t osKernelGetTickCount (void)
{
    uint64_t ticks;
    UINTPTR uvIntSave;

    if(OS_INT_ACTIVE)
    {
        ticks = 0U;
    }
    else
    {
        uvIntSave = LOS_IntLock();
        ticks = g_ullTickCount;
        LOS_IntRestore(uvIntSave);
    }

    return ticks;
}

uint64_t atiny_gettime_ms(void)
{
	return osKernelGetTickCount() * (OS_SYS_MS_PER_SECOND / LOSCFG_BASE_CORE_TICK_PER_SECOND);
}

void atiny_usleep(unsigned long usec)
{
    Delayus((uint32_t)usec);
}


/************************random function**********************/

int atiny_random(unsigned char *output, size_t len)
{
    size_t i;
    uint32_t random_number;
    uint32_t reId = 0;
    RNG_DeInit();
    RCC_AHB2PeriphClockCmd(RCC_AHB2Periph_RNG, ENABLE);
    RNG_Cmd(ENABLE);

    for (i = 0; i < len; i += sizeof(uint32_t))
    {
        reId = 0;
        while ((RNG_GetFlagStatus(RNG_FLAG_DRDY) == RESET) && reId++ < ATINY_CNT_MAX_RETRY);
        if (reId == ATINY_CNT_MAX_RETRY)
            return -1;
        random_number = RNG_GetRandomNumber();
        memcpy(output + i, &random_number,
            sizeof(uint32_t) > len - i ? len - i : sizeof(uint32_t));
    }
    return 0;
}


/************************memory function**********************/

void* atiny_malloc(size_t size)
{
    return LOS_MemAlloc(m_aucSysMem0, size);
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


/************************lock function**********************/

#if (LOSCFG_BASE_IPC_SEM == YES)

void *atiny_mutex_create(void)
{
    uint32_t uwRet;
    uint32_t uwSemId;

    if (OS_INT_ACTIVE)
    {
        return NULL;
    }

    uwRet = LOS_BinarySemCreate(1, &uwSemId);

    if (uwRet == LOS_OK)
    {
        return (void*)(GET_SEM(uwSemId));
    }
    else
    {
        return NULL;
    }
}

void atiny_mutex_destroy(void *mutex)
{
    if (OS_INT_ACTIVE)
    {
        return;
    }

    if (mutex == NULL)
    {
        return;
    }

    LOS_SemDelete(((SEM_CB_S *)mutex)->usSemID);
}

void atiny_mutex_lock(void *mutex)
{
    if (mutex == NULL)
    {
        return;
    }

    if (OS_INT_ACTIVE)
    {
        return;
    }

    LOS_SemPend(((SEM_CB_S *)mutex)->usSemID, ATINY_CNT_MAX_WAITTIME);
}

void atiny_mutex_unlock(void *mutex)
{
    if (mutex == NULL)
    {
        return;
    }

    LOS_SemPost(((SEM_CB_S *)mutex)->usSemID);
}

#else
void *atiny_mutex_create(void) { return NULL; }
void atiny_mutex_destroy(void *mutex) { ((void)mutex); }
void atiny_mutex_lock(void *mutex) { ((void)mutex); }
void atiny_mutex_unlock(void *mutex) { ((void)mutex); }

#endif
