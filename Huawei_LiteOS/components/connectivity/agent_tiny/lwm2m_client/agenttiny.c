#include "liblwm2m.h"
#include "agenttiny.h"
#include "object_comm.h"
#if defined (WITH_TINYDTLS)
#include "dtlsconnection.h"
#elif defined(WITH_MBEDTLS)
#include "dtls_conn.h"
#include "dtls_interface.h"
#else
#include "connection.h"
#endif
#include <lwip/sockets.h>
#include "internals.h"
#include "agenttiny.h"
#include "agent_list.h"
#include "pdu.h"
#include "cmsis_os.h"
#include "atiny_log.h"


#define SERVER_URI_MAX_LEN  64
#define MAX_PACKET_SIZE   1024
#define INVALID_SOCKET    (-1) 

enum 
{
    OBJ_SECURITY_INDEX = 0,
    OBJ_SERVER_INDEX,
    OBJ_DEVICE_INDEX,
    OBJ_FIRMWARE_INDEX,
    OBJ_CONNECT_INDEX,
    OBJ_APP_INDEX,
    OBJ_MAX_NUM,
};

typedef struct
{
    lwm2m_object_t * securityObjP;
    lwm2m_object_t * serverObject;
#ifndef WITH_MBEDTLS    
    int sock;
#endif
#if defined (WITH_TINYDTLS)
    dtls_connection_t * connList;
    lwm2m_context_t * lwm2mH;
#elif defined (WITH_MBEDTLS)
    dtls_conn_t  *connList;
    lwm2m_context_t * lwm2mH;
#else
    connection_t * connList;
#endif
    int addressFamily;
} client_data_t;


typedef struct 
{
    lwm2m_context_t * lwm2m_context;
    atiny_param_t     atiny_params;
    client_data_t     client_data;
    lwm2m_object_t*   obj_array[OBJ_MAX_NUM];
    int atiny_quit;
}handle_data_t;



extern void observe_handleAck(lwm2m_transaction_t * transacP, void * message);

static handle_data_t g_atiny_handle;

int  atiny_init(atiny_param_t* atiny_params, void ** phandle)
{
     if (NULL == atiny_params || NULL == phandle)
     {
	    ATINY_LOG(LOG_FATAL, "Invalid args");
        return ATINY_ARG_INVALID;
     }

     memset((void*)&g_atiny_handle, 0,  sizeof(handle_data_t));

     g_atiny_handle.atiny_params = *atiny_params;
     *phandle = &g_atiny_handle;
     return ATINY_OK;
}
    
