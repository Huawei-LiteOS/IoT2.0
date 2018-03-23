#ifndef AGENT_TINY_H
#define AGENT_TINY_H
#ifdef __cplusplus
extern "C" {
#endif

#include <stdarg.h>
#include <stdio.h>
#include "liblwm2m.h"


/***************************************************** ���½ӿ����û�ʵ�֣�agent���� ****************************************************************************************/

/*****************************************************************************
 Function :    atiny_log  
 Description : agent_tiny ��־��ӡ�ӿڣ��ýӿ����û���ʵ��
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
   //ATINY_GET_UTC_OFFSET,//���ƶ˻�ȡ����time_offset�Ƿ��������
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
 Description : agent_tiny�����·����ýӿ����û���ʵ��
 Input       : cmd       �ο�atiny_cmd_eö����
               arg       �����ֲ��������������ڴ�ռ���agent_tiny�������룬������ΪATINY_DO_DEV_REBOOT��ATINY_WRITE_APP_DATA��ATINY_UDATE_PSKʱ��
                         argΪ��Σ�������agent_tiny��䡣
 Output      : arg       �������漰GETʱ��arg�����Σ����ڴ�ռ���agent_tiny�������룬�������豸�����
 Return      : �ο�������ö��,argָ����ڴ�ռ����û�����check����buf������Ӧ����ATINY_BUF_NOT_ENOUGH
 *****************************************************************************/
int atiny_cmd_ioctl(atiny_cmd_e cmd, char* arg, int len);



/********************************************************** ���½ӿ���agent_tinyʵ�֣����û�����***********************************************************************************/
typedef struct
{
    char* binding;               /*Ŀǰ֧��U����UQ*/
    int   life_time;             /*��ѡ��Ĭ��50000,����̣���Ƶ������update���ģ������������״̬����ʱ�䳤*/
    bool  storing;               /*false/true*/
    unsigned int  storing_cnt;   /*storingΪtrueʱ��lwm2m���������ֽڸ���*/     
} atiny_server_param_t;

typedef struct
{
    bool  is_bootstrap;                 /*false��true*/
    char* server_ip;                    /*������ip���ַ���ʽ*/
    char* server_port;                  /*�������˿ڣ��ַ���ʽ*/
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
    atiny_security_param_t           security_params[2];/*��Ҫ֧��������һ��bootstrap��һ��lwm2mserver*/      
} atiny_param_t;


typedef struct
{
    char* endpoint_name;
    char* manufacturer;
    char* dev_type;
} atiny_device_info_t;

/*****************************************************************************
 Function :    atiny_init  
 Description : lwm2m��ʼ������agent_tinyʵ��
 Input       : atiny_params    lwm2m�������ò���
               phandle         ���Σ����ر�����ǰagent_tiny�����ĵľ��
 Output      : None
 Return      : 
 *****************************************************************************/
int  atiny_init(atiny_param_t* atiny_params, void ** phandle);

/*****************************************************************************
 Function :    atiny_bind  
 Description : lwm2m�������壬��agent_tinyʵ��
 Input       : device_info     epname������
               phandle         atiny_init���ص�phandle
 Output      : None
 Return      : ��lwm2m�����ɹ����ú��������ѭ���壬�����ء���ʧ�ܣ��������´�����:
 *****************************************************************************/
int atiny_bind(atiny_device_info_t* device_info,void* phandle);

/*****************************************************************************
 Function :    atiny_deinit  
 Description : agent_tinyȥ��ʼ��,��agent_tinyʵ��
 Input       : phandle   agent_tiny���
 Output      : None
 Return      : 
 *****************************************************************************/
void atiny_deinit(void* phandle);

#define MAX_REPORT_DATA_LEN      1024  /*������*/
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
    atiny_report_type_e type;     /*�����ϱ�����*/
    int cookie;                   /*����cookie,������ack�ص��У����ֲ�ͬ������*/
    int len;                      /*���ݳ��ȣ���Ӧ����MAX_REPORT_DATA_LEN*/
    uint8_t *buf;                 /*���ݻ������׵�ַ*/
    atiny_ack_callback callback;  /*ack�ص�*/
}data_report_t; 
/*****************************************************************************
 Function :    atiny_data_report
 Description : ���������ϱ�,��agent_tinyʵ��
 Input       : phandle  atiny handle
               report_data   ������
 Output      : None
 Return      : ATINY_OK  ����Ͷ��Э��ջ�ɹ�
               ATINY_MSG_CONGEST  lwm2mЭ��ջӵ���������ͱ��ĸ�������atiny_server_param_t��storing_cnt�ֶεĸ���ʱ������ӵ��
               ATINY_BUF_OVERFLOW pbuf����
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


/*agent_tiny������*/
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
