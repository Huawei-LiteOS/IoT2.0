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
 *    David Navarro, Intel Corporation - initial API and implementation
 *    Christian Renz - Please refer to git log
 *
 *******************************************************************************/

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "dtls_conn.h"
#include "dtls_interface.h"

#ifdef WITH_MBEDTLS


#define COAP_PORT "5683"
#define COAPS_PORT "5684"
#define URI_LENGTH 256


/********************* Security Obj Helpers **********************/
char * security_get_uri(lwm2m_object_t * obj, int instanceId, char * uriBuffer, int bufferSize){
    int size = 1;
    lwm2m_data_t * dataP = lwm2m_data_new(size);
    dataP->id = 0; // security server uri

    obj->readFunc(instanceId, &size, &dataP, obj);
    if (dataP != NULL &&
            dataP->type == LWM2M_TYPE_STRING &&
            dataP->value.asBuffer.length > 0)
    {
        if (bufferSize > dataP->value.asBuffer.length){
            memset(uriBuffer,0,dataP->value.asBuffer.length+1);
            strncpy(uriBuffer,(char *)dataP->value.asBuffer.buffer,dataP->value.asBuffer.length);
            lwm2m_data_free(size, dataP);
            return uriBuffer;
        }
    }
    lwm2m_data_free(size, dataP);
    return NULL;
}

int64_t security_get_mode(lwm2m_object_t * obj, int instanceId){
    int64_t mode;
    int size = 1;
    lwm2m_data_t * dataP = lwm2m_data_new(size);
    dataP->id = 2; // security mode

    obj->readFunc(instanceId, &size, &dataP, obj);
    if (0 != lwm2m_data_decode_int(dataP,&mode))
    {
        lwm2m_data_free(size, dataP);
        return mode;
    }

    lwm2m_data_free(size, dataP);
    fprintf(stderr, "Unable to get security mode : use not secure mode");
    return LWM2M_SECURITY_MODE_NONE;
}


dtls_conn_t * connection_create(dtls_conn_t * connList,
                                 lwm2m_object_t * securityObj,
                                 int instanceId,
                                 lwm2m_context_t * lwm2mH,
                                 int addressFamily)
{
    dtls_conn_t * connP = NULL;
    char uriBuf[URI_LENGTH];
    char * uri;
    char * host;
    char * port;
    int ret;

    
    fprintf(stderr, "now come into connection_create!!!");
    
    //if (security_get_mode(securityObj,instanceId)
    //                 == LWM2M_SECURITY_MODE_NONE)
    //{
    //    fprintf(stderr, "LWM2M_SECURITY_MODE_NONE in connection_create!!!");
    //    return NULL;
    //}

    uri = security_get_uri(securityObj, instanceId, uriBuf, URI_LENGTH);
    if (uri == NULL) 
    {
        fprintf(stderr, "uri is NULL!!!");
        return NULL;
    }
    printf("uri is %s\n",uri);
    
    // parse uri in the form "coaps://[host]:[port]"
    char * defaultport;
    if (0 == strncmp(uri, "coaps://", strlen("coaps://")))
    {
        host = uri+strlen("coaps://");
        defaultport = COAPS_PORT;
    }
    else if (0 == strncmp(uri, "coap://", strlen("coap://")))
    {
        host = uri+strlen("coap://");
        defaultport = COAP_PORT;
    }
    else
    {
        fprintf(stderr, "come here1!!!");
        return NULL;
    }
    port = strrchr(host, ':');
    if (port == NULL)
    {
        port = defaultport;
    }
    else
    {
        // remove brackets
        if (host[0] == '[')
        {
            host++;
            if (*(port - 1) == ']')
            {
                *(port - 1) = 0;
            }
            else
            {
                fprintf(stderr, "come here2!!!");
                return NULL;
            }
        }
        // split strings
        *port = 0;
        port++;
    }
    connP = (dtls_conn_t *)lwm2m_malloc(sizeof(dtls_conn_t));
    if(connP == NULL)
    {
        fprintf(stderr, "connP is NULL!!!");
        return NULL;
    }
    memset(connP, 0, sizeof(dtls_conn_t));
    connP->next = connList;
    connP->lastSend = lwm2m_gettime();
    connP->securityObj = securityObj;
    connP->securityInstId = instanceId;
    connP->lwm2mH = lwm2mH;

    unsigned char psk[16] = {0xef,0xe8,0x18,0x45,0xa3,0x53,0xc1,0x3c,0x0c,0x89,0x92,0xb3,0x1d,0x6b,0x6a,0x83};
    char *psk_identity = "666003";
    
    fprintf(stderr, "!!!!host is %s , port is %s,in connection_create",host,port);
    
    connP->ssl = dtls_ssl_new_with_psk(psk,16,psk_identity);
    printf("connP->ssl is %p in connection_create\n",connP->ssl);
    if(NULL == connP->ssl)
    {
        fprintf(stderr, "connP->ssl is NULL in connection_create");
        lwm2m_free(connP);
        return NULL;
    }
    ret = dtls_shakehand(connP->ssl,host,port);
    
    if(ret)
    {
        fprintf(stderr, "ret is %d in connection_create",ret);
        lwm2m_free(connP);
        return NULL;
    }

    
    

    return connP;
}

