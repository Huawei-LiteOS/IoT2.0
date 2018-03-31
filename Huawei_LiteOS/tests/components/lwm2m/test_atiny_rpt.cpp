#include <cpptest.h>
#include <iostream>
#include <fstream>
#include <memory>
#include "agenttiny.h"
#include "adapter_layer.h"
#include "atiny_log.h"
#include "object_comm.h"
#include "test_atiny_rpt.h"
#include "agent_list.h"
#include "atiny_rpt.h"
#include <stdbool.h>


void TestAtinyRpt::test_atiny_init_rpt(){
    int result= 0;
	result = atiny_init_rpt();
	TEST_ASSERT_EQUALS_MSG(ATINY_RESOURCE_NOT_ENOUGH, result, "uri equal test failed\r\n");
}
void TestAtinyRpt::test_atiny_add_rpt_uri(){
	
	int result = 0;    
	lwm2m_uri_t uri;
	rpt_list_t * list = NULL;
	result = atiny_add_rpt_uri(&uri,list);
    TEST_ASSERT_EQUALS_MSG(ATINY_ARG_INVALID, result, "uri is null point\r\n");
	
	uri = {.flag = 0x07, .objectId = 19, .instanceId = 0, .resourceId = 0};
	result = atiny_add_rpt_uri(&uri,list);
    TEST_ASSERT_EQUALS_MSG(ATINY_ARG_INVALID, result, "list is null point\r\n");
	
	uri = {.flag = 0x07, .objectId = 19, .instanceId = 0, .resourceId = 0};
    list = (rpt_list_t *)lwm2m_malloc(sizeof(rpt_list_t));
    TEST_ASSERT_MSG(list != NULL, "malloc list failed\r\n");
	result = atiny_add_rpt_uri(&uri,list);
	TEST_ASSERT_EQUALS_MSG(ATINY_OK, result, "add rpt_uri failed\r\n");	
    lwm2m_free(list);	
	
	
}

void TestAtinyRpt::test_atiny_rm_rpt_uri(){

	int result = 0;	
	lwm2m_uri_t uri = {0};	
	rpt_list_t * list = NULL;
	
	result = atiny_init_rpt();
	TEST_ASSERT_EQUALS_MSG(ATINY_RESOURCE_NOT_ENOUGH, result, "uri equal test failed\r\n");
/*remove null uri*/	
	result = atiny_rm_rpt_uri(&uri);
	TEST_ASSERT_EQUALS_MSG(ATINY_ARG_INVALID, result, "uri is null\r\n");	
/*add /19/0/0 resourceId to list, then remove this resourceId*/	
	uri = {.flag = 0x07, .objectId = 19, .instanceId = 0, .resourceId = 0};
	list = (rpt_list_t *)lwm2m_malloc(sizeof(rpt_list_t));
    TEST_ASSERT_MSG(list != NULL, "malloc list failed\r\n");
	result = atiny_add_rpt_uri(&uri,list);
	TEST_ASSERT_EQUALS_MSG(ATINY_OK, result, "add rpt_uri failed\r\n");	
	result = atiny_rm_rpt_uri(&uri);
	lwm2m_free(list);
	TEST_ASSERT_EQUALS_MSG(ATINY_OK, result, "remove rpt_uri node failed\r\n");
/*remove a not existed resourceId*/	
	uri = {.flag = 0x07, .objectId = 19, .instanceId = 0, .resourceId = 1};
	result = atiny_rm_rpt_uri(&uri);
	TEST_ASSERT_EQUALS_MSG(ATINY_ARG_INVALID, result, "not exist rpt_uri node test failed\r\n");

}

