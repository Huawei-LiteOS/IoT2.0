/*
 *  Simple DTLS client demonstration program
 *
 *  Copyright (C) 2006-2015, ARM Limited, All Rights Reserved
 *  SPDX-License-Identifier: Apache-2.0
 *
 *  Licensed under the Apache License, Version 2.0 (the "License"); you may
 *  not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 *  WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *  This file is part of mbed TLS (https://tls.mbed.org)
 */

#if !defined(MBEDTLS_CONFIG_FILE)
#include "los_mbedtls_config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif

#if defined(MBEDTLS_PLATFORM_C)
#include "mbedtls/platform.h"
#else
#include <stdio.h>
#define mbedtls_printf     printf
#define mbedtls_fprintf    fprintf
#endif


#include <string.h>

#include "mbedtls/net_sockets.h"
#include "mbedtls/debug.h"
#include "mbedtls/ssl.h"
#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/error.h"
#include "mbedtls/certs.h"
#include "mbedtls/timing.h"

#define SERVER_PORT "4433"
#define SERVER_NAME "dtls_server"
#define SERVER_ADDR "192.168.1.100" /* forces IPv4 */
#define MESSAGE     "Test DTLS LiteOS V2"

#define READ_TIMEOUT_MS 1000
#define MAX_RETRY       5

#define DEBUG_LEVEL 0

#define DTLS_RECVBUF_LEN  1024

static void my_debug( void *ctx, int level,
                      const char *file, int line,
                      const char *str )
{
    ((void) level);

    mbedtls_printf("%s:%04d: %s", file, line, str );
}

