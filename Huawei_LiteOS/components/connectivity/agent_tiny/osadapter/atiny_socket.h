#ifndef _ATINY_SOCKET_H_
#define _ATINY_SOCKET_H_

#include <stddef.h>
#include <stdint.h>

#define ATINY_ERR_SOCKET_FAILED                     -0x0042  /**< Failed to open a socket. */
#define ATINY_ERR_CONNECT_FAILED                    -0x0044  /**< The connection to the given server / port failed. */
#define ATINY_ERR_BIND_FAILED                       -0x0046  /**< Binding of the socket failed. */
#define ATINY_ERR_LISTEN_FAILED                     -0x0048  /**< Could not listen on the socket. */
#define ATINY_ERR_ACCEPT_FAILED                     -0x004A  /**< Could not accept the incoming connection. */
#define ATINY_ERR_RECV_FAILED                       -0x004C  /**< Reading information from the socket failed. */
#define ATINY_ERR_SEND_FAILED                       -0x004E  /**< Sending information through the socket failed. */
#define ATINY_ERR_CONN_RESET                        -0x0050  /**< Connection was reset by peer. */
#define ATINY_ERR_UNKNOWN_HOST                      -0x0052  /**< Failed to get an IP address for the given hostname. */
#define ATINY_ERR_BUFFER_TOO_SMALL                  -0x0043  /**< Buffer is too small to hold the data. */
#define ATINY_ERR_INVALID_CONTEXT                   -0x0045  /**< The context is invalid, eg because it was free()ed. */

#define ATINY_ERR_WANT_READ                         -0x6900  /**< Connection requires a read call. */
#define ATINY_ERR_WANT_WRITE                        -0x6880  /**< Connection requires a write call. */
#define ATINY_ERR_TIMEOUT                           -0x6800  /**< The operation timed out. */


#define ATINY_PROTO_TCP 0 /**< The TCP transport protocol */
#define ATINY_PROTO_UDP 1 /**< The UDP transport protocol */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief          Create socket and connect to server
 * 
 * \param host     Host to connect to
 * \param port     Port to connect to
 * \param proto    Protocol: ATINY_PROTO_TCP or ATINY_PROTO_UDP
 *
 * \return         Socket you created if succeed, or NULL if failed
 */
void* atiny_net_connect( const char *host, const char *port, int proto );

/**
 * \brief          Read at most 'len' characters. If no error occurs,
 *                 the actual amount read is returned.
 *
 * \param ctx      Socket
 * \param buf      The buffer to write to
 * \param len      Maximum length of the buffer
 *
 * \return         the number of bytes received,
 *                 or a non-zero error code; with a non-blocking socket,
 *                 ATINY_ERR_WANT_READ indicates read() would block.
 */
int atiny_net_recv( void *ctx, unsigned char *buf, size_t len );

/**
 * \brief          Write at most 'len' characters. If no error occurs,
 *                 the actual amount read is returned.
 *
 * \param ctx      Socket
 * \param buf      The buffer to read from
 * \param len      The length of the buffer
 *
 * \return         the number of bytes sent,
 *                 or a non-zero error code; with a non-blocking socket,
 *                 ATINY_ERR_WANT_WRITE indicates write() would block.
 */
int atiny_net_send( void *ctx, const unsigned char *buf, size_t len );

/**
 * \brief          Read at most 'len' characters, blocking for at most
 *                 'timeout' seconds. If no error occurs, the actual amount
 *                 read is returned.
 *
 * \param ctx      Socket
 * \param buf      The buffer to write to
 * \param len      Maximum length of the buffer
 * \param timeout  Maximum number of milliseconds to wait for data
 *                 0 means no timeout (wait forever)
 *
 * \return         the number of bytes received,
 *                 or a non-zero error code:
 *                 ATINY_ERR_TIMEOUT if the operation timed out,
 *                 ATINY_ERR_WANT_READ if interrupted by a signal.
 *
 * \note           This function will block (until data becomes available or
 *                 timeout is reached) even if the socket is set to
 *                 non-blocking. Handling timeouts with non-blocking reads
 *                 requires a different strategy.
 */
int atiny_net_recv_timeout( void *ctx, unsigned char *buf, size_t len,
                      uint32_t timeout );

/**
 * \brief          Gracefully shutdown the connection and free associated data
 *
 * \param ctx      The context to free
 */
void atiny_net_close( void *ctx );

#ifdef __cplusplus
}
#endif

#endif /* _ATINY_SOCKET_H_ */

