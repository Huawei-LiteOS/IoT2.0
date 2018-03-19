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
#define OBJ_PLATFORM_IDX 5
#define OBJ_COUNT 6
lwm2m_object_t* ATobjArray[OBJ_COUNT];
#define MAX_PACKET_SIZE 1024

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
    dtls_conn_t  *connList;/*???¡ã???¡ì3?¨°?????¦Ì?¦Ì??¡¤*/
    lwm2m_context_t * lwm2mH;
#else
    connection_t * connList;
#endif
    int addressFamily;
} client_data_t;


typedef struct 
{
    lwm2m_context_t * lwm2m_context;
    atiny_param_t atiny_params;
    client_data_t client_data;
    int atiny_quit;
}handle_data_t;


extern void observe_handleAck(lwm2m_transaction_t * transacP, void * message);


int  atiny_init(atiny_param_t* atiny_params, void ** phandle)
{
     handle_data_t *handle = NULL;

     if (NULL == atiny_params || NULL == phandle)
     {
	    ATINY_LOG(LOG_FATAL, "invalid args");
        return ATINY_ARG_INVALID;
     }

     handle = lwm2m_malloc(sizeof(client_data_t));
     if(NULL == handle )
     {
        ATINY_LOG(LOG_FATAL, "lwm2m_malloc fail");
        return ATINY_MALLOC_FAILED;
     }

     memset(handle, 0,  sizeof(handle));

     handle->atiny_params = *atiny_params;
     *phandle = handle;
     return ATINY_OK;     
}

