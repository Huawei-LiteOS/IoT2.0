#include <cpptest.h>
#include <iostream>
#include <fstream>
#include <memory>
#include "test_agenttiny.h"

void TestAgenttiny::test_atiny_init()
{
  atiny_param_t * atiny_param = NULL;
  void * handle = NULL;
  atiny_security_param_t *security_param = NULL;

  TEST_ASSERT_MSG((atiny_init(atiny_param, &handle) == ATINY_ARG_INVALID), "Test atiny_init(NULL, NULL) Failed");

  atiny_param = &this->prv_atiny_params;
  handle = this->prv_handle;
  atiny_param->server_params.binding = (char *)"UQS";
  atiny_param->server_params.life_time = 20;
  atiny_param->server_params.storing = FALSE;
  atiny_param->server_params.storing_cnt = 0;

  security_param = &(atiny_param->security_params[0]);
  security_param->is_bootstrap = FALSE;
  security_param->server_ip = (char *)"139.159.209.89";
  security_param->server_port = (char *)"5684";
  //security_param->psk_Id = "666003";

  TEST_ASSERT_EQUALS(atiny_init(atiny_param, &handle), ATINY_OK);
  TEST_ASSERT((handle != NULL));
  TEST_ASSERT(0 == memcmp(((handle_data_t *)handle)->atiny_params.server_params.binding, "UQS", strlen("UQS")));
  TEST_ASSERT((((handle_data_t *)this->prv_handle)->atiny_params.server_params.life_time == 20));
  TEST_ASSERT((((handle_data_t *)this->prv_handle)->atiny_params.server_params.storing == FALSE));
  TEST_ASSERT((((handle_data_t *)this->prv_handle)->atiny_params.server_params.storing_cnt == 0));

  TEST_ASSERT(((handle_data_t*)this->prv_handle)->atiny_params.security_params[0].is_bootstrap == FALSE);
  TEST_ASSERT(0 == memcmp(((handle_data_t*)this->prv_handle)->atiny_params.security_params[0].server_ip, "139.159.209.89", strlen("139.159.209.89")));
  TEST_ASSERT(0 == memcmp(((handle_data_t*)this->prv_handle)->atiny_params.security_params[0].server_port, "5684", strlen("5684")));
}

void TestAgenttiny::test_atiny_bind()
{
  atiny_device_info_t dev_info;

  dev_info.endpoint_name = NULL;
  dev_info.manufacturer = NULL;
  dev_info.dev_type = NULL;

  int ret = atiny_bind(NULL, NULL);
  TEST_ASSERT((ret == ATINY_ARG_INVALID));

  ret = atiny_bind(&dev_info, this->prv_handle);
  TEST_ASSERT((ret == ATINY_ARG_INVALID));
  
  ret = atiny_bind(&dev_info, this->prv_handle);
  TEST_ASSERT((ret == ATINY_ARG_INVALID));
  dev_info.manufacturer = NULL;(char *)"prv_manu";
  //TODO
}
extern int atiny_state_is_ready(void *phandle);
void TestAgenttiny::test_atiny_state_is_ready()
{
  handle_data_t handle;
  lwm2m_context_t context;

  handle.lwm2m_context = &context;

  context.state = STATE_READY;
  TEST_ASSERT((atiny_state_is_ready(&handle) == true));

  context.state = STATE_INITIAL;
  TEST_ASSERT((atiny_state_is_ready(&handle) == false));
}
void TestAgenttiny::test_atiny_init_objects()
{
  
}

void TestAgenttiny::test_atiny_destory()
{
  
}

void TestAgenttiny::test_atiny_deinit()
{
  handle_data_t * handle = NULL;
  //  atiny_deinit(handle);

  handle = (handle_data_t*)this->prv_handle;
  atiny_deinit(handle);
  TEST_ASSERT((handle->atiny_quit == 1));
  
}

void TestAgenttiny::test_atiny_data_report()
{

}

void TestAgenttiny::test_observe_handleAck()
{

}
TestAgenttiny::TestAgenttiny()
{
  TEST_ADD(TestAgenttiny::test_atiny_init);
  TEST_ADD(TestAgenttiny::test_atiny_bind);
  TEST_ADD(TestAgenttiny::test_atiny_state_is_ready);
  TEST_ADD(TestAgenttiny::test_atiny_init_objects);
  TEST_ADD(TestAgenttiny::test_atiny_destory);
  TEST_ADD(TestAgenttiny::test_atiny_deinit);
  TEST_ADD(TestAgenttiny::test_observe_handleAck);
  TEST_ADD(TestAgenttiny::test_atiny_data_report);

}

//protected:
void TestAgenttiny::setup()
{
  atiny_param_t* atiny_params = &(this->prv_atiny_params);

  atiny_params->server_params.binding = (char *)"UQS";
  atiny_params->server_params.life_time = 20;
  atiny_params->server_params.storing = FALSE;
  atiny_params->server_params.storing_cnt = 0;

  atiny_init(atiny_params, &this->prv_handle);
  TEST_ASSERT((atiny_params != NULL));
}

void TestAgenttiny::tear_down()
{
  atiny_deinit(this->prv_handle);
}