int test_dtls(void)
{
    int ret, len;
    mbedtls_net_context server_fd;
    unsigned char buf[1024];
    const char *pers = "dtls_client";
    int retry_left = MAX_RETRY;
	const char *psk = "12345678";
	const char *psk_identity = "Client_identity";
    mbedtls_entropy_context entropy;
    mbedtls_ctr_drbg_context ctr_drbg;
    mbedtls_ssl_context ssl;
    mbedtls_ssl_config conf;

    mbedtls_timing_delay_context timer;

#if defined(MBEDTLS_DEBUG_C)
    mbedtls_debug_set_threshold( DEBUG_LEVEL );
#endif

    /*
     * 0. Initialize the RNG and the session data
     */
    mbedtls_net_init( &server_fd );
    mbedtls_ssl_init( &ssl );
    mbedtls_ssl_config_init( &conf );
    mbedtls_ctr_drbg_init( &ctr_drbg );

    mbedtls_printf( "\n  . Seeding the random number generator..." );

    mbedtls_entropy_init( &entropy );
    if( ( ret = mbedtls_ctr_drbg_seed( &ctr_drbg, mbedtls_entropy_func, &entropy,
                               (const unsigned char *) pers,
                               strlen( pers ) ) ) != 0 )
    {
        mbedtls_printf( " failed\n  ! mbedtls_ctr_drbg_seed returned %d\n", ret );
        goto exit;
    }

    mbedtls_printf( " ok\n" );

    /*
     * 1. Start the connection
     */
    mbedtls_printf( "  . Connecting to udp/%s/%s...", SERVER_NAME, SERVER_PORT );

    if( ( ret = mbedtls_net_connect( &server_fd, SERVER_ADDR,
                                         SERVER_PORT, MBEDTLS_NET_PROTO_UDP ) ) != 0 )
    {
        mbedtls_printf( " failed\n  ! mbedtls_net_connect returned %d\n\n", ret );
        goto exit;
    }

    mbedtls_printf( " ok\n" );

    /*
     * 2. Setup stuff
     */
    mbedtls_printf( "  . Setting up the DTLS structure..." );

    if( ( ret = mbedtls_ssl_config_defaults( &conf,
                   MBEDTLS_SSL_IS_CLIENT,
                   MBEDTLS_SSL_TRANSPORT_DATAGRAM,
                   MBEDTLS_SSL_PRESET_DEFAULT ) ) != 0 )
    {
        mbedtls_printf( " failed\n  ! mbedtls_ssl_config_defaults returned %d\n\n", ret );
        goto exit;
    }

    /* OPTIONAL is usually a bad choice for security, but makes interop easier
     * in this simplified example, in which the ca chain is hardcoded.
     * Production code should set a proper ca chain and use REQUIRED. */
    mbedtls_ssl_conf_authmode( &conf, MBEDTLS_SSL_VERIFY_OPTIONAL );
    mbedtls_ssl_conf_rng( &conf, mbedtls_ctr_drbg_random, &ctr_drbg );
    mbedtls_ssl_conf_dbg( &conf, my_debug, stdout );
	
#if defined(MBEDTLS_KEY_EXCHANGE__SOME__PSK_ENABLED)
    if( ( ret = mbedtls_ssl_conf_psk( &conf, (const unsigned char *)psk, strlen(psk),
                             (const unsigned char *) psk_identity,
                             strlen( psk_identity ) ) ) != 0 )
    {
        mbedtls_printf( " failed\n  ! mbedtls_ssl_conf_psk returned %d\n\n", ret );
        goto exit;
    }
#endif

    if( ( ret = mbedtls_ssl_setup( &ssl, &conf ) ) != 0 )
    {
        mbedtls_printf( " failed\n  ! mbedtls_ssl_setup returned %d\n\n", ret );
        goto exit;
    }

    if( ( ret = mbedtls_ssl_set_hostname( &ssl, SERVER_NAME ) ) != 0 )
    {
        mbedtls_printf( " failed\n  ! mbedtls_ssl_set_hostname returned %d\n\n", ret );
        goto exit;
    }

    mbedtls_ssl_set_bio( &ssl, &server_fd,
                         mbedtls_net_send, mbedtls_net_recv, NULL );

    mbedtls_ssl_set_timer_cb( &ssl, &timer, mbedtls_timing_set_delay,
                                            mbedtls_timing_get_delay );

    mbedtls_printf( " ok\n" );

    /*
     * 3. Handshake
     */
    mbedtls_printf( "  . Performing the SSL/TLS handshake..." );

    do ret = mbedtls_ssl_handshake( &ssl );
    while( ret == MBEDTLS_ERR_SSL_WANT_READ ||
           ret == MBEDTLS_ERR_SSL_WANT_WRITE );

    if( ret != 0 )
    {
        mbedtls_printf( " failed\n  ! mbedtls_ssl_handshake returned -0x%x\n\n", -ret );
        goto exit;
    }

    mbedtls_printf( " ok\n" );

    /*
     * 4. Write the echo request
     */
send_request:
    mbedtls_printf( "  > Write to server:" );

    len = sizeof( MESSAGE ) - 1;

    do ret = mbedtls_ssl_write( &ssl, (unsigned char *) MESSAGE, len );
    while( ret == MBEDTLS_ERR_SSL_WANT_READ ||
           ret == MBEDTLS_ERR_SSL_WANT_WRITE );

    if( ret < 0 )
    {
        mbedtls_printf( " failed\n  ! mbedtls_ssl_write returned %d\n\n", ret );
        goto exit;
    }

    len = ret;
    mbedtls_printf( " %d bytes written\n\n%s\n\n", len, MESSAGE );

    /*
     * 5. Read the echo response
     */
    mbedtls_printf( "  < Read from server:" );

    len = sizeof( buf ) - 1;
    memset( buf, 0, sizeof( buf ) );

    do ret = mbedtls_ssl_read( &ssl, buf, len );
    while( ret == MBEDTLS_ERR_SSL_WANT_READ ||
           ret == MBEDTLS_ERR_SSL_WANT_WRITE );

    if( ret <= 0 )
    {
        switch( ret )
        {
            case MBEDTLS_ERR_SSL_TIMEOUT:
                mbedtls_printf( " timeout\n\n" );
                if( retry_left-- > 0 )
                    goto send_request;
                goto exit;

            case MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY:
                mbedtls_printf( " connection was closed gracefully\n" );
                ret = 0;
                goto close_notify;

            default:
                mbedtls_printf( " mbedtls_ssl_read returned -0x%x\n\n", -ret );
                goto exit;
        }
    }

    len = ret;
    mbedtls_printf( " %d bytes read\n\n%s\n\n", len, buf );

    /*
     * 6. Done, cleanly close the connection
     */
close_notify:
    mbedtls_printf( "  . Closing the connection..." );

    /* No error checking, the connection might be closed already */
    do ret = mbedtls_ssl_close_notify( &ssl );
    while( ret == MBEDTLS_ERR_SSL_WANT_WRITE );
    ret = 0;

    mbedtls_printf( " done\n" );

    /*
     * 9. Final clean-ups and exit
     */
exit:

#ifdef MBEDTLS_ERROR_C
    if( ret != 0 )
    {
        char error_buf[100];
        mbedtls_strerror( ret, error_buf, 100 );
        mbedtls_printf( "Last error was: %d - %s\n\n", ret, error_buf );
    }
#endif

    mbedtls_net_free( &server_fd );
    mbedtls_ssl_free( &ssl );
    mbedtls_ssl_config_free( &conf );
    mbedtls_ctr_drbg_free( &ctr_drbg );
    mbedtls_entropy_free( &entropy );

    /* Shell can not handle large exit numbers -> 1 for errors */
    if( ret < 0 )
        ret = 1;

    return( ret );
}



