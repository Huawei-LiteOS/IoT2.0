#ifndef AGENT_TINY_H
#define AGENT_TINY_H
#ifdef __cplusplus
extern "C" {
#endif

#include <stdarg.h>
#include <stdio.h>
#include "liblwm2m.h"


/***************************************************** 以下接口由用户实现，agent调用 ****************************************************************************************/

/*****************************************************************************
 Function :    atiny_log  
 Description : agent_tiny 日志打印接口，该接口由用户侧实现
 Input      
 Output      : 
 Return      : 
 *****************************************************************************/
 void atiny_log(const char* fmt, ...);

typedef enum
{
   ATINY_GET_BINDING_MODES,
   ATINY_GET_MODEL_NUMBER,
   ATINY_DO_DEV_REBOOT,
   ATINY_GET_MIN_VOLTAGE,
   ATINY_GET_MAX_VOLTAGE,
   ATINY_GET_SERIAL_NUMBER,  
   ATINY_GET_BATERRY_LEVEL,
   ATINY_GET_MEMORY_FREE,
   ATINY_GET_DEV_ERR,
   //ATINY_GET_CURRENT_TIME,
   //ATINY_GET_UTC_OFFSET,//从云端获取，与time_offset是否存在区别
   //ATINY_GET_TIMEZONE,
   ATINY_GET_POWER_CURRENT_1,
   ATINY_GET_POWER_CURRENT_2,
   ATINY_GET_POWER_SOURCE_1,
   ATINY_GET_POWER_SOURCE_2,
   ATINY_DO_FACTORY_RESET,
   ATINY_GET_FIRMWARE_VER,
   ATINY_TRIG_FIRMWARE_UPDATE,
   ATINY_GET_FIRMWARE_STATE,
   ATINY_GET_FIRMWARE_RESULT,
   ATINY_GET_NETWORK_BEARER,
   ATINY_GET_SIGNAL_STRENGTH,
   ATINY_GET_CELL_ID,
   ATINY_GET_LINK_QUALITY,
   ATINY_GET_LINK_UTILIZATION,
   ATINY_WRITE_APP_DATA,
   ATINY_UPDATE_PSK,
}atiny_cmd_e;
/*****************************************************************************
 Function :    atiny_cmd_ioctl  
 Description : agent_tiny命令下发，该接口由用户侧实现
 Input       : cmd       参考atiny_cmd_e枚举体
               arg       命令字参数缓冲区，其内存空间由agent_tiny负责申请，命令字为ATINY_DO_DEV_REBOOT、ATINY_WRITE_APP_DATA，ATINY_UDATE_PSK时，
                         arg为入参，内容由agent_tiny填充。
 Output      : arg       命令字涉及GET时，arg作出参，其内存空间由agent_tiny负责申请，内容由设备侧填充
 Return      : 参考错误码枚举,arg指向的内存空间由用户负责check，如buf不够，应返回ATINY_BUF_NOT_ENOUGH
 *****************************************************************************/
int atiny_cmd_ioctl(atiny_cmd_e cmd, char* arg, int len);



/********************************************************** 以下接口由agent_tiny实现，供用户调用***********************************************************************************/
typedef struct
{
    char* binding;               /*目前支持U或者UQ*/
    int   life_time;             /*必选，默认50000,如过短，则频繁发送update报文，如过长，在线状态更新时间长*/
    bool  storing;               /*false/true*/
    unsigned int  storing_cnt;   /*storing为true时，lwm2m缓存区总字节个数*/     
} atiny_server_param_t;

typedef struct
{
    bool  is_bootstrap;                 /*false或true*/
    char* server_ip;                    /*服务器ip，字符格式*/
    char* server_port;                  /*服务器端口，字符格式*/
    char* psk_Id;
    char* psk;
    unsigned short psk_len;
} atiny_security_param_t;

typedef enum
{
    FIRMWARE_UPDATE_STATE = 0,
    APP_DATA
} atiny_report_type_e;


typedef struct
{
    atiny_server_param_t             server_params;
    atiny_security_param_t           security_params[2];/*需要支持两个，一个bootstrap，一个lwm2mserver*/      
} atiny_param_t;


typedef struct
{
    char* endpoint_name;
    char* manufacturer;
    char* dev_type;
} atiny_device_info_t;

/*****************************************************************************
 Function :    atiny_init  
 Description : lwm2m初始化，由agent_tiny实现
 Input       : atiny_params    lwm2m基本配置参数
               phandle         出参，返回表征当前agent_tiny上下文的句柄
 Output      : None
 Return      : 
 *****************************************************************************/
int  atiny_init(atiny_param_t* atiny_params, void ** phandle);

/*****************************************************************************
 Function :    atiny_bind  
 Description : lwm2m主函数体，由agent_tiny实现
 Input       : device_info     epname等数据
               phandle         atiny_init返回的phandle
 Output      : None
 Return      : 如lwm2m启动成功，该函数进入大循环体，不返回。如失败，返回以下错误码:
 *****************************************************************************/
int atiny_bind(atiny_device_info_t* device_info,void* phandle);

/*****************************************************************************
 Function :    atiny_deinit  
 Description : agent_tiny去初始化,由agent_tiny实现
 Input       : phandle   agent_tiny句柄
 Output      : None
 Return      : 
 *****************************************************************************/
void atiny_deinit(void* phandle);

#define MAX_REPORT_DATA_LEN      1024  /*待斟酌*/
#define MAX_BUFFER_REPORT_CNT 8
typedef enum
{
    NOT_SENT = 0,
    SENT_WAIT_RESPONSE,
    SENT_FAIL,
    SENT_TIME_OUT,
    SENT_SUCCESS,
    SENT_GET_RST,
    SEND_PENDING,
    OBSERVE_CANCEL
}DATA_SEND_STATUS;

typedef void (*atiny_ack_callback) (atiny_report_type_e type, int cookie, DATA_SEND_STATUS status);
typedef struct _data_report_t
{ 
    atiny_report_type_e type;     /*数据上报类型*/
    int cookie;                   /*数据cookie,用以在ack回调中，区分不同的数据*/
    int len;                      /*数据长度，不应大于MAX_REPORT_DATA_LEN*/
    uint8_t *buf;                 /*数据缓冲区首地址*/
    atiny_ack_callback callback;  /*ack回调*/
}data_report_t; 
/*****************************************************************************
 Function :    atiny_data_report
 Description : 主动数据上报,由agent_tiny实现
 Input       : phandle  atiny handle
               report_data   数据体
 Output      : None
 Return      : ATINY_OK  报文投递协议栈成功
               ATINY_MSG_CONGEST  lwm2m协议栈拥塞，待发送报文个数大于atiny_server_param_t中storing_cnt字段的个数时，返回拥塞
               ATINY_BUF_OVERFLOW pbuf过长
 *****************************************************************************/
int atiny_data_report(void* phandle, data_report_t *report_data);


typedef enum
{
    LOG_FATAL = 0,
    LOG_ERR,
    LOG_WARNING,
    LOG_INFO,
    LOG_DEBUG
} atiny_log_e;
void atiny_set_log_level(atiny_log_e level);

atiny_log_e atiny_get_log_level(void);


/*agent_tiny错误码*/
typedef enum 
{
  ATINY_OK = 0,
  ATINY_ARG_INVALID = -1,
  ATINY_BUF_OVERFLOW = -2,
  ATINY_MSG_CONGEST         = -3,
  ATINY_MALLOC_FAILED       = -4,
  ATINY_RESOURCE_NOT_FOUND  = -5,
  ATINY_RESOURCE_NOT_ENOUGH      = -6,
  ATINY_CLIENT_UNREGISTERED = -7, 
  ATINY_SOCKET_CREATE_FAILED = -8,
}atiny_error_e;


#ifdef __cplusplus
}
#endif

#endif