int  atiny_init_object(atiny_param_t* atiny_params, const atiny_device_info_t* device_info, handle_data_t *handle)
{
    int result;
    client_data_t *pdata;
    lwm2m_context_t* lwm2m_context = NULL;
    char serverUri[SERVER_URI_MAX_LEN];
    int serverId = 123;
    char * epname; 
    atiny_security_param_t *security_param;
    const char * localPort;

        /*init objects*/
    if(atiny_params == NULL || device_info == NULL)
    {
        return ATINY_ARG_INVALID;
    }

    security_param = atiny_params->security_params;
    localPort = security_param->server_port;
    epname = (char *)device_info->endpoint_name;
 
    pdata = &handle->client_data;
    pdata->addressFamily = AF_INET;
    pdata->sock = INVALID_SOCKET;
    
    ATINY_LOG(LOG_INFO, "Trying to bind LWM2M Client to port %s", localPort);
    
    #ifndef WITH_MBEDTLS    
    pdata->sock = create_socket(localPort, pdata->addressFamily);
    if (pdata->sock < 0)
    {
        ATINY_LOG(LOG_INFO, "Failed to open socket: %d %s\r\n", errno, strerror(errno));
        return ATINY_SOCKET_CREATE_FAILED;
    }
    #endif 
    
    lwm2m_context = lwm2m_init(pdata);
    if (NULL == lwm2m_context)
    {   
        lwm2m_free(pdata);
        return ATINY_MALLOC_FAILED;
    } 
    
    #if defined (WITH_TINYDTLS) || defined(WITH_MBEDTLS)
    pdata->lwm2mH = lwm2m_context;
    #endif

    handle->lwm2m_context = lwm2m_context;
    #if defined (WITH_TINYDTLS) || defined(WITH_MBEDTLS)
    snprintf(serverUri, SERVER_URI_MAX_LEN, "coaps://%s:%s", 
    atiny_params->security_params[0].server_ip, atiny_params->security_params[0].server_port); 
    #else
    snprintf(serverUri, SERVER_URI_MAX_LEN, "coap://%s:%s", 
    atiny_params->security_params[0].server_ip, atiny_params->security_params[0].server_port); 
    #endif

    handle->obj_array[OBJ_SECURITY_INDEX] = get_security_object(serverId, serverUri,
    atiny_params->security_params[0].psk_Id, atiny_params->security_params[0].psk, 
    atiny_params->security_params[0].psk_len, false);
    if (NULL ==  handle->obj_array[OBJ_SECURITY_INDEX])
    {
        ATINY_LOG(LOG_FATAL, "Failed to create server object");
        return ATINY_MALLOC_FAILED;
    }
    pdata->securityObjP = handle->obj_array[OBJ_SECURITY_INDEX];

    handle->obj_array[OBJ_SERVER_INDEX] = get_server_object(serverId, atiny_params->server_params.binding,
    atiny_params->server_params.life_time, atiny_params->server_params.storing);
    if(NULL == handle->obj_array[OBJ_SERVER_INDEX])
    {
        ATINY_LOG(LOG_FATAL, "Failed to create server object");
        return ATINY_MALLOC_FAILED;
    }

    handle->obj_array[OBJ_DEVICE_INDEX] = get_object_device(atiny_params, device_info->manufacturer);
    if(NULL == handle->obj_array[OBJ_DEVICE_INDEX])
    {   
        ATINY_LOG(LOG_FATAL, "Failed to create server object");
        return ATINY_MALLOC_FAILED;
    }
    
    handle->obj_array[OBJ_FIRMWARE_INDEX] = get_object_firmware(atiny_params);
    if (NULL == handle->obj_array[OBJ_FIRMWARE_INDEX])
    {
        ATINY_LOG(LOG_FATAL, "Failed to create server object");
        return ATINY_MALLOC_FAILED;
    }
    
    handle->obj_array[OBJ_CONNECT_INDEX] = get_object_conn_m(atiny_params);
    if (NULL == handle->obj_array[OBJ_CONNECT_INDEX])
    {
        ATINY_LOG(LOG_FATAL, "Failed to create server object");
        return ATINY_MALLOC_FAILED;
    }
        
    handle->obj_array[OBJ_APP_INDEX] = get_platform_object(atiny_params);
    if (NULL == handle->obj_array[OBJ_APP_INDEX])
    {
        ATINY_LOG(LOG_FATAL, "Failed to create server object");
        return ATINY_MALLOC_FAILED;
    }
        
    result = lwm2m_configure(lwm2m_context, epname, NULL, NULL, OBJ_MAX_NUM, handle->obj_array);
    if (result != 0)
    {
        return ATINY_RESOURCE_NOT_FOUND;
    }
    
    return ATINY_OK;
}

static int atiny_recv(lwm2m_context_t * contextP, struct timeval * timeoutP)
{
    client_data_t* dataP;
    dataP = (client_data_t*)(contextP->userData);
        
    #ifndef WITH_MBEDTLS
	  int result = 0;
    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(dataP->sock, &readfds); 
    
    result = select(FD_SETSIZE, &readfds, NULL, NULL, timeoutP);
    if (result < 0)
    {
        if (errno != EINTR)
        {
            fprintf(stderr, "Error in select(): %d %s\r\n", errno, strerror(errno));
        }
    }
    else if (result > 0)
    {
        uint8_t buffer[MAX_PACKET_SIZE];
        int numBytes;  
        if (FD_ISSET(dataP->sock, &readfds))            
        {
            struct sockaddr_storage addr;
            socklen_t addrLen;

            addrLen = sizeof(addr);    
            numBytes = recvfrom(dataP->sock, buffer, MAX_PACKET_SIZE, 0, (struct sockaddr *)&addr, &addrLen);
            if (0 > numBytes)
            {
                fprintf(stderr, "Error in recvfrom(): %d %s\r\n", errno, strerror(errno));
            }
            else if (0 < numBytes)
            {
                char s[INET_ADDRSTRLEN];
                in_port_t port;

            #ifdef WITH_DTLS
                dtls_connection_t * connP;
            #else
                connection_t * connP;
            #endif

                struct sockaddr_in *saddr = (struct sockaddr_in *)&addr;
                inet_ntop(saddr->sin_family, &saddr->sin_addr, s, INET_ADDRSTRLEN);
                port = saddr->sin_port;
                fprintf(stderr, "%d bytes received from [%s]:%hu\r\n", numBytes, s, ntohs(port));
                          
                output_buffer(stderr, buffer, numBytes, 0);

                connP = connection_find(dataP->connList, &addr, addrLen);
                if (connP != NULL)
                {
          
                #ifdef WITH_DTLS
                    int result = connection_handle_packet(connP, buffer, numBytes);
                    if (0 != result)
                    {
                        printf("error handling message %d\n",result);
                    }
                #else
                    lwm2m_handle_packet(contextP, buffer, numBytes, connP);
                #endif
                }
                else
                {
                    fprintf(stderr, "received bytes ignored!\r\n");
                }
            }    
        }
    }
    #else
    dtls_conn_t *connP = dataP->connList;
    if(connP == NULL)
    {
        fprintf(stderr, "connP is NULL!\r\n");
        return ATINY_OK;;
    }
    int numBytes;
    uint8_t buffer[MAX_PACKET_SIZE];
    printf("connP->ssl is %p\n",connP->ssl);
    numBytes = dtls_read(connP->ssl,buffer, MAX_PACKET_SIZE);
    if(numBytes > 0)
        lwm2m_handle_packet(dataP->lwm2mH, buffer, numBytes, connP);
    #endif
    return ATINY_OK;
}