int test_dtls2(void)
{
    int ret, len;
    mbedtls_net_context *server_fd;
    unsigned char *buf;/*1024*/
    int retry_left = MAX_RETRY;
    mbedtls_entropy_context *entropy;
    mbedtls_ctr_drbg_context *ctr_drbg;
    mbedtls_ssl_context *ssl;
    mbedtls_ssl_config *conf;
    const char *pers = "dtls_client";
    //const char *psk = "12345678";
    //const char *psk_identity = "Client_identity";
    const char *psk = "e8bb121d8eb6f6028540f622d8bf59d3";
    const char *psk_identity = "urn:imei:0123456789";

    mbedtls_timing_delay_context *timer;

#if defined(MBEDTLS_DEBUG_C)
    mbedtls_debug_set_threshold( DEBUG_LEVEL );
#endif

    ssl       = mbedtls_calloc(1,sizeof(mbedtls_ssl_context));
    conf      = mbedtls_calloc(1,sizeof(mbedtls_ssl_config));
    entropy   = mbedtls_calloc(1,sizeof(mbedtls_entropy_context));
	ctr_drbg  = mbedtls_calloc(1,sizeof(mbedtls_ctr_drbg_context));
    buf       = mbedtls_calloc(1,DTLS_RECVBUF_LEN);
    
	timer     = mbedtls_calloc(1,sizeof(mbedtls_timing_delay_context));
    /*
     * 0. Initialize the RNG and the session data
     */
    server_fd = mbedtls_calloc(1,sizeof(mbedtls_net_context));
    mbedtls_net_init( server_fd );
    mbedtls_ssl_init( ssl );
    mbedtls_ssl_config_init( conf );
    mbedtls_ctr_drbg_init( ctr_drbg );

    mbedtls_printf( "\n  . Seeding the random number generator..." );

    mbedtls_entropy_init( entropy );
    if( ( ret = mbedtls_ctr_drbg_seed( ctr_drbg, mbedtls_entropy_func, entropy,
                               (const unsigned char *) pers,
                               strlen( pers ) ) ) != 0 )
    {
        mbedtls_printf( " failed\n  ! mbedtls_ctr_drbg_seed returned %d\n", ret );
        goto exit;
    }

    mbedtls_printf( " ok\n" );

    
    
    /*
     * 1. Start the connection
     */
    mbedtls_printf( "  . Connecting to udp/%s/%s...", SERVER_NAME, SERVER_PORT );

    if( ( ret = mbedtls_net_connect( server_fd, SERVER_ADDR,
                                         SERVER_PORT, MBEDTLS_NET_PROTO_UDP ) ) != 0 )
    {
        mbedtls_printf( " failed\n  ! mbedtls_net_connect returned %d\n\n", ret );
        goto exit;
    }

    mbedtls_printf( " ok\n" );
    /*
     * 2. Setup stuff
     */
    mbedtls_printf( "  . Setting up the DTLS structure..." );

    if( ( ret = mbedtls_ssl_config_defaults( conf,
                   MBEDTLS_SSL_IS_CLIENT,
                   MBEDTLS_SSL_TRANSPORT_DATAGRAM,
                   MBEDTLS_SSL_PRESET_DEFAULT ) ) != 0 )
    {
        mbedtls_printf( " failed\n  ! mbedtls_ssl_config_defaults returned %d\n\n", ret );
        goto exit;
    }

    /* OPTIONAL is usually a bad choice for security, but makes interop easier
     * in this simplified example, in which the ca chain is hardcoded.
     * Production code should set a proper ca chain and use REQUIRED. */
    mbedtls_ssl_conf_authmode( conf, MBEDTLS_SSL_VERIFY_OPTIONAL );
    mbedtls_ssl_conf_rng( conf, mbedtls_ctr_drbg_random, ctr_drbg );
    mbedtls_ssl_conf_dbg( conf, my_debug, stdout );
	
#if defined(MBEDTLS_KEY_EXCHANGE__SOME__PSK_ENABLED)
    if( ( ret = mbedtls_ssl_conf_psk( conf, (const unsigned char *)psk, strlen(psk),
                             (const unsigned char *) psk_identity,
                             strlen( psk_identity ) ) ) != 0 )
    {
        mbedtls_printf( " failed\n  ! mbedtls_ssl_conf_psk returned %d\n\n", ret );
        goto exit;
    }
#endif

    if( ( ret = mbedtls_ssl_setup( ssl, conf ) ) != 0 )
    {
        mbedtls_printf( " failed\n  ! mbedtls_ssl_setup returned %d\n\n", ret );
        goto exit;
    }

    if( ( ret = mbedtls_ssl_set_hostname( ssl, SERVER_NAME ) ) != 0 )
    {
        mbedtls_printf( " failed\n  ! mbedtls_ssl_set_hostname returned %d\n\n", ret );
        goto exit;
    }

    

    mbedtls_ssl_set_bio( ssl, server_fd,
                         mbedtls_net_send, mbedtls_net_recv, NULL );

    mbedtls_ssl_set_timer_cb( ssl, timer, mbedtls_timing_set_delay,
                                            mbedtls_timing_get_delay );

    mbedtls_printf( " ok\n" );

    /*
     * 3. Handshake
     */
    mbedtls_printf( "  . Performing the SSL/TLS handshake..." );

    do ret = mbedtls_ssl_handshake( ssl );
    while( ret == MBEDTLS_ERR_SSL_WANT_READ ||
           ret == MBEDTLS_ERR_SSL_WANT_WRITE );

    if( ret != 0 )
    {
        mbedtls_printf( " failed\n  ! mbedtls_ssl_handshake returned -0x%x\n\n", -ret );
        goto exit;
    }

    mbedtls_printf( " ok\n" );

    /*
     * 4. Write the echo request
     */
send_request:
    mbedtls_printf( "  > Write to server:" );

    len = sizeof( MESSAGE ) - 1;

    do ret = mbedtls_ssl_write( ssl, (unsigned char *) MESSAGE, len );
    while( ret == MBEDTLS_ERR_SSL_WANT_READ ||
           ret == MBEDTLS_ERR_SSL_WANT_WRITE );

    if( ret < 0 )
    {
        mbedtls_printf( " failed\n  ! mbedtls_ssl_write returned %d\n\n", ret );
        goto exit;
    }

    len = ret;
    mbedtls_printf( " %d bytes written\n\n%s\n\n", len, MESSAGE );

    /*
     * 5. Read the echo response
     */
    mbedtls_printf( "  < Read from server:" );

    len = DTLS_RECVBUF_LEN - 1;
    memset( buf, 0, DTLS_RECVBUF_LEN );

    do ret = mbedtls_ssl_read( ssl, buf, len );
    while( ret == MBEDTLS_ERR_SSL_WANT_READ ||
           ret == MBEDTLS_ERR_SSL_WANT_WRITE );

    if( ret <= 0 )
    {
        switch( ret )
        {
            case MBEDTLS_ERR_SSL_TIMEOUT:
                mbedtls_printf( " timeout\n\n" );
                if( retry_left-- > 0 )
                    goto send_request;
                goto exit;

            case MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY:
                mbedtls_printf( " connection was closed gracefully\n" );
                ret = 0;
                goto close_notify;

            default:
                mbedtls_printf( " mbedtls_ssl_read returned -0x%x\n\n", -ret );
                goto exit;
        }
    }

    len = ret;
    mbedtls_printf( " %d bytes read\n\n%s\n\n", len, buf );

    /*
     * 6. Done, cleanly close the connection
     */
close_notify:
    mbedtls_printf( "  . Closing the connection..." );

    /* No error checking, the connection might be closed already */
    do ret = mbedtls_ssl_close_notify( ssl );
    while( ret == MBEDTLS_ERR_SSL_WANT_WRITE );
    ret = 0;

    mbedtls_printf( " done\n" );

    /*
     * 9. Final clean-ups and exit
     */
exit:

#ifdef MBEDTLS_ERROR_C
    if( ret != 0 )
    {
        char error_buf[100];
        mbedtls_strerror( ret, error_buf, 100 );
        mbedtls_printf( "Last error was: %d - %s\n\n", ret, error_buf );
    }
#endif

    mbedtls_net_free( server_fd );
    mbedtls_ssl_free( ssl );
    mbedtls_ssl_config_free( conf );
    mbedtls_ctr_drbg_free( ctr_drbg );
    mbedtls_entropy_free( entropy );

	mbedtls_free(timer);
	mbedtls_free(buf);
	mbedtls_free(server_fd);
	mbedtls_free(ctr_drbg);
	mbedtls_free(entropy);
	mbedtls_free(conf);
	mbedtls_free(ssl);

    /* Shell can not handle large exit numbers -> 1 for errors */
    if( ret < 0 )
        ret = 1;

    return( ret );
}



