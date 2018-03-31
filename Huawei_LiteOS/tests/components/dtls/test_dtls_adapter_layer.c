#include "test_dtls_adapter_layer.h"
#include "unistd.h"

#ifdef __cplusplus
extern "C" {
#endif


void* 	 atiny_malloc(size_t size)
{
	return malloc(size);
}


int	atiny_printf(const char *format, ...)
{
	printf(format);
	return 0;
}


int	atiny_snprintf(char *buf, unsigned int size, const char *format, ...)
{
	return 0;
}


void	atiny_free(void *ptr)
{
	free(ptr);
	return;
}


void	atiny_usleep(unsigned long usec)
{
	return;
}


uint64_t atiny_gettime_ms(void)
{
	return 0;
}


int	atiny_random(unsigned char *output, size_t len)
{
	return 0;
}



/*------net related-----------*/


void	atiny_net_close( void *ctx )
{
	return;
}

int	atiny_net_recv( void *ctx, unsigned char *buf, size_t len )
{
	return 0;
}


int	atiny_net_recv_timeout( void *ctx, unsigned char *buf, size_t len, uint32_t timeout )
{
	return 0;
}


int	atiny_net_send( void *ctx, const unsigned char *buf, size_t len )
{
	return 0;
}


void*	atiny_net_connect( const char *host, const char *port, int proto )
{
	return NULL;
}

#ifdef __cplusplus
}
#endif