void TestAtinyRpt::test_atiny_queue_rpt_data(){
	int result = 0;
	lwm2m_uri_t uri = {0};	
	data_report_t *data = NULL;	
    rpt_list_t * list = NULL;
/*queue null uri*/	
	result = atiny_init_rpt();
	TEST_ASSERT_EQUALS_MSG(ATINY_RESOURCE_NOT_ENOUGH, result, "uri equal test failed\r\n");
	result = atiny_queue_rpt_data(&uri, data);
	TEST_ASSERT_EQUALS_MSG(ATINY_ARG_INVALID, result, "null point");
		
/*first add /19/0/0 resourceId to list, then queue report data to uri's list of this list*/	
	uri = {.flag = 0x07, .objectId = 19, .instanceId = 0, .resourceId = 0};
	list = (rpt_list_t *)lwm2m_malloc(sizeof(rpt_list_t));
    TEST_ASSERT_MSG(list != NULL, "malloc list failed\r\n");
	
	result = atiny_add_rpt_uri(&uri,list);
	TEST_ASSERT_EQUALS_MSG(ATINY_OK, result, "add rpt_uri failed\r\n");	
	
	data = (data_report_t *)lwm2m_malloc(sizeof(data_report_t));
	TEST_ASSERT_MSG(data != NULL, "malloc failed\r\n");
	data->type = APP_DATA;
	data->cookie = 8;
	data->len = 1024;
	data->callback = NULL;
	data->buf = NULL;
    result = atiny_queue_rpt_data(&uri, data);
	TEST_ASSERT_EQUALS_MSG(ATINY_OK, result, "null point");
/*didn't add /19/0/1 to list, queue it to uri's list of this list */	
	uri = {.flag = 0x07, .objectId = 19, .instanceId = 0, .resourceId = 1};
	result = atiny_queue_rpt_data(&uri, data);
	TEST_ASSERT_EQUALS_MSG(ATINY_RESOURCE_NOT_FOUND, result, "null point");
	
	lwm2m_free(data);
	lwm2m_free(list);

}
void TestAtinyRpt::test_atiny_dequeue_rpt_data(){

	int result = 0;
	lwm2m_uri_t uri = {0};	
	rpt_list_t *list = NULL;
	data_report_t *data_queue = NULL;
	data_report_t *data_dequeue = NULL;

	list = (rpt_list_t *)lwm2m_malloc(sizeof(rpt_list_t));
    TEST_ASSERT_MSG(list != NULL, "malloc list failed\r\n");
	
	result = atiny_init_rpt();
	TEST_ASSERT_EQUALS_MSG(ATINY_RESOURCE_NOT_ENOUGH, result, "uri equal test failed\r\n");
	
/*dequeue null queue*/
    *list = {0};	
	result = atiny_dequeue_rpt_data(*list, data_dequeue);
	TEST_ASSERT_EQUALS_MSG(ATINY_ARG_INVALID, result, "null point");
	
/*add /19/0/0 to queue, then dequeue it*/	   
	uri = {.flag = 0x07, .objectId = 19, .instanceId = 0, .resourceId = 0};
	result = atiny_add_rpt_uri(&uri,list);
	TEST_ASSERT_EQUALS_MSG(ATINY_OK, result, "add rpt_uri failed\r\n");	
	
	data_queue = (data_report_t *)lwm2m_malloc(sizeof(data_report_t));
	TEST_ASSERT_MSG(data_queue != NULL, "malloc failed\r\n");
	
	data_queue->type = APP_DATA;
	data_queue->cookie = 8;
	data_queue->len = 1024;
	data_queue->callback = NULL;
	data_queue->buf = NULL;
    result = atiny_queue_rpt_data(&uri, data_queue);
	TEST_ASSERT_EQUALS_MSG(ATINY_OK, result, "null point");
	
	data_dequeue = (data_report_t *)lwm2m_malloc(sizeof(data_report_t));
	TEST_ASSERT_MSG(data_dequeue != NULL, "malloc failed\r\n");
	
	result = atiny_dequeue_rpt_data(*list, data_dequeue);
	TEST_ASSERT_EQUALS_MSG(ATINY_OK, result, "null point");
	lwm2m_free(data_queue);
	
/*dequeue didn't queue resourceId /19/0/1*/	
    uri = {.flag = 0x07, .objectId = 19, .instanceId = 0, .resourceId = 1};
	result = atiny_add_rpt_uri(&uri,list);
	TEST_ASSERT_EQUALS_MSG(ATINY_OK, result, "add rpt_uri failed\r\n");	
	
	data_dequeue = (data_report_t *)lwm2m_malloc(sizeof(data_report_t));
	TEST_ASSERT_MSG(data_dequeue != NULL, "malloc failed\r\n");
	
	result = atiny_dequeue_rpt_data(*list, data_dequeue);
	TEST_ASSERT_EQUALS_MSG(ATINY_RESOURCE_NOT_FOUND, result, "null point");	
	
	lwm2m_free(data_queue);
	lwm2m_free(list);
    
}
void TestAtinyRpt::test_atiny_clear_rpt_data(){
	int result = 0;
	lwm2m_uri_t *uri = NULL;	
    rpt_list_t *list = NULL;
    data_report_t *data_queue = NULL;
	
    list = (rpt_list_t *)lwm2m_malloc(sizeof(rpt_list_t));
    TEST_ASSERT_MSG(list != NULL, "malloc list failed\r\n");
	
	result = atiny_init_rpt();
	TEST_ASSERT_EQUALS_MSG(ATINY_RESOURCE_NOT_ENOUGH, result, "uri equal test failed\r\n");
	
/*clear a null uri*/	
	result = atiny_clear_rpt_data(uri, SENT_SUCCESS);
	TEST_ASSERT_EQUALS_MSG(ATINY_ARG_INVALID, result, "null point");	
	
/*add /19/0/0 to list and queue a data to this uri's list, this delete uri node */	

	uri = (lwm2m_uri_t *)lwm2m_malloc(sizeof(lwm2m_uri_t));
    TEST_ASSERT_MSG(uri != NULL, "malloc list failed\r\n");
   *uri = {.flag = 0x07, .objectId = 19, .instanceId = 0, .resourceId = 0};
	result = atiny_add_rpt_uri(uri,list);
	TEST_ASSERT_EQUALS_MSG(ATINY_OK, result, "add rpt_uri failed\r\n");	
	
	data_queue = (data_report_t *)lwm2m_malloc(sizeof(data_report_t));
	TEST_ASSERT_MSG(data_queue != NULL, "malloc failed\r\n");
	
	data_queue->type = APP_DATA;
	data_queue->cookie = 8;
	data_queue->len = 1024;
	data_queue->callback = NULL;
	data_queue->buf = NULL;
    result = atiny_queue_rpt_data(uri, data_queue);
	TEST_ASSERT_EQUALS_MSG(ATINY_OK, result, "null point");
	
	int *send_result = 0;
	result = atiny_clear_rpt_data(uri, (int)send_result);
    TEST_ASSERT_EQUALS_MSG(ATINY_OK, result, "uri equal test failed\r\n");
	

	*uri = {.flag = 0x07, .objectId = 19, .instanceId = 0, .resourceId = 1};
	result = atiny_clear_rpt_data(uri, SENT_SUCCESS);
    TEST_ASSERT_EQUALS_MSG(ATINY_RESOURCE_NOT_FOUND, result, "uri equal test failed\r\n");
	
	lwm2m_free(data_queue);
	lwm2m_free(uri);
    lwm2m_free(list);
	
}





  TestAtinyRpt::TestAtinyRpt(){
    TEST_ADD(TestAtinyRpt::test_atiny_init_rpt);
    TEST_ADD(TestAtinyRpt::test_atiny_add_rpt_uri);
	TEST_ADD(TestAtinyRpt::test_atiny_rm_rpt_uri);
	TEST_ADD(TestAtinyRpt::test_atiny_queue_rpt_data);
	TEST_ADD(TestAtinyRpt::test_atiny_dequeue_rpt_data);
	TEST_ADD(TestAtinyRpt::test_atiny_clear_rpt_data);

  }

  void TestAtinyRpt::setup(){
    std::cout<<"in steup\n";
  }

  void TestAtinyRpt::tear_down(){
    std::cout<<"in teardown\n";
  }
