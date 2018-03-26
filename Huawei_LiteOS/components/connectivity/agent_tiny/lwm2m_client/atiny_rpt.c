#include <string.h>
#include <stdio.h>
#include "liblwm2m.h"
#include "internals.h"
#include "atiny_rpt.h"
#include "agenttiny.h"
#include "agent_list.h"
#include "atiny_log.h"
#include "atiny_adapter.h"


#define URI_FORMAT "uri(flag:0x%x,objId:%d,instId:%d,resId:%d)"
#define URI_LOG_PARAM(uri) (uri)->flag,(uri)->objectId,(uri)->instanceId,(uri)->resourceId

typedef struct _atiny_rpt_list_t
{
    atiny_dl_list node;
    lwm2m_uri_t uri;
    atiny_dl_list rpt_list;
    int rpt_node_cnt;
}atiny_rpt_list_t;

typedef struct
{ 
    atiny_dl_list list;
    data_report_t data;
}atiny_rpt_node_t;



static atiny_dl_list g_atiny_rpt_table;
static void *g_mutex = NULL;

//TODO: when free,recorrect then callback code

static int atiny_is_uri_equal(const lwm2m_uri_t * uri0, const lwm2m_uri_t * uri1)
{
    if(LWM2M_URI_IS_SET_RESOURCE(uri0) && LWM2M_URI_IS_SET_RESOURCE(uri1))
    {
        
        return (uri0->objectId == uri1->objectId) && (uri0->instanceId == uri1->instanceId)
            && (uri0->resourceId == uri1->resourceId);
    }

    return (uri0->objectId == uri1->objectId) && (uri0->instanceId == uri1->instanceId);
}



static atiny_rpt_list_t * atiny_find_rpt_list(const lwm2m_uri_t * uri)
{
    atiny_dl_list *item;
    atiny_dl_list *next;
        
    ATINY_DL_LIST_FOR_EACH_SAFE(item, next, &g_atiny_rpt_table)
    {
        atiny_rpt_list_t *data_node = (atiny_rpt_list_t *)item;
        if(atiny_is_uri_equal(&data_node->uri, uri))
        {
            return data_node;
        }
    }
    return NULL;
}

static void atiny_free_list(atiny_dl_list *list,  void(*free_data)(atiny_dl_list *node, void* param),  void *param)
{
    atiny_dl_list *item;
    atiny_dl_list *next;
        
    ATINY_DL_LIST_FOR_EACH_SAFE(item, next, list)
    {
        atiny_list_delete(item);
        if(free_data != NULL)
        {
            free_data(item, param);        
        }
    }
}

static void atiny_visit_list(atiny_dl_list *list,  void(*visit_data)(atiny_dl_list *node, void *param), void *param)
{
    atiny_dl_list *item;
    atiny_dl_list *next;
        
    ATINY_DL_LIST_FOR_EACH_SAFE(item, next, list)
    {
        visit_data(item, param);
    }
}

static void atiny_clear_rpt_list_node_data(atiny_dl_list * node,  void *result)
{
    atiny_rpt_node_t *rpt_node = (atiny_rpt_node_t *)node;

    if(rpt_node->data.callback)
    {
        rpt_node->data.callback(rpt_node->data.type, 
            rpt_node->data.cookie, (DATA_SEND_STATUS)(long)result);
    }
    
    if(rpt_node->data.buf)
    {
        lwm2m_free(rpt_node->data.buf);
        rpt_node->data.buf = NULL;
    }
}


static void atiny_clear_rpt_list(atiny_dl_list * node, void * result)
{
    atiny_rpt_list_t *rpt_list = (atiny_rpt_list_t *)node;
    atiny_free_list(&rpt_list->rpt_list, atiny_clear_rpt_list_node_data, result);
}


