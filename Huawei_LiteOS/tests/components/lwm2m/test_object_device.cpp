
#include <cpptest.h>
#include <iostream>
#include <fstream>
#include <memory>
#include "agenttiny.h"
#include "adapter_layer.h"
#include "atiny_log.h"
#include "object_comm.h"
#include "test_object_device.h"

/* testcase for read 3/0 in object_app.c 
*/

    void TestObjectDevice::test_func1(){
      int result;
      int len = 1; 
	  lwm2m_uri_t uri = {.flag = 0x07, .objectId = 3, .instanceId = 0, .resourceId = 0};
	  lwm2m_data_t * data = NULL;
	  lwm2m_object_t * testObj = NULL;
	  atiny_param_t * atiny_pa = NULL;
	  const char* facturer = "uuuuu";
	
	  testObj = get_object_device(atiny_pa,facturer);
	  TEST_ASSERT(testObj->readFunc != NULL);
	 
	  lwm2m_list_t * list = lwm2m_list_find(testObj->instanceList, uri.instanceId);
	  TEST_ASSERT(list != NULL);
	
	  data = lwm2m_data_new(1);
	  uri.resourceId = 0;
	  data->id = uri.resourceId;
	  result = testObj->readFunc(uri.instanceId, &len, &data, NULL, testObj);
	  TEST_ASSERT_MSG(strncmp((const char*)(data->value.asBuffer.buffer),facturer,sizeof(facturer)) == 0,"read manufacture failed\r\n");
	  lwm2m_data_free(1,data);	
	  TEST_ASSERT_EQUALS_MSG(result, COAP_205_CONTENT, result);
	  
	  data = lwm2m_data_new(1);
	  uri.resourceId = 1;
	  data->id = uri.resourceId;
	  result = testObj->readFunc(uri.instanceId, &len, &data, NULL, testObj);
	  TEST_ASSERT_MSG(strcmp((const char*)(data->value.asBuffer.buffer),"Lightweight M2M Client") == 0,"read model number failed\r\n");	  
	  lwm2m_data_free(1,data);
	  TEST_ASSERT_EQUALS_MSG(result, COAP_205_CONTENT, result);
		
	  data = lwm2m_data_new(1);
	  uri.resourceId = 2;
	  data->id = uri.resourceId;
	  result = testObj->readFunc(uri.instanceId, &len, &data, NULL, testObj);
	  TEST_ASSERT_MSG(strncmp((const char*)(data->value.asBuffer.buffer),"345000123",strlen("345000123")) == 0,"read serial number failed\r\n");		  
	  lwm2m_data_free(1,data);
	  TEST_ASSERT_EQUALS_MSG(result, COAP_205_CONTENT, result);

	  data = lwm2m_data_new(1);
	  uri.resourceId = 3;
	  data->id = uri.resourceId;
	  result = testObj->readFunc(uri.instanceId, &len, &data, NULL, testObj);
	  TEST_ASSERT_MSG(strncmp((const char*)(data->value.asBuffer.buffer),"example_ver001",strlen("example_ver001")) == 0,"read firmware version failed\r\n");		  	  
	  lwm2m_data_free(1,data);
	  TEST_ASSERT_EQUALS_MSG(result, COAP_205_CONTENT, result);
		
	  data = lwm2m_data_new(1);
	  uri.resourceId = 6;
	  data->id = uri.resourceId;
	  result = testObj->readFunc(uri.instanceId, &len, &data, NULL, testObj);
	  TEST_ASSERT_MSG(data->value.asChildren.array->value.asInteger == 1,"read min power failed\r\n");		  	  
	  TEST_ASSERT_MSG((data->value.asChildren.array + 1)->value.asInteger == 5,"read max power failed\r\n");	
	  lwm2m_data_free(1,data);
	  TEST_ASSERT_EQUALS_MSG(result, COAP_205_CONTENT, result);
		
	  data = lwm2m_data_new(1);
	  uri.resourceId = 7;
	  data->id = uri.resourceId;
	  result = testObj->readFunc(uri.instanceId, &len, &data, NULL, testObj);
	  TEST_ASSERT_MSG(data->value.asChildren.array->value.asInteger == 3800,"read min voltage failed\r\n");		  	  
	  TEST_ASSERT_MSG((data->value.asChildren.array + 1)->value.asInteger == 5000,"read max voltage failed\r\n");		  	  
	  lwm2m_data_free(1,data);
	  TEST_ASSERT_EQUALS_MSG(result, COAP_205_CONTENT, result);
		
      data = lwm2m_data_new(1);
	  uri.resourceId = 8;
	  data->id = uri.resourceId;
	  result = testObj->readFunc(uri.instanceId, &len, &data, NULL, testObj);
	  TEST_ASSERT_MSG(data->value.asChildren.array->value.asInteger == 125,"read min current failed\r\n");		  	  
	  TEST_ASSERT_MSG((data->value.asChildren.array + 1)->value.asInteger == 900,"read max current failed\r\n");
	  lwm2m_data_free(1,data);
	  TEST_ASSERT_EQUALS_MSG(result, COAP_205_CONTENT, result);
	
	  data = lwm2m_data_new(1);
	  uri.resourceId = 9;
	  data->id = uri.resourceId;
	  result = testObj->readFunc(uri.instanceId, &len, &data, NULL, testObj);
	  TEST_ASSERT_MSG(data->value.asInteger == 5000,"read battery level failed\r\n");
	  lwm2m_data_free(1,data);
	  TEST_ASSERT_EQUALS_MSG(result, COAP_205_CONTENT, result);
		
	  data = lwm2m_data_new(1);
	  uri.resourceId = 10;
	  data->id = uri.resourceId;
	  result = testObj->readFunc(uri.instanceId, &len, &data, NULL, testObj);
	  TEST_ASSERT_MSG(data->value.asInteger == 5000,"read memory free failed\r\n");
	  lwm2m_data_free(1,data);
	  TEST_ASSERT_EQUALS_MSG(result, COAP_205_CONTENT, result);
		
	  data = lwm2m_data_new(1);
	  uri.resourceId = 11;
	  data->id = uri.resourceId;
	  result = testObj->readFunc(uri.instanceId, &len, &data, NULL, testObj);
	  TEST_ASSERT_MSG(data->value.asChildren.array->value.asInteger == 0,"read error code failed\r\n");
	  lwm2m_data_free(1,data);
	  TEST_ASSERT_EQUALS_MSG(result, COAP_205_CONTENT, result);
		
	  data = lwm2m_data_new(1);
	  uri.resourceId = 13;
	  data->id = uri.resourceId;
	  result = testObj->readFunc(uri.instanceId, &len, &data, NULL, testObj);
	  lwm2m_data_free(1,data);
	  TEST_ASSERT_EQUALS_MSG(result, COAP_205_CONTENT, result);
		
	  data = lwm2m_data_new(1);
	  uri.resourceId = 15;
	  data->id = uri.resourceId;
	  result = testObj->readFunc(uri.instanceId, &len, &data, NULL, testObj);
	  lwm2m_data_free(1,data);
	  TEST_ASSERT_EQUALS_MSG(result, COAP_205_CONTENT, result);
		
	  data = lwm2m_data_new(1);
	  uri.resourceId = 16;
	  data->id = uri.resourceId;
	  result = testObj->readFunc(uri.instanceId, &len, &data, NULL, testObj);
	  TEST_ASSERT_MSG(strncmp((const char*)(data->value.asBuffer.buffer), "UQS", strlen("UQS")) == 0,"read bind mode failed\r\n");		  
	  lwm2m_data_free(1,data);
	  TEST_ASSERT_EQUALS_MSG(result, COAP_205_CONTENT, result);

       free_object_device(testObj);
	  
      }
	  
	  void TestObjectDevice::test_func2(){
      int result;
      int len = 0; 
	  uint8_t * buffer = NULL;
	  lwm2m_uri_t uri = {.flag = 0x07, .objectId = 3, .instanceId = 0, .resourceId = 0};
	  lwm2m_object_t * testObj = NULL;
	  atiny_param_t * atiny_pa = NULL;
	  const char* facturer = "uuuuu";
	
	  testObj = get_object_device(atiny_pa,facturer);
	  TEST_ASSERT(testObj->executeFunc != NULL);
	
	  lwm2m_list_t * list = lwm2m_list_find(testObj->instanceList, uri.instanceId);
	  TEST_ASSERT(list != NULL);

	  uri.resourceId = 4;
	  result = testObj->executeFunc(uri.instanceId, uri.resourceId, buffer, len, testObj);
	  TEST_ASSERT_EQUALS_MSG(result, COAP_204_CHANGED, result);
		
      uri.resourceId = 5;
	  result = testObj->executeFunc(uri.instanceId, uri.resourceId, buffer, len, testObj);
	  TEST_ASSERT_EQUALS_MSG(result, COAP_204_CHANGED, result);
		
      uri.resourceId = 12;
	  result = testObj->executeFunc(uri.instanceId, uri.resourceId, buffer, len, testObj);
	  TEST_ASSERT_EQUALS_MSG(result, COAP_204_CHANGED, result);
		
      uri.resourceId = 0;
	  result = testObj->executeFunc(uri.instanceId, uri.resourceId, buffer, len, testObj);
	  TEST_ASSERT_EQUALS_MSG(result, COAP_405_METHOD_NOT_ALLOWED, result);
	  
	  free_object_device(testObj);
	  
      }


  TestObjectDevice::TestObjectDevice(){
    TEST_ADD(TestObjectDevice::test_func1);
	TEST_ADD(TestObjectDevice::test_func2);

  }

  void TestObjectDevice::setup(){
    std::cout<<"in steup\n";
  }

  void TestObjectDevice::tear_down(){
    std::cout<<"in teardown\n";
	
  }