mbedtls_ssl_context *dtls_ssl_new(void)
{
    int ret;
    mbedtls_ssl_context *ssl;
    mbedtls_ssl_config *conf;
    mbedtls_entropy_context *entropy;
    mbedtls_ctr_drbg_context *ctr_drbg;
    
    
	const char *pers = "dtls_client";
    const char *psk = "12345678";
    const char *psk_identity = "Client_identity";  
	
    ssl       = mbedtls_calloc(1,sizeof(mbedtls_ssl_context));
    conf      = mbedtls_calloc(1,sizeof(mbedtls_ssl_config));
    entropy   = mbedtls_calloc(1,sizeof(mbedtls_entropy_context));
    ctr_drbg  = mbedtls_calloc(1,sizeof(mbedtls_ctr_drbg_context));
    
    
    
    if(NULL == ssl || NULL == conf || entropy == NULL ||
       NULL == ctr_drbg)
    {
         goto exit_fail;
    }
    /*
     * 0. Initialize the RNG and the session data
     */
    
    mbedtls_ssl_init( ssl );
    mbedtls_ssl_config_init( conf );
    mbedtls_ctr_drbg_init( ctr_drbg );
    mbedtls_entropy_init( entropy );
    

    if( ( ret = mbedtls_ctr_drbg_seed( ctr_drbg, mbedtls_entropy_func, entropy,
                               (const unsigned char *) pers,
                               strlen( pers ) ) ) != 0 )
    {
        mbedtls_printf( " failed\n  ! mbedtls_ctr_drbg_seed returned %d\n", ret );
        goto exit_fail;
    }
    
    

    

    /*
     * 2. Setup stuff
     */
    mbedtls_printf( "  . Setting up the DTLS structure..." );

    if( ( ret = mbedtls_ssl_config_defaults( conf,
                   MBEDTLS_SSL_IS_CLIENT,
                   MBEDTLS_SSL_TRANSPORT_DATAGRAM,
                   MBEDTLS_SSL_PRESET_DEFAULT ) ) != 0 )
    {
        mbedtls_printf( " failed\n  ! mbedtls_ssl_config_defaults returned %d\n\n", ret );
        goto exit_fail;
    }

    /* OPTIONAL is usually a bad choice for security, but makes interop easier
     * in this simplified example, in which the ca chain is hardcoded.
     * Production code should set a proper ca chain and use REQUIRED. */
    mbedtls_ssl_conf_authmode( conf, MBEDTLS_SSL_VERIFY_OPTIONAL );
    mbedtls_ssl_conf_rng( conf, mbedtls_ctr_drbg_random, ctr_drbg );
    mbedtls_ssl_conf_dbg( conf, my_debug, stdout );
    
#if defined(MBEDTLS_KEY_EXCHANGE__SOME__PSK_ENABLED)
    if( ( ret = mbedtls_ssl_conf_psk( conf, (const unsigned char *)psk, strlen(psk),
                             (const unsigned char *) psk_identity,
                             strlen( psk_identity ) ) ) != 0 )
    {
        mbedtls_printf( " failed\n  ! mbedtls_ssl_conf_psk returned %d\n\n", ret );
        goto exit_fail;
    }
#endif
    if( ( ret = mbedtls_ssl_setup( ssl, conf ) ) != 0 )
    {
        mbedtls_printf( " failed\n  ! mbedtls_ssl_setup returned %d\n\n", ret );
        goto exit_fail;
    }
  

    if( ( ret = mbedtls_ssl_set_hostname( ssl, SERVER_NAME ) ) != 0 )
    {
        mbedtls_printf( " failed\n  ! mbedtls_ssl_set_hostname returned %d\n\n", ret );
        goto exit_fail;
    }

    mbedtls_printf( " ok\n" );

    return ssl;

exit_fail:
    if(ctr_drbg)mbedtls_free(ctr_drbg);
    if(entropy)mbedtls_free(entropy);
    if(conf)mbedtls_free(conf);
    if(ssl)mbedtls_free(ssl);
    
    return NULL;
}