void connection_free(dtls_conn_t * connList)
{
    //add code here;
}

int connection_send(dtls_conn_t *connP, uint8_t * buffer, size_t length){

    printf("call dtls_write in connection_send, length is %d\n",length);
    
    if (connP->ssl == NULL) {
        // no security
        return -1;
    } 
    else 
    {
#if 0
        if (DTLS_NAT_TIMEOUT > 0 && (lwm2m_gettime() - connP->lastSend) > DTLS_NAT_TIMEOUT)
        {
            // we need to rehandhake because our source IP/port probably changed for the server
            if ( connection_rehandshake(connP, false) != 0 )
            {
                printf("can't send due to rehandshake error\n");
                return -1;
            }
        }
#endif 

        return dtls_write(connP->ssl, buffer, length);
        
    }


}

#if 0
int connection_rehandshake(dtls_connection_t *connP, bool sendCloseNotify) {

    // if not a dtls connection we do nothing
    if (connP->dtlsSession == NULL) {
        return 0;
    }

    // reset current session
    dtls_peer_t * peer = dtls_get_peer(connP->dtlsContext, connP->dtlsSession);
    if (peer != NULL)
    {
        if (!sendCloseNotify)
        {
            peer->state =  DTLS_STATE_CLOSED;
        }
        dtls_reset_peer(connP->dtlsContext, peer);
    }

    // start a fresh handshake
    int result = dtls_connect(connP->dtlsContext, connP->dtlsSession);
    if (result !=0) {
         printf("error dtls reconnection %d\n",result);
    }
    return result;
}
#endif

uint8_t lwm2m_buffer_send(void * sessionH,
                          uint8_t * buffer,
                          size_t length,
                          void * userdata)
{
    dtls_conn_t * connP = (dtls_conn_t*) sessionH;

    if (connP == NULL)
    {
        fprintf(stderr, "#> failed sending %lu bytes, missing connection\r\n", length);
        return COAP_500_INTERNAL_SERVER_ERROR ;
    }

    printf("call connection_send in lwm2m_buffer_send, length is %d\n",length);

    if (-1 == connection_send(connP, buffer, length))
    {
        fprintf(stderr, "#> failed sending %lu bytes\r\n", length);
        return COAP_500_INTERNAL_SERVER_ERROR ;
    }

    return COAP_NO_ERROR;
}

bool lwm2m_session_is_equal(void * session1,
                            void * session2,
                            void * userData)
{
    return (session1 == session2);
}

#endif

