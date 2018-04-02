
#include "adapter_layer.h"
#include "unistd.h"
#include <semaphore.h>

#define OS_SYS_MEM_SIZE                                     0x0024000          // size 200k
//for lwip defined
#define F_GETFL 3

UINT32  g_semCrt = -1;
UINT32  g_vuwIntCount = 0;
UINT64      g_ullTickCount = 0;
UINT8 m_aucSysMem0[OS_SYS_MEM_SIZE];
void *g_pstAllSem;
#ifdef __cplusplus
extern "C" {
#endif

void atiny_log(const char* fmt, ...)
{
  //printf("%s",fmt);
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
    return g_semCrt;
}
UINT32 LOS_SemDelete(UINT32 uwSemHandle)
{
    return -1;
}
UINT32 LOS_SemPend(UINT32 uwSemHandle, UINT32 uwTimeout)
{
    return -1;
}
UINT32 LOS_SemPost(UINT32 uwSemHandle)
{
    return -1;
}

mbedtls_ssl_context *dtls_ssl_new_with_psk(char *psk, unsigned psk_len, char *psk_identity)
{
  return NULL;
}
int dtls_shakehand(mbedtls_ssl_context *ssl, const char *host, const char *port)
{
  return 0;
}
void dtls_ssl_destroy(mbedtls_ssl_context *ssl)
{
  return;
}
int dtls_read(mbedtls_ssl_context *ssl, unsigned char *buf, size_t len, uint32_t timeout)
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
int lwip_fcntl(int s, int cmd, int val)
{
  /*
   * Never return 'WOULD BLOCK' on a non-blocking socket
   */
  int ret = -1;
  switch (cmd)
    {
    case F_GETFL:
      {
	ret = 0;
      }
      break;
    default:
      break;
    }
  return ret;
}

int  lwip_shutdown(int s, int how)
{
  return -1;
}
  //  int lwip_close(int s){
  //return -1;
  //}

#ifdef __cplusplus
}
#endif