int  atiny_init_object(atiny_param_t* atiny_params, const atiny_device_info_t* device_info, handle_data_t *handle)
{
    int result;
    client_data_t *pdata;
    lwm2m_context_t* lwm2m_context = NULL;
    char serverUri[SERVER_URI_MAX_LEN];
    int serverId = 123;
    atiny_security_param_t *server_object_param = atiny_params->security_params;
    const char * localPort = server_object_param->server_port;
    char * epname = (char *)device_info->endpoint_name;

        /*init objects*/
    if(atiny_params == NULL)
    {
        return ATINY_ARG_INVALID;
    }
    
    pdata = &handle->client_data;
    memset(pdata, 0, sizeof(client_data_t));

    pdata->addressFamily = AF_INET;

    ATINY_LOG(LOG_INFO, "Trying to bind LWM2M Client to port %s", localPort);
    
    #ifndef WITH_MBEDTLS    
    pdata->sock = create_socket(localPort, pdata->addressFamily);
    if (pdata->sock < 0)
    {
        ATINY_LOG(LOG_INFO, "Failed to open socket: %d %s\r\n", errno, strerror(errno));
        return -1;
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

    ATobjArray[0] = get_security_object(serverId, serverUri,
    atiny_params->security_params[0].psk_Id, atiny_params->security_params[0].psk, 
    atiny_params->security_params[0].psk_len, false);
    if (NULL == ATobjArray[0])
    {
        ATINY_LOG(LOG_FATAL, "Failed to create server object");
        return ATINY_MALLOC_FAILED;
    }

    pdata->securityObjP = ATobjArray[0];

    ATobjArray[1] = get_server_object(serverId, atiny_params->server_params.binding,
    atiny_params->server_params.life_time, atiny_params->server_params.storing);
    if(NULL == ATobjArray[1])
    {
        ATINY_LOG(LOG_FATAL, "Failed to create server object");
        return ATINY_MALLOC_FAILED;
    }

    ATobjArray[2] = get_object_device(atiny_params, device_info->manufacturer);
    if(NULL == ATobjArray[2])
    {   
        ATINY_LOG(LOG_FATAL, "Failed to create server object");
        return ATINY_MALLOC_FAILED;
    }
    
    ATobjArray[3] = get_object_firmware(atiny_params);
    if (NULL == ATobjArray[3])
    {
        ATINY_LOG(LOG_FATAL, "Failed to create server object");
        return ATINY_MALLOC_FAILED;
    }
    
    ATobjArray[4] = get_object_conn_m(atiny_params);
    if (NULL == ATobjArray[4])
    {
        ATINY_LOG(LOG_FATAL, "Failed to create server object");
        return ATINY_MALLOC_FAILED;
    }
        
    ATobjArray[5] = get_platform_object(atiny_params);
    if (NULL == ATobjArray[OBJ_PLATFORM_IDX])
    {
        ATINY_LOG(LOG_FATAL, "Failed to create server object");
        return ATINY_MALLOC_FAILED;
    }
        
    result = lwm2m_configure(lwm2m_context, epname, NULL, NULL, OBJ_COUNT, ATobjArray);
    if (result != 0)
    {
        return ATINY_RESOURCE_NOT_FOUND;
    }

    
    observe_register_ack_call_back(observe_handleAck);
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
     
    if(handle_data)
    {
        lwm2m_close(handle_data->lwm2m_context);
    }

   
    clean_security_object(ATobjArray[0]);
    lwm2m_free(ATobjArray[0]);
    clean_server_object(ATobjArray[1]);
    lwm2m_free(ATobjArray[1]);
    free_object_device(ATobjArray[2]);
    free_object_firmware(ATobjArray[3]);
    free_object_conn_m(ATobjArray[4]);
    free_platform_object(ATobjArray[5]);
    if(handle_data)
    {
        lwm2m_free(handle_data);
    }

     /*close socket*/
}

static inline int atiny_check_change_to_disconnect(lwm2m_client_state_t old_state, lwm2m_client_state_t new_state)
{
    return ((STATE_READY == old_state) 
            && (STATE_BOOTSTRAP_REQUIRED == new_state));
}

static void atiny_observe_cancel_notify(const lwm2m_context_t * contextP, const lwm2m_uri_t * uriP)
{
    if(NULL == uriP)
    {
         ATINY_LOG(LOG_FATAL, "uriP null");
         return;
    }

    if(NULL == ATobjArray[OBJ_PLATFORM_IDX])
    {
        return;
    }

    if(uriP->objectId == ATobjArray[OBJ_PLATFORM_IDX]->objID)
    {
        free_platform_object_rpt_list(ATobjArray[OBJ_PLATFORM_IDX]);
        ATINY_LOG(LOG_INFO, "remove all rpt data by observe cancel");
    }
}

int atiny_bind(atiny_device_info_t* device_info,void* phandle)
{
    handle_data_t *handle = phandle;    
    lwm2m_context_t * lwm2mP = NULL;
    struct timeval tv;
    int ret;

    if((NULL == device_info) || (NULL == phandle))
    {
        ATINY_LOG(LOG_FATAL, "phandle null");
        return ATINY_ARG_INVALID;
    }

    
    if(NULL == device_info->endpoint_name)
    {
        //ATINY_LOG_INFO("device_info->endpoint_name null");
        return ATINY_ARG_INVALID; 
    }

    if(NULL == device_info->manufacturer)
    {
        ATINY_LOG(LOG_ERR, "device_info->manufacturer null");
        return ATINY_ARG_INVALID; 
    }
    
   
    ret = atiny_init_object(&handle->atiny_params, device_info, handle);
    if(ret != ATINY_OK)
    {
        ATINY_LOG(LOG_FATAL, "atiny_init_object fail %d", ret);
        return ret;
    }

    lwm2m_reg_observe_cancel_notify(atiny_observe_cancel_notify);

    
    lwm2mP = handle->lwm2m_context;
    if(NULL == lwm2mP)
    {
        ATINY_LOG(LOG_FATAL, "lwm2m_context null");
        return ATINY_ARG_INVALID;
    }
    


    while(!handle->atiny_quit)
    {                
        tv.tv_sec = 10;
        tv.tv_usec = 0;        
        time_t sec = (time_t)tv.tv_sec;    
        lwm2m_client_state_t old_state = lwm2mP->state;
        lwm2m_step(lwm2mP,&sec);          
        atiny_recv(lwm2mP,&tv); 
        if(atiny_check_change_to_disconnect(old_state, lwm2mP->state))
        {
            ATINY_LOG(LOG_INFO, "change to disconnect");
            free_platform_object_rpt_list(ATobjArray[OBJ_PLATFORM_IDX]);
        }
    }

    atiny_detroy(phandle);
 
    return ATINY_OK;
}



void atiny_deinit(void* handle)
{
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
#if 0
    if(lwm2m_resource_value_changed(atiny_context,&uri) != URI_OBSERVED)
    {
        ATINY_LOG(LOG_ERR, "%d not observe", report_data->type);
        return ATINY_ARG_INVALID;
    }
#endif
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

