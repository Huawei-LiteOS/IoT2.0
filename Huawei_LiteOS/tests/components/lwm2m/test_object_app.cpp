
#include <cpptest.h>
#include <iostream>
#include <fstream>
#include <memory>
#include "agenttiny.h"
#include "adapter_layer.h"
#include "atiny_log.h"
#include "object_comm.h"
#include "atiny_rpt.h"

atiny_param_t* atiny_params; 
atiny_security_param_t  *security_param = NULL;
static atiny_param_t g_atiny_params;
static void* g_phandle = NULL;
void ack_callback(atiny_report_type_e type, int cookie, DATA_SEND_STATUS status){
    printf("type:%d cookie:%d status:%d\n", type,cookie, status);
}
typedef struct 
{
    lwm2m_context_t * lwm2m_context;
    atiny_param_t     atiny_params;
    client_data_t     client_data;
    lwm2m_object_t*   obj_array[6];
    int atiny_quit;
}handle_data_t;
void param_init()
{
	atiny_params = &g_atiny_params;
	atiny_params->server_params.binding = (char*)"UQS";
    atiny_params->server_params.life_time = 50000;
    atiny_params->server_params.storing = FALSE;
    atiny_params->server_params.storing_cnt = 0;
    
    security_param = &(atiny_params->security_params[0]);
    security_param->is_bootstrap = FALSE;
    security_param->server_ip = (char*)"139.159.209.89";
    security_param->server_port = (char*)"5683";
    security_param->psk_Id = NULL;
    security_param->psk = NULL;
    security_param->psk_len = 0;
	
}

