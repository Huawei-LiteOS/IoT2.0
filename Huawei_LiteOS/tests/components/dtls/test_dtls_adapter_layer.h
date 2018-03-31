#ifndef _TEST_DTLS_ADAPTER_LAYER_H_
#define _TEST_DTLS_ADAPTER_LAYER_H_
#ifdef __cplusplus
extern "C" {
#endif

#include<stdlib.h>
#include"atiny_adapter.h"
#include"atiny_socket.h"
#include"net_sockets.h"




void* 	 atiny_malloc(size_t size);
int 	 atiny_printf(const char *format, ...);
int	     atiny_snprintf(char *buf, unsigned int size, const char *format, ...);
void     atiny_free(void *ptr);
void     atiny_usleep(unsigned long usec);
uint64_t atiny_gettime_ms(void);
int      atiny_random(unsigned char *output, size_t len);


void*    atiny_net_connect( const char *host, const char *port, int proto );
int      atiny_net_recv( void *ctx, unsigned char *buf, size_t len );
int      atiny_net_recv_timeout( void *ctx, unsigned char *buf, size_t len, uint32_t timeout );
int      atiny_net_send( void *ctx, const unsigned char *buf, size_t len );
void     atiny_net_close( void *ctx );



#ifdef __cplusplus
}
#endif
#endif