static void atiny_notify_stack_rpt_data_change(atiny_dl_list * node, void * context)
{
    atiny_rpt_list_t *rpt_list = (atiny_rpt_list_t *)node;
    if(!atiny_list_empty(&rpt_list->rpt_list))
    {
        ATINY_LOG(LOG_INFO, "data change cnt %d "URI_FORMAT,  rpt_list->rpt_node_cnt, URI_LOG_PARAM(&rpt_list->uri));
        lwm2m_resource_value_changed(context, &rpt_list->uri);
    }
}


int atiny_init_rpt(void)
{
    atiny_list_init(&g_atiny_rpt_table);
    g_mutex = atiny_mutex_create();
    if(NULL == g_mutex)
    {
        ATINY_LOG(LOG_ERR, "atiny_mutex_create fail");
        return ATINY_RESOURCE_NOT_ENOUGH;;
    }
    return ATINY_OK;
}


int atiny_add_rpt_uri(const lwm2m_uri_t *uri,  rpt_list_t *list)
{
    atiny_rpt_list_t *rpt_list  = NULL;
    int ret = ATINY_ARG_INVALID;

    if (NULL == uri)
    {
        ATINY_LOG(LOG_ERR, "null point");
        return ATINY_ARG_INVALID;
    }

    if (NULL == list)
    {
        ATINY_LOG(LOG_ERR, "null point");
        return ATINY_ARG_INVALID;
    }

    *list = NULL;

    atiny_mutex_lock(g_mutex);    
    do
    {
        rpt_list = atiny_find_rpt_list(uri);
        if(rpt_list != NULL)
        {
           ATINY_LOG(LOG_ERR, "uri exist,"URI_FORMAT, URI_LOG_PARAM(uri));
           break;
        }

        rpt_list = lwm2m_malloc(sizeof(*rpt_list));
        if(NULL == rpt_list)
        {
           ATINY_LOG(LOG_ERR, "lwm2m_malloc fail,"URI_FORMAT, URI_LOG_PARAM(uri));
           break;
        }

        memset(rpt_list, 0, sizeof(*rpt_list)); 
        atiny_list_init(&rpt_list->rpt_list);
        memcpy(&rpt_list->uri, uri, sizeof(rpt_list->uri));
         
        atiny_list_insert_tail(&g_atiny_rpt_table, &rpt_list->node);
        ret = ATINY_OK;
        
    }while(0);
    atiny_mutex_unlock(g_mutex);

    *list = rpt_list;

    return ret;
}


int atiny_rm_rpt_uri(const lwm2m_uri_t *uri)
{
    
    int ret = ATINY_ARG_INVALID;

    if (NULL == uri)
    {
        ATINY_LOG(LOG_ERR, "null point");
        return ATINY_ARG_INVALID;
    }

    
    atiny_mutex_lock(g_mutex);
    do
    {
        atiny_rpt_list_t *rpt_list  = NULL;
        rpt_list = atiny_find_rpt_list(uri);        
        if(NULL == rpt_list)
        {
           ATINY_LOG(LOG_ERR, "uri not exit,"URI_FORMAT, URI_LOG_PARAM(uri));
           break;
        }
        
        atiny_list_delete(&rpt_list->node);

        atiny_free_list(&rpt_list->rpt_list, atiny_clear_rpt_list_node_data,  (void *)NOT_SENT);

        lwm2m_free(rpt_list);

        ret = ATINY_OK;
    }while(0);
    atiny_mutex_unlock(g_mutex);

    return ret;
}



