#include "liblwm2m.h"
#include "agenttiny.h"


#include "connection.h"


#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <lwip/sockets.h>
#include <lwip/netdb.h>
#include <lwip/errno.h>
#include <signal.h>
#include "internals.h"
#include "los_sys.h"

#include "los_base.h"
#include "los_config.h"
#include "los_typedef.h"
#include "los_hwi.h"
#include "los_task.ph"
#include "los_sem.h"
#include "los_event.h"
#include "los_memory.h"
#include "los_queue.ph"
#include "cmsis_os.h"
#include "agenttiny.h"
#include "atiny_adapter.h"

void atiny_log(const char* fmt, ...)
{
    atiny_printf(fmt);
}
typedef struct 
{
    atiny_device_info_t* device_info;
    void* phandle;
}atiny_task_para;

UINT32 g_TestTaskID;


void atiny_task(atiny_task_para *para)
{
    atiny_bind(para->device_info, para->phandle);
}

UINT32 atiny_create_task(atiny_task_para * para)
{
    UINT32 uwRet = LOS_OK;
    TSK_INIT_PARAM_S lwm2m_para;
    memset(&lwm2m_para, 0, sizeof(TSK_INIT_PARAM_S));
    lwm2m_para.pfnTaskEntry = (TSK_ENTRY_FUNC)atiny_task;
    lwm2m_para.pcName       =  "lwm2m";
    lwm2m_para.uwStackSize  =  0x8000;
    lwm2m_para.usTaskPrio   =  5;
    lwm2m_para.uwArg = (UINT32)para;
    uwRet = LOS_TaskCreate(&g_TestTaskID, &lwm2m_para);
    if(uwRet != LOS_OK)
    {
        printf("task create failed .\n");
        return LOS_NOK;
    }
        return uwRet;
}


static void* g_phandle = NULL;
static atiny_task_para para;
static atiny_device_info_t g_device_info;

static atiny_param_t g_atiny_params;



void ack_callback(atiny_report_type_e type, int cookie, DATA_SEND_STATUS status){
    printf("type:%d cookie:%d status:%d\n", type,cookie, status);
}

int atiny_test(void){
	uint8_t buf[16] = "gooooogle";
    data_report_t report_data;
    int ret;
    report_data.buf = buf;
    report_data.callback = ack_callback;
    report_data.cookie = 0;
    report_data.len = sizeof(buf);
    report_data.type = APP_DATA;
    int cnt = 0;
    while(1){
	if(g_phandle!=NULL && atiny_state_is_ready(g_phandle)){
        report_data.cookie = cnt;
        cnt++;
    	ret = atiny_data_report(g_phandle, &report_data);
        printf("report ret:%d\n",ret);
    }
    osDelay(250*8);
        }
}
UINT32 TskHandle;

UINT32 creat_report_task()
{
    UINT32 uwRet = LOS_OK;
    TSK_INIT_PARAM_S task_init_param;

    task_init_param.usTaskPrio = 1;
    task_init_param.pcName = "task2";
    task_init_param.pfnTaskEntry = (TSK_ENTRY_FUNC)atiny_test;
    task_init_param.uwStackSize = 0x1000;
    //task_init_param.uwArg = NULL;

    uwRet = LOS_TaskCreate(&TskHandle, &task_init_param);
    if(LOS_OK != uwRet)
    {
        return uwRet;
    }
    return uwRet;
        
}
#ifdef WITH_DTLS
char * g_endpoint_name = "11110006";
char g_psk_value[16] = {0xef,0xe8,0x18,0x45,0xa3,0x53,
    0xc1,0x3c,0x0c,0x89,0x92,0xb3,0x1d,0x6b,0x6a,0x96};
#else
char * g_endpoint_name = "22220006";
#endif
void lwm2m_main(int argc, char *argv[])
{
     atiny_device_info_t *device_info = &g_device_info;
	 atiny_param_t* atiny_params; 
     atiny_security_param_t  *security_param = NULL;
	
    if(NULL == device_info)
    {
        return;
    }
    //device_info->endpoint_name = "urn:imei:15700077089";
    //device_info->endpoint_name = "666004";
    //device_info->endpoint_name = "666003";
    device_info->endpoint_name = g_endpoint_name;
    device_info->manufacturer = "test";
    
    //void* phandle;
   
    atiny_params = &g_atiny_params;

    atiny_params->server_params.binding = "UQS";
    atiny_params->server_params.life_time = 20;
    atiny_params->server_params.storing = FALSE;
    atiny_params->server_params.storing_cnt = 0;
    
    security_param = &(atiny_params->security_params[0]);
    security_param->is_bootstrap = FALSE;
    security_param->server_ip = "139.159.209.89";
#ifdef WITH_DTLS
    security_param->server_port = "5684";
#else
    security_param->server_port = "5683";
#endif

#ifdef WITH_DTLS
    security_param->psk_Id = g_endpoint_name;
    security_param->psk = g_psk_value;
    security_param->psk_len = 16;
#else
    security_param->psk_Id      = NULL;
    security_param->psk         = NULL;
    security_param->psk_len     = 0;
#endif
#if 0   
    /*
      回调函数初始化
      */
    //..............................................
    atiny_params->device_reboot = device_reboot;
    atiny_params->get_device_voltage = atiny_test_get_device_voltage;
    atiny_params->get_firmware_version = atiny_test_get_firmware_version;
    atiny_params->firmware_update = atiny_test_firmware_update;
    atiny_params->get_firmware_update_state = atiny_test_get_firmware_update_state;
    atiny_params->get_firmware_update_result = atiny_test_get_firmware_update_result;
    atiny_params->get_network_bearer = atiny_test_get_network_bearer;
    atiny_params->get_singal_strength = atiny_test_get_singal_strength;

    atiny_params->get_cell_id = atiny_test_get_cell_id;

    atiny_params->app_write = atiny_test_app_write_callback;


     atiny_device_reboot_fn              device_reboot;
    atiny_get_device_voltage_fn         get_device_voltage;
    
    atiny_get_firmware_version_fn       get_firmware_version;
    atiny_firmware_update_fn            firmware_update;
    atiny_get_firmware_update_state_fn  get_firmware_update_state;  /*当前升级状态，支持主动上报*/
    atiny_get_firmware_update_result_fn get_firmware_update_result; /*下载和升级状态，支持主动上报*/
    
    atiny_get_network_bearer_fn  get_network_bearer;
    atiny_get_singal_strength_fn get_singal_strength;
    atiny_get_cell_id_fn        get_cell_id;

    atiny_app_write_callback app_write;
#endif
    //..............................................
    if(ATINY_OK != atiny_init(atiny_params, &g_phandle))
    {
        LOS_MemFree(m_aucSysMem0,atiny_params);
        return;
    }
    creat_report_task();
    //atiny_task_para para;
    para.device_info = device_info;
    para.phandle  = g_phandle;
    atiny_task(&para);

}
