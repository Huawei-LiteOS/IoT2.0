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
#include "atiny_socket.h"
#include "object_comm.h"

#define COAP_PORT "5683"
#define COAPS_PORT "5684"


connection_t * connection_create(connection_t * connList,
                                 lwm2m_object_t * securityObj,
                                 int instanceId,
                                 lwm2m_context_t * lwm2mH)
{
    connection_t * connP = NULL;
    char * uri;
    char * host;
    char * port;
    int ret;
    fprintf(stderr, "now come into connection_create!!!");
  
    security_instance_t * targetP = (security_instance_t *)LWM2M_LIST_FIND(securityObj->instanceList, instanceId);
    if (NULL == targetP)
    {
        return NULL;
    }

    uri = targetP->uri;
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

	connP = (connection_t *)lwm2m_malloc(sizeof(connection_t));
	if(connP == NULL)
	{
		fprintf(stderr, "connP is NULL!!!");
		return NULL;
	}
	memset(connP, 0, sizeof(connection_t));

  if (targetP->securityMode != LWM2M_SECURITY_MODE_NONE)
    {
        //unsigned char psk[16] = {0xef,0xe8,0x18,0x45,0xa3,0x53,0xc1,0x3c,0x0c,0x89,0x92,0xb3,0x1d,0x6b,0x6a,0x83};
        //char *psk_identity = "666003";
        		
        connP->net_context = (void*)dtls_ssl_new_with_psk(targetP->secretKey, targetP->secretKeyLen, targetP->publicIdentity);
	    if(NULL == connP->net_context)
	    {
	        fprintf(stderr, "connP->ssl is NULL in connection_create");
	        lwm2m_free(connP);
	        return NULL;
	    }
	    ret = dtls_shakehand(connP->net_context,host,port);
	    
	    if(ret != 0)
	    {
	        fprintf(stderr, "ret is %d in connection_create",ret);
	        lwm2m_free(connP);
	        return NULL;
	    }
			  connP->dtls_flag = true;			
    }
    else
    {
        // no dtls session
        connP->net_context = atiny_net_connect(host,port, ATINY_PROTO_UDP);
				if(NULL == connP->net_context)
				{
						fprintf(stderr, "connP->ssl is NULL in connection_create");
						lwm2m_free(connP);
						return NULL;
				}
			  connP->dtls_flag = false;
    }
	connP->next = connList;
    connP->securityObj = securityObj;
	connP->securityInstId = instanceId;
	connP->lwm2mH = lwm2mH;

    return connP;
}

void connection_free(connection_t * connP)
{
    if (connP->dtls_flag == false) {
        // no security
        atiny_net_close(connP->net_context);
    }
	else
    {
		dtls_ssl_destroy(connP->net_context);
    }
}

void * lwm2m_connect_server(uint16_t secObjInstID, void * userData)
{
  client_data_t * dataP;
  lwm2m_list_t * instance;
  connection_t * newConnP = NULL;
  dataP = (client_data_t *)userData;
  lwm2m_object_t  * securityObj = dataP->securityObjP;

  fprintf(stderr, "Now come into Connection creation in lwm2m_connect_server.\n");
  
  instance = LWM2M_LIST_FIND(dataP->securityObjP->instanceList, secObjInstID);
  if (instance == NULL)
  {
      return NULL;
  }


  newConnP = connection_create(dataP->connList, securityObj, instance->id, dataP->lwm2mH);
  if (newConnP == NULL)
  {
      fprintf(stderr, "Connection creation failed.\n");     
      return NULL;
  }

  fprintf(stderr, "Connection creation successfully in lwm2m_connect_server.\n");
  
  dataP->connList = newConnP;
  return (void *)newConnP;
}

void lwm2m_close_connection(void * sessionH, void * userData)
{
    client_data_t * app_data;
    connection_t * targetP;

    app_data = (client_data_t *)userData;
    targetP = (connection_t *)sessionH;

    if (targetP == app_data->connList)
    {
        app_data->connList = targetP->next;
		connection_free(targetP);
        lwm2m_free(targetP);
    }
    else
    {
        connection_t * parentP;

        parentP = app_data->connList;
        while (parentP != NULL && parentP->next != targetP)
        {
            parentP = parentP->next;
        }
        if (parentP != NULL)
        {
            parentP->next = targetP->next;
			connection_free(targetP);
            lwm2m_free(targetP);
        }
    }
}


int lwm2m_buffer_recv(void * sessionH, uint8_t * buffer, size_t length, uint32_t timeout)
{
    connection_t * connP = (connection_t*) sessionH;
    timeout*=1000;

    if (connP->dtls_flag == false) {
        // no security
        return atiny_net_recv_timeout(connP->net_context, buffer, length, timeout);
    } 
    else 
    {
        return dtls_read(connP->net_context, buffer, length, timeout);
        
    }
}

uint8_t lwm2m_buffer_send(void * sessionH,
                          uint8_t * buffer,
                          size_t length,
                          void * userdata)
{
    connection_t * connP = (connection_t*) sessionH;

    if (connP == NULL)
    {
        fprintf(stderr, "#> failed sending %lu bytes, missing connection\r\n",(unsigned long)length);
        return COAP_500_INTERNAL_SERVER_ERROR ;
    }

    printf("call connection_send in lwm2m_buffer_send, length is %d\n",length);
    
    if (connP->dtls_flag == false) {
        // no security
        return atiny_net_send(connP->net_context, buffer, length);
    } 
    else 
    {
        return dtls_write(connP->net_context, buffer, length);
        
    }
/*
    if (-1 == connection_send(connP, buffer, length))
    {
        fprintf(stderr, "#> failed sending %lu bytes\r\n", (unsigned long)length);
        return COAP_500_INTERNAL_SERVER_ERROR ;
    }
*/
    //return COAP_NO_ERROR;
}

bool lwm2m_session_is_equal(void * session1,
                            void * session2,
                            void * userData)
{
    return (session1 == session2);
}