int dtls_shakehand(mbedtls_ssl_context *ssl, const char *host, const char *port)
{
    int ret;
    mbedtls_net_context *server_fd;
    mbedtls_timing_delay_context *timer;

    server_fd = mbedtls_calloc(1,sizeof(mbedtls_net_context));
    timer     = mbedtls_calloc(1,sizeof(mbedtls_timing_delay_context));
   

    if(NULL == server_fd || NULL == timer)
    {
         ret = MBEDTLS_ERR_SSL_ALLOC_FAILED;
         goto exit_fail;
    }
    
    /*
      * 1. Start the connection
      */
    mbedtls_net_init( server_fd );
    mbedtls_printf( "  . Connecting to udp/%s/%s...", host, port );

    if( ( ret = mbedtls_net_connect( server_fd, host,
                                         port, MBEDTLS_NET_PROTO_UDP ) ) != 0 )
    {
        mbedtls_printf( " failed\n  ! mbedtls_net_connect returned %d\n\n", ret );
        goto exit_fail;
    }

    mbedtls_ssl_set_bio( ssl, server_fd,
                         mbedtls_net_send, mbedtls_net_recv, NULL );

    mbedtls_ssl_set_timer_cb( ssl, timer, mbedtls_timing_set_delay,
                                            mbedtls_timing_get_delay );

    /*
     * 3. Handshake
     */
    mbedtls_printf( "  . Performing the SSL/TLS handshake..." );

    do ret = mbedtls_ssl_handshake( ssl );
    while( ret == MBEDTLS_ERR_SSL_WANT_READ ||
           ret == MBEDTLS_ERR_SSL_WANT_WRITE );

    if( ret != 0 )
    {
        mbedtls_printf( " failed\n  ! mbedtls_ssl_handshake returned -0x%x\n\n", -ret );
        goto exit_fail;
    }


    mbedtls_printf( " ok\n" );
    return 0;

exit_fail:
    
    if(server_fd)mbedtls_free(server_fd);
    if(timer)mbedtls_free(timer);
    return ret;
    
}
void dtls_ssl_destroy(mbedtls_ssl_context *ssl)
{
    
    mbedtls_ssl_config           *conf;
    mbedtls_ctr_drbg_context     *ctr_drbg;
    mbedtls_entropy_context      *entropy;
    mbedtls_net_context          *server_fd;
    mbedtls_timing_delay_context *timer;
    
    conf       = ssl->conf;
    ctr_drbg   = conf->p_rng;
    entropy    =  ctr_drbg->p_entropy;
    server_fd  = (mbedtls_net_context *)ssl->p_bio;
    timer      = (mbedtls_timing_delay_context *)ssl->p_timer;
        
    mbedtls_net_free( server_fd );
    mbedtls_ssl_free( ssl );
    mbedtls_ssl_config_free( conf );
    mbedtls_ctr_drbg_free( ctr_drbg );
    mbedtls_entropy_free( entropy );

	mbedtls_free(timer);
	mbedtls_free(server_fd);
	mbedtls_free(ctr_drbg);
	mbedtls_free(entropy);
	mbedtls_free(conf);
	mbedtls_free(ssl);

}