void atiny_detroy(void* handle)
{
    handle_data_t *handle_data = (handle_data_t *)handle;
     
    if(handle_data == NULL)
    {
        return;  
    }

    if(handle_data->lwm2m_context != NULL)
    {
        lwm2m_close(handle_data->lwm2m_context); 
    }

    if (INVALID_SOCKET != handle_data->client_data.sock)
    {
        close(handle_data->client_data.sock);
    }
    connection_free(handle_data->client_data.connList);
    
    if (handle_data->obj_array[OBJ_SECURITY_INDEX] != NULL)
    {
        clean_security_object(handle_data->obj_array[OBJ_SECURITY_INDEX]);
        lwm2m_free(handle_data->obj_array[OBJ_SECURITY_INDEX]);
    }
    
    if (handle_data->obj_array[OBJ_SERVER_INDEX] != NULL)
    {
        clean_server_object(handle_data->obj_array[OBJ_SERVER_INDEX]);
        lwm2m_free(handle_data->obj_array[OBJ_SERVER_INDEX]);
    }
    
    if (handle_data->obj_array[OBJ_DEVICE_INDEX] != NULL)
    {
         free_object_device(handle_data->obj_array[OBJ_DEVICE_INDEX]);
    }

    if (handle_data->obj_array[OBJ_FIRMWARE_INDEX] != NULL)
    {
        free_object_firmware(handle_data->obj_array[OBJ_FIRMWARE_INDEX]);
    }

    if (handle_data->obj_array[OBJ_CONNECT_INDEX] != NULL)
    {
        free_object_conn_m(handle_data->obj_array[OBJ_CONNECT_INDEX]);
    }

    if (handle_data->obj_array[OBJ_APP_INDEX] != NULL)
    {
        free_platform_object(handle_data->obj_array[OBJ_APP_INDEX]);
    }
     /*close socket*/
}


static void atiny_observe_cancel_notify(const lwm2m_context_t * contextP, const lwm2m_uri_t * uriP)
{
    lwm2m_object_t * targetP;
    
    if(NULL == uriP)
    {
         ATINY_LOG(LOG_FATAL, "uriP null");
         return;
    }

    LOG_URI(uriP);
    targetP = (lwm2m_object_t *)LWM2M_LIST_FIND(contextP->objectList, uriP->objectId);
 
    if(NULL != targetP)
    {
        free_platform_object_rpt_list(targetP);
        ATINY_LOG(LOG_INFO, "remove all rpt data by observe cancel");
    }
}