int atiny_dequeue_rpt_data(rpt_list_t rpt_list,  data_report_t *data)
{
    int ret = ATINY_RESOURCE_NOT_FOUND;

    if (NULL == rpt_list
        || (NULL == data))
    {
        ATINY_LOG(LOG_ERR, "null point");
        return ATINY_ARG_INVALID;
    }
    
    atiny_mutex_lock(g_mutex);
    do
    {
        atiny_rpt_node_t *data_node;
        
        if(atiny_list_empty(&rpt_list->rpt_list))
        {
        
            ATINY_LOG(LOG_ERR, "dequeue empty rpt list");
            break;
        }

        data_node = (atiny_rpt_node_t *)rpt_list->rpt_list.next;
        atiny_list_delete(&data_node->list);
        rpt_list->rpt_node_cnt--;
        
        memcpy(data, &data_node->data, sizeof(*data));
        lwm2m_free(data_node);
        ret = ATINY_OK;
        
    }while(0);
    atiny_mutex_unlock(g_mutex);

    return ret;    
}
int atiny_queue_rpt_data(const lwm2m_uri_t *uri, const data_report_t *data)
{
    int ret = ATINY_MALLOC_FAILED;

    if ((NULL == uri) || (NULL == data))
    {
        ATINY_LOG(LOG_ERR, "null point");
        return ATINY_ARG_INVALID;
    }
    
    atiny_mutex_lock(g_mutex);
    do
    {
        atiny_rpt_list_t *rpt_list;
        atiny_rpt_node_t *data_node;
        
        rpt_list = atiny_find_rpt_list(uri);
        if(NULL == rpt_list)
        {
           ATINY_LOG(LOG_ERR, "uri rpt list not exit,"URI_FORMAT, URI_LOG_PARAM(uri));
           ret = ATINY_RESOURCE_NOT_FOUND;
           break;
        }

        if(rpt_list->rpt_node_cnt >= MAX_BUFFER_REPORT_CNT)
        {
           ATINY_LOG(LOG_ERR, "uri rpt exceed rpt cnt %d,"URI_FORMAT, 
                    rpt_list->rpt_node_cnt, URI_LOG_PARAM(uri));
           ret = ATINY_RESOURCE_NOT_ENOUGH;
           break;
        }
        
        data_node = lwm2m_malloc(sizeof(*data_node));
        if(NULL == data_node)
        {
           ATINY_LOG(LOG_ERR, "malloc fail,"URI_FORMAT, URI_LOG_PARAM(uri));
           break;
        }
        memset(&data_node->list, 0, sizeof(data_node->list));
        memcpy(&data_node->data, data, sizeof(data_node->data));
        atiny_list_insert_tail(&rpt_list->rpt_list, &data_node->list);
        rpt_list->rpt_node_cnt++;

        ret = ATINY_OK;
        
    }while(0);
    atiny_mutex_unlock(g_mutex);

    return ret;

}



int atiny_clear_rpt_data(const lwm2m_uri_t *uri, int result)
{
   int ret = ATINY_RESOURCE_NOT_FOUND;

   if (NULL == uri)
   {
       ATINY_LOG(LOG_ERR, "null point");
       return ATINY_ARG_INVALID;
   }

   
   atiny_mutex_lock(g_mutex);
   do
   {
       atiny_rpt_list_t *rpt_list;      
       
       rpt_list = atiny_find_rpt_list(uri);
       if(NULL == rpt_list)
       {
          ATINY_LOG(LOG_ERR, "uri rpt list not exit,"URI_FORMAT, URI_LOG_PARAM(uri));
          break;
       }

       atiny_free_list(&rpt_list->rpt_list, atiny_clear_rpt_list_node_data, (void *)result);
       rpt_list->rpt_node_cnt = 0;
	   ret = ATINY_OK;
    }while(0);
    
    atiny_mutex_unlock(g_mutex);

    return ret;
}



int atiny_step_rpt(lwm2m_context_t * context)
{
    atiny_mutex_lock(g_mutex);
    atiny_visit_list(&g_atiny_rpt_table, atiny_notify_stack_rpt_data_change, context);
    atiny_mutex_unlock(g_mutex);
    return ATINY_OK;
}


void atiny_destory_rpt(void)
{
    atiny_free_list(&g_atiny_rpt_table, atiny_clear_rpt_list, (void *)NOT_SENT);

    atiny_mutex_destroy(g_mutex);
}




