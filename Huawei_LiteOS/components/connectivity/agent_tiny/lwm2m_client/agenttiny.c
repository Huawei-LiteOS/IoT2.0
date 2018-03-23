#include "liblwm2m.h"
#include "internals.h"
#include "agenttiny.h"
#include "object_comm.h"
#include "dtls_conn.h"
#include "atiny_log.h"
#include "atiny_rpt.h"


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
    
int  atiny_init_objects(atiny_param_t* atiny_params, const atiny_device_info_t* device_info, handle_data_t *handle)
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

    result = atiny_init_rpt();
    if (result != ATINY_OK)
    {
        ATINY_LOG(LOG_FATAL, "atiny_init_rpt fail,ret=%d", result);
        return result;
    }
    
    pdata = &handle->client_data;
    memset(pdata, 0, sizeof(client_data_t));

    ATINY_LOG(LOG_INFO, "Trying to init objects");
    
    
    lwm2m_context = lwm2m_init(pdata);
    if (NULL == lwm2m_context)
    {   
        lwm2m_free(pdata);
        return ATINY_MALLOC_FAILED;
    } 
    
    #if defined WITH_DTLS
    pdata->lwm2mH = lwm2m_context;
    #endif

    handle->lwm2m_context = lwm2m_context;

	if(atiny_params->security_params[0].psk != NULL)
	{
		snprintf(serverUri, SERVER_URI_MAX_LEN, "coaps://%s:%s", 
		atiny_params->security_params[0].server_ip, atiny_params->security_params[0].server_port); 		
	}
	else
	{
		snprintf(serverUri, SERVER_URI_MAX_LEN, "coap://%s:%s", 
		atiny_params->security_params[0].server_ip, atiny_params->security_params[0].server_port); 
	}

    handle->obj_array[OBJ_SECURITY_INDEX] = get_security_object(serverId, serverUri,
    atiny_params->security_params[0].psk_Id, atiny_params->security_params[0].psk, 
    atiny_params->security_params[0].psk_len, false);
    if (NULL ==  handle->obj_array[OBJ_SECURITY_INDEX])
    {
        ATINY_LOG(LOG_FATAL, "Failed to create security object");
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
        ATINY_LOG(LOG_FATAL, "Failed to create device object");
        return ATINY_MALLOC_FAILED;
    }
    
    handle->obj_array[OBJ_FIRMWARE_INDEX] = get_object_firmware(atiny_params);
    if (NULL == handle->obj_array[OBJ_FIRMWARE_INDEX])
    {
        ATINY_LOG(LOG_FATAL, "Failed to create firmware object");
        return ATINY_MALLOC_FAILED;
    }
    
    handle->obj_array[OBJ_CONNECT_INDEX] = get_object_conn_m(atiny_params);
    if (NULL == handle->obj_array[OBJ_CONNECT_INDEX])
    {
        ATINY_LOG(LOG_FATAL, "Failed to create connect object");
        return ATINY_MALLOC_FAILED;
    }
        
    handle->obj_array[OBJ_APP_INDEX] = get_platform_object(atiny_params);
    if (NULL == handle->obj_array[OBJ_APP_INDEX])
    {
        ATINY_LOG(LOG_FATAL, "Failed to create app object");
        return ATINY_MALLOC_FAILED;
    }
        
    result = lwm2m_configure(lwm2m_context, epname, NULL, NULL, OBJ_MAX_NUM, handle->obj_array);
    if (result != 0)
    {
        return ATINY_RESOURCE_NOT_FOUND;
    }
    
    return ATINY_OK;
}

static int lwm2m_poll(lwm2m_context_t* contextP, uint32_t* timeout)
{
    client_data_t* dataP;  
    uint8_t buffer[MAX_PACKET_SIZE] = {0};
    int numBytes;
    connection_t* connP;

    dataP = (client_data_t*)(contextP->userData);   
    connP = dataP->connList;
    
	while (connP != NULL)
    {
		numBytes = lwm2m_buffer_recv(connP, buffer, MAX_PACKET_SIZE, *timeout);
		if(numBytes <= 0)
		{
			ATINY_LOG(LOG_INFO, "no packet arrived!");
		}
		else
		{
			lwm2m_handle_packet(contextP, buffer, numBytes, connP);
		}
	    connP = connP->next;
	}

    return ATINY_OK;
}

void atiny_detroy(void* handle)
{
    handle_data_t *handle_data = (handle_data_t *)handle;

    atiny_destory_rpt();
     
    if(handle_data == NULL)
    {
        return;  
    }
   

    if(handle_data->lwm2m_context != NULL)
    {
        lwm2m_close(handle_data->lwm2m_context); 
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
   
    if(NULL == uriP)
    {
         ATINY_LOG(LOG_ERR, "uriP null");
         return;
    }

    atiny_clear_rpt_data(uriP, OBSERVE_CANCEL);
    ATINY_LOG(LOG_INFO, "remove all rpt data by observe cancel");
}

int atiny_bind(atiny_device_info_t* device_info,void* phandle)
{
    handle_data_t *handle = phandle;    
    uint32_t timeout;
    int ret;

    if((NULL == device_info) || (NULL == phandle))
    {
        ATINY_LOG(LOG_FATAL, "Parameter null");
        return ATINY_ARG_INVALID;
    }

    
    if(NULL == device_info->endpoint_name)
    {
        ATINY_LOG(LOG_FATAL, "Endpoint name null");
        return ATINY_ARG_INVALID; 
    }

    if(NULL == device_info->manufacturer)
    {
        ATINY_LOG(LOG_FATAL, "Manufacturer name null");
        return ATINY_ARG_INVALID; 
    }
    
   
    ret = atiny_init_objects(&handle->atiny_params, device_info, handle);
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
        timeout = 10;

        (void)atiny_step_rpt(handle->lwm2m_context);
        lwm2m_step(handle->lwm2m_context,&timeout); 
        lwm2m_poll(handle->lwm2m_context,&timeout);
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
    lwm2m_context_t* atiny_context;
    int ret;
    data_report_t data;

    handle_data_t *handle = (handle_data_t *)phandle;
   
    if (NULL == phandle || NULL == report_data || report_data->len <= 0 
        || report_data->len > MAX_REPORT_DATA_LEN || (NULL == handle->lwm2m_context))
    {
        ATINY_LOG(LOG_ERR, "invalid args");
        return ATINY_ARG_INVALID;
    }

    
    atiny_context = (lwm2m_context_t*)handle->lwm2m_context;
    if (atiny_context->state != STATE_READY)
    {

       ATINY_LOG(LOG_ERR, "unrregister");
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

    memcpy(&data, report_data, sizeof(data));
    data.buf = lwm2m_malloc(report_data->len);
    if (NULL == data.buf)
    {
        ATINY_LOG(LOG_ERR, "lwm2m_malloc fail,len %d",data.len);
        return ATINY_MALLOC_FAILED;;
    }
    memcpy(data.buf, report_data->buf, report_data->len);

    ret = atiny_queue_rpt_data(&uri, &data);

    if(ATINY_OK != ret)
    {
        if(data.buf != NULL)
        {
            lwm2m_free(data.buf);
        }
    }

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

