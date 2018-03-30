
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include <lwip/sockets.h>
#include <lwip/netdb.h>
#include <lwip/errno.h>

#include "atiny_socket.h"
#include "atiny_adapter.h"
#include "agenttiny.h"

#ifdef __GNUC__
#include <errno.h>
#endif

#define _SOCKLEN_T

typedef struct
{
    int fd;
}
atiny_net_context;


/*
 * Check if the requested operation would be blocking on a non-blocking socket
 * and thus 'failed' with a negative return value.
 *
 * Note: on a blocking socket this function always returns 0!
 */
static int net_would_block( int ctx )
{
    /*
     * Never return 'WOULD BLOCK' on a non-blocking socket
     */
    if( ( fcntl( ctx, F_GETFL, 0 ) & O_NONBLOCK ) != O_NONBLOCK )
        return( 0 );

    switch( errno )
    {
#if defined EAGAIN
        case EAGAIN:
#endif
#if defined EWOULDBLOCK && EWOULDBLOCK != EAGAIN
        case EWOULDBLOCK:
#endif
            return( 1 );
    }
    return( 0 );
}


/*
 * Initiate a connection with host:port and the given protocol
 */
void* atiny_net_connect( const char *host, const char *port, int proto )
{
    int ret;
    struct addrinfo hints, *addr_list, *cur;
    atiny_net_context *ctx;

    /* Do name resolution with both IPv6 and IPv4 */
    memset( &hints, 0, sizeof( hints ) );
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = proto == ATINY_PROTO_UDP ? SOCK_DGRAM : SOCK_STREAM;
    hints.ai_protocol = proto == ATINY_PROTO_UDP ? IPPROTO_UDP : IPPROTO_TCP;

    if( getaddrinfo( host, port, &hints, &addr_list ) != 0 )
        return ( NULL );
		
		ctx = atiny_malloc(sizeof(atiny_net_context));
    ctx->fd= -1;

    /* Try the sockaddrs until a connection succeeds */
    ret = ATINY_ERR_UNKNOWN_HOST;
    for( cur = addr_list; cur != NULL; cur = cur->ai_next )
    {
        ctx->fd = (int) socket( cur->ai_family, cur->ai_socktype,
                            cur->ai_protocol );
        if( ctx->fd < 0 )
        {
            ret = ATINY_ERR_SOCKET_FAILED;
            continue;
        }

        if( connect( ctx->fd, cur->ai_addr, cur->ai_addrlen ) == 0
            && fcntl( ctx->fd, F_SETFL, fcntl( ctx->fd, F_GETFL, 0 ) | O_NONBLOCK ) == 0)
        {
            ret = ATINY_OK;
            break;
        }

        close( ctx->fd );
        ret = ATINY_ERR_CONNECT_FAILED;
    }

    freeaddrinfo( addr_list );

    if (ret != 0)
    {
        atiny_free(ctx);
        ctx = NULL;
    }

    return ( ctx );
}

/*
 * Read at most 'len' characters
 */
int atiny_net_recv( void *ctx, unsigned char *buf, size_t len )
{
    int ret;
    int fd = ((atiny_net_context*)ctx)->fd;

    if( fd < 0 )
        return( ATINY_ERR_INVALID_CONTEXT );

    ret = (int) read( fd, buf, len );

    if( ret < 0 )
    {
        if( net_would_block( fd ) != 0 )
            return( ATINY_ERR_WANT_READ );
        if( errno == EPIPE || errno == ECONNRESET )
            return( ATINY_ERR_CONN_RESET );
        if( errno == EINTR )
            return( ATINY_ERR_WANT_READ );

        return( ATINY_ERR_RECV_FAILED );
    }

    return( ret );
}

/*
 * Read at most 'len' characters, blocking for at most 'timeout' ms
 */
int atiny_net_recv_timeout( void *ctx, unsigned char *buf, size_t len,
                      uint32_t timeout )
{
    int ret;
    struct timeval tv;
    fd_set read_fds;
    int fd = ((atiny_net_context*)ctx)->fd;

    if( fd < 0 )
        return( ATINY_ERR_INVALID_CONTEXT );

    FD_ZERO( &read_fds );
    FD_SET( fd, &read_fds );

    tv.tv_sec  = timeout / 1000;
    tv.tv_usec = ( timeout % 1000 ) * 1000;

    ret = select( fd + 1, &read_fds, NULL, NULL, timeout == 0 ? NULL : &tv );
    /* Zero fds ready means we timed out */
    if( ret == 0 )
        return( ATINY_ERR_TIMEOUT );

    if( ret < 0 )
    {
        if( errno == EINTR )
            return( ATINY_ERR_WANT_READ );

        return( ATINY_ERR_RECV_FAILED );
    }

    /* This call will not block */
    return( atiny_net_recv( ctx, buf, len ) );
}

/*
 * Write at most 'len' characters
 */
int atiny_net_send( void *ctx, const unsigned char *buf, size_t len )
{
    int ret;
    int fd = ((atiny_net_context*)ctx)->fd;

    if( fd < 0 )
    {
        return( ATINY_ERR_INVALID_CONTEXT );
    }
    
    ret = (int) write( fd, buf, len );

    if( ret < 0 )
    {
        if( net_would_block( fd ) != 0 )
            return( ATINY_ERR_WANT_WRITE );
        if( errno == EPIPE || errno == ECONNRESET )
            return( ATINY_ERR_CONN_RESET );
        if( errno == EINTR )
            return( ATINY_ERR_WANT_WRITE );

        return( ATINY_ERR_SEND_FAILED );
    }

    return( ret );
}

/*
 * Gracefully close the connection
 */
void atiny_net_close( void *ctx )
{
    int fd = ((atiny_net_context*)ctx)->fd;
    if( fd == -1 )
        return;

    shutdown( fd, SHUT_RDWR );
    close( fd );
    atiny_free(ctx);
}