class myTest: public Test::Suite{
      void test_func1(){
      int result;
      int len = 1; 
	  lwm2m_uri_t uri = {.flag = 0x07, .objectId = 19, .instanceId = 0, .resourceId = 0};
	  lwm2m_data_t * data = NULL;
	  lwm2m_object_t * testObj = NULL;
	  atiny_param_t * atiny_pa = NULL;
	  const char* facturer = "uuuuu";
	  atiny_init_rpt();
	  testObj = get_platform_object(atiny_pa);
	  TEST_ASSERT_MSG(testObj != NULL, "test get_platform_object() failed");
	  TEST_ASSERT_MSG(testObj->readFunc != NULL, "testObj->readFunc != NULL failed");
	  TEST_ASSERT_MSG(testObj->instanceList != NULL, "testObj->instanceList != NULL failed");
		 
	  lwm2m_list_t * list = lwm2m_list_find(testObj->instanceList, uri.instanceId);
	  TEST_ASSERT_MSG(list != NULL, "Obj_app find instance failed");
//Obj_app->readFunc can only read 19/0/0,19/1/0,19/0,19/1
      //read 19/0/0
	  data = lwm2m_data_new(1);
	  uri.resourceId = 0;
	  data->id = uri.resourceId;
	  
	  result = testObj->readFunc(uri.instanceId, &len, &data, NULL, testObj);
	  TEST_ASSERT_MSG(result == COAP_205_CONTENT, "Obj_app read cgf param NULL /19/0/0 failed");
	  
	  lwm2m_data_cfg_t  cfg = {0, 0, NULL};
	  result = testObj->readFunc(uri.instanceId, &len, &data, &cfg, testObj);
	  TEST_ASSERT_MSG(result == COAP_404_NOT_FOUND, "Obj_app read no rpt queue /19/0/0 failed");
	  lwm2m_data_free(1,data);
	  
	  //use queue rpt  
	  uint8_t buf[16] = "gooooogle";
      data_report_t report_data;
      int ret;
      report_data.buf = buf;
      report_data.callback = ack_callback;
      report_data.cookie = 0;
      report_data.len = sizeof(buf);
      report_data.type = APP_DATA;  
	  param_init();
	  ret = atiny_init(atiny_params, &g_phandle);//init g_handle
	  client_data_t *pdata = (client_data_t *)malloc(sizeof(client_data_t));
	  memset(pdata, 0, sizeof(client_data_t));   
      lwm2m_context_t * lwm2m_context = lwm2m_init(pdata);
	  lwm2m_context->state = STATE_READY;
	  ((handle_data_t*)g_phandle)->lwm2m_context = lwm2m_context;
	  atiny_data_report(g_phandle, &report_data);//report
	  data = lwm2m_data_new(1);
	  result = testObj->readFunc(uri.instanceId, &len, &data, &cfg, testObj);
	  TEST_ASSERT_MSG(result == COAP_205_CONTENT, "Obj_app read rpt queue /19/0/0 failed");
	  uint8_t * read_data = data->value.asBuffer.buffer;
	  printf("read_data=%s\n",read_data);
      ret = strncmp((char *)buf, (char *)read_data, sizeof(buf));
	  TEST_ASSERT_MSG(ret == 0, "Obj_app read data content correct failed");

	  //read 19/0/1
	  data = lwm2m_data_new(1);
	  uri.resourceId = 1;
	  data->id = uri.resourceId;
	  result = testObj->readFunc(uri.instanceId, &len, &data, NULL, testObj);
	  lwm2m_data_free(1,data);
	  TEST_ASSERT_MSG(result == COAP_404_NOT_FOUND, "Obj_app read /19/0/1 failed");
	  //read 19/1/0
	  data = lwm2m_data_new(1);
	  uri.instanceId = 1;
	  uri.resourceId = 0;
	  data->id = uri.resourceId;
	  result = testObj->readFunc(uri.instanceId, &len, &data, NULL, testObj);
	  lwm2m_data_free(1,data);
	  TEST_ASSERT_MSG(result == COAP_205_CONTENT, "Obj_app read /19/1/0 failed");
      //read 19/1/1
	  data = lwm2m_data_new(1);
	  uri.resourceId = 1;
	  data->id = uri.resourceId;
	  result = testObj->readFunc(uri.instanceId, &len, &data, NULL, testObj);
	  lwm2m_data_free(1,data);
	  TEST_ASSERT_MSG(result == COAP_404_NOT_FOUND, "Obj_app read /19/1/1 failed");
	  //read 19/2/0
	  data = lwm2m_data_new(1);
	  uri.instanceId = 2;
	  uri.resourceId = 0;
	  data->id = uri.resourceId;
	  result = testObj->readFunc(uri.instanceId, &len, &data, NULL, testObj);
	  lwm2m_data_free(1,data);
	  TEST_ASSERT_MSG(result == COAP_404_NOT_FOUND, "Obj_app read /19/2/0 failed");
	  //read 19/2/1
	  data = lwm2m_data_new(1);
	  uri.instanceId = 2;
	  uri.resourceId = 1;
	  data->id = uri.resourceId;
	  result = testObj->readFunc(uri.instanceId, &len, &data, NULL, testObj);
	  lwm2m_data_free(1,data);
	  TEST_ASSERT_MSG(result == COAP_404_NOT_FOUND, "Obj_app read /19/2/1 failed");
	  //read 19/3/0	 
      data = lwm2m_data_new(1);
	  uri.instanceId = 3;
	  uri.resourceId = 0;
	  data->id = uri.resourceId;
	  result = testObj->readFunc(uri.instanceId, &len, &data, NULL, testObj);
	  lwm2m_data_free(1,data);
	  TEST_ASSERT_MSG(result == COAP_404_NOT_FOUND, "Obj_app read /19/3/0 failed");
	  //read 19/0
	  uri = {.flag = 0x06, .objectId = 19, .instanceId = 0};
	  data = NULL;
	  len = 0;
	  result = testObj->readFunc(uri.instanceId, &len, &data, NULL, testObj);
	  lwm2m_data_free(1,data);
	  TEST_ASSERT_MSG(result == COAP_205_CONTENT, "Obj_app read /19/0 failed");
      //read 19/1
	  uri.instanceId = 1;
	  data = NULL;
	  len = 0;
	  result = testObj->readFunc(uri.instanceId, &len, &data, NULL, testObj);
	  lwm2m_data_free(1,data);
	  TEST_ASSERT_MSG(result == COAP_205_CONTENT, "Obj_app read /19/1 failed");
	  
      free_platform_object(testObj);
  
      };
	  
