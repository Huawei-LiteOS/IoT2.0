#include <cpptest.h>
#include <iostream>
#include <fstream>
#include <memory>
#include "agenttiny.h"
#include "adapter_layer.h"
#include "atiny_log.h"
#include "object_comm.h"
#include "test_object_connectivity_moni.h"

/* testcase for read 4/0 in object_connectivity_moni.c 
*/

  void TestObjectConnectivityMoni::test_prv_read(){
      int result;
    lwm2m_uri_t uri = {.flag = 0x07, .objectId = 4, .instanceId = 0, .resourceId = 0};
    int len = 1; 
	  lwm2m_data_t *data;
	  lwm2m_object_t * testObj = NULL;
	  atiny_param_t * atiny_pa = NULL;
	
	  testObj = get_object_conn_m(atiny_pa);
	  TEST_ASSERT(testObj->readFunc != NULL);
	
	  lwm2m_list_t * list = lwm2m_list_find(testObj->instanceList, uri.instanceId);
	  TEST_ASSERT(list != NULL);
		
	  data = lwm2m_data_new(1);
      uri.resourceId = 1;
	  data->id = uri.resourceId;
	  result = testObj->readFunc(uri.instanceId, &len, &data, NULL, testObj);
	  TEST_ASSERT_MSG(data->value.asChildren.array->value.asInteger == 5,"read network bearer failed\r\n");
	  lwm2m_data_free(1,data);
	  TEST_ASSERT_EQUALS_MSG(result, COAP_205_CONTENT, result);
		
      data = lwm2m_data_new(1);
      uri.resourceId = 2;
	  data->id = uri.resourceId;
	  result = testObj->readFunc(uri.instanceId, &len, &data, NULL, testObj);
	  TEST_ASSERT_MSG(data->value.asInteger == 90,"read redio signal strength failed\r\n");
	  lwm2m_data_free(1,data);
	  TEST_ASSERT_EQUALS_MSG(result, COAP_205_CONTENT, result);

      data = lwm2m_data_new(1);
      uri.resourceId = 3;
	  data->id = uri.resourceId;
	  result = testObj->readFunc(uri.instanceId, &len, &data, NULL, testObj);
	  TEST_ASSERT_MSG(data->value.asInteger == 98,"read link quality failed\r\n");
	  lwm2m_data_free(1,data);
	  TEST_ASSERT_EQUALS_MSG(result, COAP_205_CONTENT, result);
		
      data = lwm2m_data_new(1);
      uri.resourceId = 6;
	  data->id = uri.resourceId;
	  result = testObj->readFunc(uri.instanceId, &len, &data, NULL, testObj);
	  TEST_ASSERT_MSG(data->value.asInteger == 10,"read link utilization failed\r\n");
	  lwm2m_data_free(1,data);
	  TEST_ASSERT_EQUALS_MSG(result, COAP_205_CONTENT, result);
		
      data = lwm2m_data_new(1);
      uri.resourceId = 8;
	  data->id = uri.resourceId;
	  result = testObj->readFunc(uri.instanceId, &len, &data, NULL, testObj);
	  TEST_ASSERT_MSG(data->value.asInteger == 21103,"read cell id failed\r\n");
	  lwm2m_data_free(1,data);
	  TEST_ASSERT_EQUALS_MSG(result, COAP_205_CONTENT, result);
		
      data = lwm2m_data_new(1);
      uri.resourceId = 11;
	  data->id = uri.resourceId;
	  result = testObj->readFunc(uri.instanceId, &len, &data, NULL, testObj);
	  lwm2m_data_free(1,data);
	  TEST_ASSERT_EQUALS_MSG(result, COAP_404_NOT_FOUND, result);
	  
	  free_object_device(testObj);	
  }


  TestObjectConnectivityMoni::TestObjectConnectivityMoni(){
    TEST_ADD(TestObjectConnectivityMoni::test_prv_read);
  }

  void TestObjectConnectivityMoni::setup(){
    std::cout<<"in TestObjectConnectivityMoni steup\n";
  }

  void TestObjectConnectivityMoni::tear_down(){
    std::cout<<"in teardown\n";
  }




