#ifndef __DEMO_H_
#define __DEMO_H_
#include <cpptest.h>
#include "agenttiny.h"
#include "adapter_layer.h"
#include "atiny_log.h"
#include "stub.h"
#include "object_comm.h"

enum
  {
    OBJ_SECURITY_INDEX = 0,
    OBJ_SERVER_INDEX,
    OBJ_DEVICE_INDEX,
    OBJ_FIRMWARE_INDEX,
    OBJ_CONNECT_INDEX,
    OBJ_APP_INDEX,
    OBJ_MAX_NUM,
  };


typedef struct
{
  lwm2m_context_t * lwm2m_context;
  atiny_param_t     atiny_params;
  client_data_t     client_data;
  lwm2m_object_t*   obj_array[OBJ_MAX_NUM];
  int atiny_quit;
}handle_data_t;


class TestAgenttiny:public Test::Suite {
  atiny_param_t prv_atiny_params;
  void* prv_handle = NULL;

 protected:
  void tear_down();
  void setup();

 public:
  void test_func1();
  void test_func2();
  void test_func3();
  void test_func4();
  void test_func5();

  TestAgenttiny();
};
#endif
