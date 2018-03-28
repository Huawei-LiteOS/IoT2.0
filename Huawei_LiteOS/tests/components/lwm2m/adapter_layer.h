#ifndef _ADAPTER_LAYER_H_
#define _ADAPTER_LAYER_H_
#ifdef __cplusplus
extern "C" {
#endif
#include <stdlib.h>
#include "los_typedef.h"
#include "agenttiny.h"
#include "dtls_conn.h"
#include "stm32f4xx.h"
#include "dtls_interface.h"

void atiny_log(const char* fmt, ...);
void *LOS_MemAlloc (VOID *pPool, UINT32  uwSize);
UINT32 LOS_MemFree(VOID *pPool, VOID *pMem);
UINTPTR LOS_IntLock(VOID);
VOID LOS_IntRestore(UINTPTR uvIntSave);
void Delayus(uint32_t usec);
void RNG_DeInit(void);
void RCC_AHB2PeriphClockCmd(uint32_t RCC_AHB2Periph, FunctionalState NewState);
void RNG_Cmd(FunctionalState NewState);
FlagStatus RNG_GetFlagStatus(uint8_t RNG_FLAG);
uint32_t RNG_GetRandomNumber(void);
UINT32 LOS_BinarySemCreate (UINT16 usCount, UINT32 *puwSemHandle);
UINT32 LOS_SemDelete(UINT32 uwSemHandle);
UINT32 LOS_SemPend(UINT32 uwSemHandle, UINT32 uwTimeout);
UINT32 LOS_SemPost(UINT32 uwSemHandle);
mbedtls_ssl_context *dtls_ssl_new_with_psk(char *psk, unsigned psk_len, char *psk_identity);
int dtls_shakehand(mbedtls_ssl_context *ssl, const char *host, const char *port);
void* atiny_net_connect( const char *host, const char *port, int proto );
void atiny_net_close( void *ctx );
int atiny_net_recv_timeout( void *ctx, unsigned char *buf, size_t len,
			    uint32_t timeout );
int dtls_read(mbedtls_ssl_context *ssl, unsigned char *buf, size_t len, uint32_t timeout);
int atiny_net_send( void *ctx, const unsigned char *buf, size_t len );
int dtls_write(mbedtls_ssl_context *ssl, const unsigned char *buf, size_t len);
UINT64 LOS_TickCountGet (VOID);
int atiny_cmd_ioctl(atiny_cmd_e cmd, char* arg, int len);

#ifdef __cplusplus
}
#endif
#endif