	  void test_func2(){
      int result;
      int len = 0; 
	  uint8_t * buffer = NULL;
	  lwm2m_uri_t uri = {.flag = 0x07, .objectId = 19, .instanceId = 0, .resourceId = 0};
	  lwm2m_object_t * testObj = NULL;
	  atiny_param_t * atiny_pa = NULL;
	  atiny_init_rpt();
	  testObj = get_platform_object(atiny_pa);
	  TEST_ASSERT_MSG(testObj->writeFunc != NULL, "Obj_app->writeFunc != NULL failed");
	
	  lwm2m_list_t * list = lwm2m_list_find(testObj->instanceList, uri.instanceId);
	  TEST_ASSERT_MSG(list != NULL, "Obj_app find instance failed");
      
	  char value[] = "testobjappwrite";
	  lwm2m_data_t * dataP;
	  //write /19/0/0
	  dataP = lwm2m_data_new(1);
	  lwm2m_data_encode_nstring(value, strlen(value), dataP);
	  result = testObj->writeFunc(uri.instanceId, 1, dataP, testObj);
	  TEST_ASSERT_MSG(result == COAP_204_CHANGED, "Obj_app write /19/0/0 failed");
	  
	  //write /19/1/0
	  uri.instanceId = 1;
	  result = testObj->writeFunc(uri.instanceId, 1, dataP, testObj);
	  TEST_ASSERT_MSG(result == COAP_204_CHANGED, "Obj_app write /19/1/0 failed");
	  
	  //write /19/0/1
	  uri.instanceId = 0;
	  uri.resourceId = 1;
	  dataP->id = uri.resourceId;
	  result = testObj->writeFunc(uri.instanceId, 1, dataP, testObj);
	  TEST_ASSERT_MSG(result == COAP_404_NOT_FOUND, "Obj_app write /19/0/1 failed");
	  
	  //write /19/3/0
	  uri.instanceId = 3;
	  uri.resourceId = 0;
	  dataP->id = uri.resourceId;
	  result = testObj->writeFunc(uri.instanceId, 1, dataP, testObj);
	  TEST_ASSERT_MSG(result == COAP_404_NOT_FOUND, "Obj_app write /19/0/1 failed");
	  
	  lwm2m_data_free(1,dataP);
	  free_object_device(testObj);
	  
      };
      void test_func3(){
      int result;
	  lwm2m_uri_t uri = {.flag = 0x07, .objectId = 19, .instanceId = 0, .resourceId = 0};
	  lwm2m_object_t * testObj = NULL;
	  atiny_param_t * atiny_pa = NULL;
      atiny_init_rpt();
	  
	  testObj = get_platform_object(atiny_pa);
	  TEST_ASSERT_MSG(testObj->executeFunc != NULL, "Obj_app->executeFunc != NULL failed");
	  TEST_ASSERT_MSG(testObj->instanceList != NULL, "testObj->instanceList != NULL failed");
	  lwm2m_list_t * list = lwm2m_list_find(testObj->instanceList, 1);
	  TEST_ASSERT_MSG(list != NULL, "Obj_app find instance failed");
      
	  uint8_t buffer[] = "testobjappexecutefunc";
	  //write /19/0/0
	  result = testObj->executeFunc(uri.instanceId, uri.resourceId, buffer, sizeof(buffer), testObj);
	  TEST_ASSERT_MSG(result == COAP_204_CHANGED, "Obj_app execute /19/0/0 failed");
	  //write /19/0/1
	  uri.resourceId = 1;
	  result = testObj->executeFunc(uri.instanceId, uri.resourceId, buffer, sizeof(buffer), testObj);
	  TEST_ASSERT_MSG(result == COAP_405_METHOD_NOT_ALLOWED, "Obj_app execute /19/0/1 failed");
	  //write /19/1/0
	  uri.instanceId = 1;
	  uri.resourceId = 0;
	  result = testObj->executeFunc(uri.instanceId, uri.resourceId, buffer, sizeof(buffer), testObj);
	  TEST_ASSERT_MSG(result == COAP_204_CHANGED, "Obj_app execute /19/1/0 failed");
	  //write /19/1/2
	  uri.resourceId = 2;
	  result = testObj->executeFunc(uri.instanceId, uri.resourceId, buffer, sizeof(buffer), testObj);
	  TEST_ASSERT_MSG(result == COAP_204_CHANGED, "Obj_app execute /19/1/2 failed");
	  //write /19/1/3
	  uri.resourceId = 3;
	  result = testObj->executeFunc(uri.instanceId, uri.resourceId, buffer, sizeof(buffer), testObj);
	  TEST_ASSERT_MSG(result == COAP_405_METHOD_NOT_ALLOWED, "Obj_app execute /19/1/3 failed");
	  //write /19/1/4
	  uri.resourceId = 4;
	  result = testObj->executeFunc(uri.instanceId, uri.resourceId, buffer, sizeof(buffer), testObj);
	  TEST_ASSERT_MSG(result == COAP_404_NOT_FOUND, "Obj_app execute /19/1/4 failed");
	  };

public:
  myTest(){
    TEST_ADD(myTest::test_func1);
	TEST_ADD(myTest::test_func2);
	TEST_ADD(myTest::test_func3);
  }

  protected:
  void setup(){
    std::cout<<"in steup\n";
  }

  void tear_down(){
    std::cout<<"in teardown\n";
	
  }
};

int main()
{
  Test::Suite ts;
  ts.add(std::auto_ptr<Test::Suite>(new myTest));
  //myTest tests;
  std::ofstream pages;

  Test::HtmlOutput output;
  pages.open("pages.htm");
  ts.run(output);
  output.generate(pages);
  return 0;
}