int atiny_bind(atiny_device_info_t* device_info,void* phandle)
{
    handle_data_t *handle = phandle;    
    struct timeval tv;
    int ret;

    if((NULL == device_info) || (NULL == phandle))
    {
        ATINY_LOG(LOG_FATAL, "Parameter null");
        return ATINY_ARG_INVALID;
    }

    
    if(NULL == device_info->endpoint_name)
    {
        ATINY_LOG(LOG_ERR, "Endpoint name null");
        return ATINY_ARG_INVALID; 
    }

    if(NULL == device_info->manufacturer)
    {
        ATINY_LOG(LOG_ERR, "Manufacturer name null");
        return ATINY_ARG_INVALID; 
    }
    
   
    ret = atiny_init_object(&handle->atiny_params, device_info, handle);
    if(ret != ATINY_OK)
    {
        ATINY_LOG(LOG_FATAL, "atiny_init_object fail %d", ret);
        atiny_detroy(handle);
        return ret;
    }

    lwm2m_reg_observe_cancel_notify(atiny_observe_cancel_notify);
    observe_register_ack_call_back(observe_handleAck);
    
    while(!handle->atiny_quit)
    {                
        tv.tv_sec = 10;
        tv.tv_usec = 0;
        time_t sec = (time_t)tv.tv_sec;
        lwm2m_step(handle->lwm2m_context,&sec); 
        atiny_recv(handle->lwm2m_context,&tv);
    }

    atiny_detroy(phandle);

    return ATINY_OK;
}



void atiny_deinit(void* handle)
{
    if (handle == NULL)
    {
        return;
    }
    
    handle_data_t* handle_data = (handle_data_t*)handle;
    handle_data->atiny_quit = 1;
}

int atiny_data_report(void* phandle, data_report_t *report_data)
{
    lwm2m_uri_t uri;
    data_node_t* data_node;   
    lwm2m_context_t* atiny_context;
    int ret;

    handle_data_t *handle = (handle_data_t *)phandle;
   
    if (NULL == phandle || NULL == report_data || report_data->len <= 0 
        || report_data->len > MAX_REPORT_DATA_LEN || (NULL == handle->lwm2m_context))
    {
        ATINY_LOG(LOG_FATAL, "invalid args");
        return ATINY_ARG_INVALID;
    }
    
    atiny_context = (lwm2m_context_t*)handle->lwm2m_context;
    if (atiny_context->state != STATE_READY)
    {

       ATINY_LOG(LOG_FATAL, "unrregister");
       return ATINY_CLIENT_UNREGISTERED;
    }

    memset((void*)&uri,0, sizeof(uri));
    
    switch (report_data->type) {
        case FIRMWARE_UPDATE_STATE:
            lwm2m_stringToUri("/5/0/3", 6, &uri);
            break;
        case APP_DATA:        
            lwm2m_stringToUri("/19/0/0", 7, &uri);
            break;
        default:
            return ATINY_RESOURCE_NOT_FOUND;
    }

    data_node = (data_node_t*)lwm2m_malloc(sizeof(data_node_t));
    if (NULL == data_node)
    {
        ATINY_LOG(LOG_ERR, "lwm2m_malloc fail");
        return ATINY_MALLOC_FAILED;
    }

    ret = ATINY_MALLOC_FAILED;
    do
    {
        memset((void*)data_node, 0, sizeof(data_node_t));
        data_node->data.cookie = report_data->cookie;
        data_node->data.callback = report_data->callback;
        data_node->data.type = report_data->type;
        data_node->data.len = report_data->len;

        data_node->data.buf = lwm2m_malloc(report_data->len);
        if (NULL == data_node->data.buf)
        {
            ATINY_LOG(LOG_ERR, "lwm2m_malloc fail,len %d", report_data->len);
            break;
        }
        
        memcpy(data_node->data.buf, report_data->buf, report_data->len);
        atiny_list_init(&(data_node->list)); 
        ret = object_change(atiny_context, &uri, (uint8_t *)data_node, sizeof(data_node_t)); /*´íÎó×ª»»*/
    }while(0);

    if(ATINY_OK != ret)
    {
        if(data_node->data.buf != NULL)
        {
            lwm2m_free(data_node->data.buf);
        }
        lwm2m_free(data_node);
    }
    lwm2m_resource_value_changed(atiny_context,&uri);
    return ret;
}

void observe_handleAck(lwm2m_transaction_t * transacP, void * message)
{    
    atiny_ack_callback ack_callback = (atiny_ack_callback)transacP->cfg.callback;
    if (transacP->ack_received)
    {
        ack_callback((atiny_report_type_e)(transacP->cfg.type), transacP->cfg.cookie, SENT_SUCCESS);
    }
    else if (transacP->retrans_counter > COAP_MAX_RETRANSMIT)
    {
        ack_callback((atiny_report_type_e)(transacP->cfg.type), transacP->cfg.cookie, SENT_TIME_OUT);
    }
}

