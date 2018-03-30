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
#ifdef __cplusplus
extern "C" {
#endif


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

//#define SERVER_PORT "4433"
//#define SERVER_NAME "dtls_server"
//#define SERVER_ADDR "192.168.0.109" /* forces IPv4 */
//#define MESSAGE     "Test DTLS LiteOS V2"

#define SERVER_PORT "5684"
#define SERVER_NAME "dtls_server"
#define SERVER_ADDR "139.159.209.89" /* forces IPv4 */
//#define SERVER_ADDR "5.39.83.206" /* forces IPv4 */
//#define SERVER_ADDR "192.168.0.116" /* forces IPv4 */


#define MESSAGE     "Test DTLS LiteOS V2"


#define READ_TIMEOUT_MS 1000
#define MAX_RETRY       5

#define DEBUG_LEVEL 0

#define DTLS_RECVBUF_LEN  1024


mbedtls_ssl_context *dtls_ssl_new_with_psk(char *psk, unsigned psk_len, char *psk_identity);
int dtls_shakehand(mbedtls_ssl_context *ssl, const char *host, const char *port);
void dtls_ssl_destroy(mbedtls_ssl_context *ssl);
int dtls_write(mbedtls_ssl_context *ssl, const unsigned char *buf, size_t len);
int dtls_read(mbedtls_ssl_context *ssl, unsigned char *buf, size_t len, uint32_t timeout);



#ifdef __cplusplus
}
#endif