int dtls_sendrecv_test(void)
{
    int ret, len;
    unsigned char *buf;/*1024*/

    mbedtls_ssl_context *ssl;

    ssl = dtls_ssl_new();

    if(ssl == NULL)
        return -1;
    ret = dtls_shakehand(ssl, SERVER_ADDR, SERVER_PORT);
    if(ret != 0)
        return -1;
    
    
    
    mbedtls_printf( "  > Write to server:" );

    buf = mbedtls_calloc(1,DTLS_RECVBUF_LEN);

    len = sizeof( MESSAGE ) - 1;

    do ret = mbedtls_ssl_write( ssl, (unsigned char *) MESSAGE, len );
    while( ret == MBEDTLS_ERR_SSL_WANT_READ ||
           ret == MBEDTLS_ERR_SSL_WANT_WRITE );

    if( ret < 0 )
    {
        mbedtls_printf( " failed\n  ! mbedtls_ssl_write returned %d\n\n", ret );
        goto exit;
    }

    len = ret;
    mbedtls_printf( " %d bytes written\n\n%s\n\n", len, MESSAGE );

    /*
     * 5. Read the echo response
     */
    mbedtls_printf( "  < Read from server:" );

    len = DTLS_RECVBUF_LEN - 1;
    memset( buf, 0, DTLS_RECVBUF_LEN );

    do ret = mbedtls_ssl_read( ssl, buf, len );
    while( ret == MBEDTLS_ERR_SSL_WANT_READ ||
           ret == MBEDTLS_ERR_SSL_WANT_WRITE );

    if( ret <= 0 )
    {
        switch( ret )
        {
            case MBEDTLS_ERR_SSL_TIMEOUT:
                mbedtls_printf( " timeout\n\n" );
                
                goto exit;

            case MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY:
                mbedtls_printf( " connection was closed gracefully\n" );
                ret = 0;
                goto close_notify;

            default:
                mbedtls_printf( " mbedtls_ssl_read returned -0x%x\n\n", -ret );
                goto exit;
        }
    }

    len = ret;
    mbedtls_printf( " %d bytes read\n\n%s\n\n", len, buf );  

close_notify:
    mbedtls_printf( "  . Closing the connection..." );
    
    /* No error checking, the connection might be closed already */
    do ret = mbedtls_ssl_close_notify( ssl );
    while( ret == MBEDTLS_ERR_SSL_WANT_WRITE );
    ret = 0;
    
    mbedtls_printf( " done\n" );

exit:
    mbedtls_free( buf );
    dtls_ssl_destroy( ssl );
    return ret;
}




