/*******************************************************************************
 *
 * Copyright (c) 2015 Intel Corporation and others.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompany this distribution.
 *
 * The Eclipse Public License is available at
 *    http://www.eclipse.org/legal/epl-v10.html
 * The Eclipse Distribution License is available at
 *    http://www.eclipse.org/org/documents/edl-v10.php.
 *
 * Contributors:
 *    Simon Bernard - initial API and implementation
 *    Christian Renz - Please refer to git log
 *
 *******************************************************************************/

#ifndef DTLS_CONN_H_
#define DTLS_CONN_H_

#include <stdio.h>
//#include <netdb.h>
//#include <sys/socket.h>
//#include <sys/stat.h>
#include <stdint.h>
#include <stdbool.h>
#include "liblwm2m.h"


// after 40sec of inactivity we rehandshake
#define DTLS_NAT_TIMEOUT 40

typedef struct _connection_t
{
    struct _connection_t *  next;
    void* net_context;
    lwm2m_object_t * securityObj;
    int securityInstId;
	lwm2m_context_t * lwm2mH;
} connection_t;

connection_t * connection_create(connection_t * connList, lwm2m_object_t * securityObj,
    int instanceId, lwm2m_context_t * lwm2mH);

void connection_free(connection_t * connList);

//int connection_send(connection_t *connP, uint8_t * buffer, size_t length);

bool lwm2m_session_is_equal(void * session1, void * session2, void * userData);

uint8_t lwm2m_buffer_send(void * sessionH,
                          uint8_t * buffer,
                          size_t length,
                          void * userdata);
int lwm2m_buffer_recv(void * sessionH, uint8_t * buffer, size_t length, uint32_t timeout);

void lwm2m_close_connection(void * sessionH, void * userData);

//int connection_handle_packet(connection_t *connP, uint8_t * buffer, size_t length);

// rehandshake a connection, useful when your NAT timed out and your client has a new IP/PORT
//int connection_rehandshake(dtls_connection_t *connP, bool sendCloseNotify);

#endif
