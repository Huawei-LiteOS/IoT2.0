#ifndef ATINY_RPT_H
#define ATINY_RPT_H
#include "agenttiny.h"
#include "agent_list.h"



#ifdef __cplusplus
extern "C" {
#endif



#define URI_FORMAT "uri(flag:0x%x,objId:%d,instId:%d,resId:%d)"
#define URI_LOG_PARAM(uri) (uri)->flag,(uri)->objectId,(uri)->instanceId,(uri)->resourceId


ATINY_INLINE void get_instance_uri(uint16_t object_id, uint16_t instance_id, lwm2m_uri_t *uri)
{
    uri->flag = LWM2M_URI_FLAG_OBJECT_ID | LWM2M_URI_FLAG_INSTANCE_ID;
    uri->objectId = object_id;
    uri->instanceId = instance_id;
    uri->resourceId = 0;    
}
ATINY_INLINE void get_resource_uri(uint16_t object_id, uint16_t instance_id, uint16_t resource_id, lwm2m_uri_t *uri)
{
    uri->flag = LWM2M_URI_FLAG_OBJECT_ID | LWM2M_URI_FLAG_INSTANCE_ID | LWM2M_URI_FLAG_RESOURCE_ID;
    uri->objectId = object_id;
    uri->instanceId = instance_id;
    uri->resourceId = resource_id;    
}

struct _atiny_rpt_list_t;
typedef struct _atiny_rpt_list_t * rpt_list_t;

int atiny_init_rpt(void);
int atiny_add_rpt_uri(const lwm2m_uri_t *uri,  rpt_list_t *list);
int atiny_rm_rpt_uri(const lwm2m_uri_t *uri);


int atiny_dequeue_rpt_data(rpt_list_t rpt_list,  data_report_t *data);
int atiny_queue_rpt_data(const lwm2m_uri_t *uri, const data_report_t *data);
int atiny_clear_rpt_data(const lwm2m_uri_t *uri, int result);

int atiny_step_rpt(lwm2m_context_t * context);
void atiny_destory_rpt(void);




#ifdef __cplusplus
}
#endif

#endif
