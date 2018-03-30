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
  void test_atiny_init();
  void test_atiny_state_is_ready();
  void test_atiny_bind();
  void test_atiny_init_objects();
  void test_atiny_destory();
  void test_atiny_deinit();
  void test_atiny_data_report();
  void test_observe_handleAck();
  
  TestAgenttiny();
};
#endif
