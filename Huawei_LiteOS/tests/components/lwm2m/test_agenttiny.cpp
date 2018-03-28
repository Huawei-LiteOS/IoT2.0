#include <cpptest.h>
#include <iostream>
#include <fstream>
#include <memory>
#include "test_agenttiny.h"

void myTest::test_func1(){
    atiny_param_t * p_atiny_param = NULL;
    void * handle = NULL;

    TEST_ASSERT_MSG((atiny_init(p_atiny_param, &handle) == ATINY_ARG_INVALID), "Test atiny_init(NULL, NULL) Failed");
  };
void myTest::test_func2(){
  atiny_param_t * atiny_param = &this->prv_atiny_params;
  void * handle = this->prv_handle;
  atiny_security_param_t *security_param = NULL;
    atiny_param->server_params.binding = "UQS";
    atiny_param->server_params.life_time = 20;
    atiny_param->server_params.storing = FALSE;
    atiny_param->server_params.storing_cnt = 0;

    security_param = &(atiny_param->security_params[0]);
    security_param->is_bootstrap = FALSE;
    security_param->server_ip = "139.159.209.89";
    security_param->server_port = "5684";
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
  };

void myTest::test_func3(){
        atiny_log_e log_level = atiny_get_log_level();
        atiny_set_log_level(LOG_FATAL);
        TEST_ASSERT((atiny_get_log_level() == LOG_FATAL));
        atiny_set_log_level(LOG_INFO);
        TEST_ASSERT((atiny_get_log_level() == LOG_INFO));
        atiny_set_log_level(log_level);
        TEST_ASSERT((atiny_get_log_level() == log_level));
        
    }

void myTest::test_func4(){
      atiny_device_info_t dev_info;
      
      dev_info.endpoint_name = "test_epname";
      dev_info.manufacturer = "prv_manu";
      dev_info.dev_type = NULL;

      int ret = atiny_bind(NULL, NULL);
      TEST_ASSERT((ret == ATINY_ARG_INVALID));

      //      ret = atiny_bind(&dev_info, this->prv_handle);
      //TEST_ASSERT((0));
    }

void myTest::test_func5(){
       void * handle = NULL;
       stubInfo si;
       //       setStub((void*)atiny_init, (void*)stub_atiny_init, &si);
       atiny_init(NULL, NULL);
       cleanStub(&si);
    }

myTest::myTest(){
    TEST_ADD(myTest::test_func1);
    TEST_ADD(myTest::test_func2);
    TEST_ADD(myTest::test_func3);
    TEST_ADD(myTest::test_func4);
  }

//protected:
void myTest::setup(){
    atiny_param_t* atiny_params = &(this->prv_atiny_params);
 
    atiny_params->server_params.binding = "UQS";
    atiny_params->server_params.life_time = 20;
    atiny_params->server_params.storing = FALSE;
    atiny_params->server_params.storing_cnt = 0;
    
    std::cout<<"in steup\n";
    
    atiny_init(atiny_params, &this->prv_handle);
    TEST_ASSERT((atiny_params != NULL));
  }

void myTest::tear_down(){
    std::cout<<"in teardown\n";
    atiny_deinit(this->prv_handle);
  }

int main()
{
  Test::Suite ts;
  ts.add(std::auto_ptr<Test::Suite>(new myTest));
  std::ofstream pages;

  Test::HtmlOutput output;
  pages.open("pages.htm");
  ts.run(output);
  output.generate(pages);
  std::cout<<"All TestCases fininsh!\n";
  return 0;

}

