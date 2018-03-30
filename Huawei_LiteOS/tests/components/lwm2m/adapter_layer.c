
#include "adapter_layer.h"
#include "unistd.h"
#include <semaphore.h>

#define OS_SYS_MEM_SIZE                                     0x0024000          // size 200k

UINT32  g_vuwIntCount = 0;
UINT64      g_ullTickCount;
UINT8 m_aucSysMem0[OS_SYS_MEM_SIZE];
void *g_pstAllSem;
#ifdef __cplusplus
extern "C" {
#endif

void atiny_log(const char* fmt, ...)
{
  printf("%s",fmt);
  return;
}


void *LOS_MemAlloc (VOID *pPool, UINT32  uwSize)
{
  return malloc(uwSize);
}
UINT32 LOS_MemFree(VOID *pPool, VOID *pMem)
{
  free(pMem);
  return 0;
}

UINTPTR LOS_IntLock(VOID)
{
    return 0;
}
VOID LOS_IntRestore(UINTPTR uvIntSave)
{
    return ;
}
void Delayus(uint32_t usec)
{
    usleep(usec);
}
void RNG_DeInit(void)
{
    return;
}
void RCC_AHB2PeriphClockCmd(uint32_t RCC_AHB2Periph, FunctionalState NewState)
{
    return;
}
void RNG_Cmd(FunctionalState NewState)
{
    return ;
}
FlagStatus RNG_GetFlagStatus(uint8_t RNG_FLAG)
{
    return RESET;
}
uint32_t RNG_GetRandomNumber(void)
{
    return 0;
}
UINT32 LOS_BinarySemCreate (UINT16 usCount, UINT32 *puwSemHandle)
{
  sem_t  *sem = (sem_t *)puwSemHandle;
  printf("in %s\n", __func__);
  UINT32 ret = -1;//sem_init(&sem, 0, usCount);
  printf("ret = %d in %s, sem.len = %d\n", ret, __func__, sizeof(sem));
  return ret;
  //   return 0;
}
UINT32 LOS_SemDelete(UINT32 uwSemHandle)
{
  //sem_destroy(uwSemHandle);
    return -1;
}
UINT32 LOS_SemPend(UINT32 uwSemHandle, UINT32 uwTimeout)
{
  struct timespec ts;

  ts.tv_sec = uwTimeout;
  ts.tv_nsec = 0;
  //sem_timedwait(uwSemHandle, &ts);
    return -1;
}
UINT32 LOS_SemPost(UINT32 uwSemHandle)
{
    return 0;
}

mbedtls_ssl_context *dtls_ssl_new_with_psk(char *psk, unsigned psk_len, char *psk_identity)
{
  return NULL;
}
int dtls_shakehand(mbedtls_ssl_context *ssl, const char *host, const char *port)
{
  return 0;
}
void* atiny_net_connect( const char *host, const char *port, int proto )
{
  return NULL;
}
void atiny_net_close( void *ctx )
{
  return ;
}
void dtls_ssl_destroy(mbedtls_ssl_context *ssl)
{
  return;
}
int atiny_net_recv_timeout( void *ctx, unsigned char *buf, size_t len,
                      uint32_t timeout )
{
  return 0;
}
int dtls_read(mbedtls_ssl_context *ssl, unsigned char *buf, size_t len, uint32_t timeout)
{
  return 0;
}
int atiny_net_send( void *ctx, const unsigned char *buf, size_t len )
{
  return 0;
}

int dtls_write(mbedtls_ssl_context *ssl, const unsigned char *buf, size_t len)
{
  return 0;
}

UINT64 LOS_TickCountGet (VOID)
{
  return 0;
}
/*
int atiny_cmd_ioctl(atiny_cmd_e cmd, char* arg, int len)
{
  return 0;
}
*/
#ifdef __cplusplus
}
#endif
