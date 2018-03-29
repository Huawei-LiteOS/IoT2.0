#include <cpptest.h>
#include <iostream>
#include <fstream>
#include <memory>
#include "agenttiny.h"
#include "adapter_layer.h"
#include "atiny_log.h"
#include "object_comm.h"
#include "test_object_firmware.h"

/* testcase for read 5/0 in object_app.c 
*/



  void TestObjectFirmware::test_func1(){
      int result;
      lwm2m_uri_t uri = {.flag = 0x07, .objectId = 5, .instanceId = 0, .resourceId = 0};
      int len = 1; 
	  lwm2m_data_t *data;
	  lwm2m_object_t * testObj = NULL;
	  atiny_param_t * atiny_pa = NULL;
	
	  testObj = get_object_firmware(atiny_pa);
	  TEST_ASSERT(testObj->readFunc != NULL);
	
	  lwm2m_list_t * list = lwm2m_list_find(testObj->instanceList, uri.instanceId);
	  TEST_ASSERT(list != NULL);
		
	  data = lwm2m_data_new(1);
      uri.resourceId = 5;
	  data->id = uri.resourceId;
	  result = testObj->readFunc(uri.instanceId, &len, &data, NULL, testObj);
	  lwm2m_data_free(1,data);
	  TEST_ASSERT_EQUALS_MSG(result, COAP_205_CONTENT, result);
		
	  data = lwm2m_data_new(1);
      uri.resourceId = 0;
	  data->id = uri.resourceId;
	  result = testObj->readFunc(uri.instanceId, &len, &data, NULL, testObj);
	  lwm2m_data_free(1,data);
	  TEST_ASSERT_EQUALS_MSG(result, COAP_405_METHOD_NOT_ALLOWED, result);
		
	  data = lwm2m_data_new(1);
      uri.resourceId = 8;
	  data->id = uri.resourceId;
	  result = testObj->readFunc(uri.instanceId, &len, &data, NULL, testObj);
	  lwm2m_data_free(1,data);
	  TEST_ASSERT_EQUALS_MSG(result, COAP_404_NOT_FOUND, result);
	
  }
  void TestObjectFirmware::test_func2(){
	  int result;
      lwm2m_uri_t uri = {.flag = 0x07, .objectId = 5, .instanceId = 0, .resourceId = 0};
      int len = 0; 
	  uint8_t * buffer = NULL;
	  lwm2m_object_t * testObj = NULL;
	  atiny_param_t * atiny_pa = NULL;
	  
	  testObj = get_object_firmware(atiny_pa);
	  TEST_ASSERT("testObj->executeFunc != NULL");
	  ((firmware_data_t * )(testObj->userData))->state = 1;
	  
	  lwm2m_list_t * list = lwm2m_list_find(testObj->instanceList, uri.instanceId);
	  TEST_ASSERT("list != NULL");

      uri.resourceId = 2;
	  result = testObj->executeFunc(uri.instanceId, uri.resourceId, buffer, len, testObj);
	  TEST_ASSERT_EQUALS_MSG(result, COAP_204_CHANGED, result);
		
	  uri.resourceId = 0;
	  result = testObj->executeFunc(uri.instanceId, uri.resourceId, buffer, len, testObj);
	  TEST_ASSERT_EQUALS_MSG(result, COAP_405_METHOD_NOT_ALLOWED, result);
  }


  TestObjectFirmware::TestObjectFirmware(){
    TEST_ADD(TestObjectFirmware::test_func1);
    TEST_ADD(TestObjectFirmware::test_func2);
  }

  void TestObjectFirmware::setup(){
    std::cout<<"in steup\n";
  }

  void TestObjectFirmware::tear_down(){
    std::cout<<"in teardown\n";
  }


