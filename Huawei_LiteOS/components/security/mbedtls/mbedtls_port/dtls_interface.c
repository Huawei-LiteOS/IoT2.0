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

#include "dtls_interface.h"
#include "atiny_adapter.h"
#include "mbedtls/net_sockets.h"
static void my_debug( void *ctx, int level,
                      const char *file, int line,
                      const char *str )
{
    ((void) level);

    mbedtls_printf("%s:%04d: %s", file, line, str );
}

static void* atiny_calloc(size_t n, size_t size)
{
    return atiny_malloc(n*size);
}

mbedtls_ssl_context *dtls_ssl_new_with_psk(char *psk, unsigned psk_len, char *psk_identity)
{
    int ret;
    mbedtls_ssl_context *ssl;
    mbedtls_ssl_config *conf;
    mbedtls_entropy_context *entropy;
    mbedtls_ctr_drbg_context *ctr_drbg;
    
    
    const char *pers = "dtls_client";
    //const char *psk = "12345678";
    //const char *psk_identity = "Client_identity";  

    mbedtls_platform_set_calloc_free(atiny_calloc, atiny_free);
    mbedtls_platform_set_snprintf(atiny_snprintf);
    mbedtls_platform_set_printf(atiny_printf);
	
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
    mbedtls_ssl_conf_read_timeout( conf, 5000 );
    
#if defined(MBEDTLS_KEY_EXCHANGE__SOME__PSK_ENABLED)
    if( ( ret = mbedtls_ssl_conf_psk( conf, (const unsigned char *)psk, psk_len,
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
  
#if defined(MBEDTLS_X509_CRT_PARSE_C)
    if( ( ret = mbedtls_ssl_set_hostname( ssl, SERVER_NAME ) ) != 0 )
    {
        mbedtls_printf( " failed\n  ! mbedtls_ssl_set_hostname returned %d\n\n", ret );
        goto exit_fail;
    }
#endif
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
    mbedtls_printf( "  . Connecting to udp/%s/%s...", host, port );

    if( ( server_fd = mbedtls_net_connect( host,
                                         port, MBEDTLS_NET_PROTO_UDP ) ) == NULL )
    {
        mbedtls_printf( " failed\n  ! mbedtls_net_connect\n\n" );
        goto exit_fail;
    }

    mbedtls_ssl_set_bio( ssl, server_fd,
                         mbedtls_net_send, mbedtls_net_recv, mbedtls_net_recv_timeout );

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


int dtls_write(mbedtls_ssl_context *ssl, const unsigned char *buf, size_t len)
{
    int ret = 0;
    do ret = mbedtls_ssl_write( ssl, (unsigned char *) buf, len );
    while( ret == MBEDTLS_ERR_SSL_WANT_READ ||
           ret == MBEDTLS_ERR_SSL_WANT_WRITE );
    
    return ret;
}

int dtls_read(mbedtls_ssl_context *ssl, unsigned char *buf, size_t len, uint32_t timeout)
{
    int ret = 0;

    mbedtls_ssl_conf_read_timeout( ssl->conf, timeout );

    do ret = mbedtls_ssl_read( ssl, buf, len );
    while( ret == MBEDTLS_ERR_SSL_WANT_READ ||
           ret == MBEDTLS_ERR_SSL_WANT_WRITE );
    
    return ret;
}



