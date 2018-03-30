#include <cpptest.h>
#include "connection.h"
#include "agenttiny.h"

class TestConnection: public Test::Suite {

 public:
  atiny_device_info_t prv_dev_info;
  atiny_param_t prv_atiny_params;
  void * prv_handle;
 protected:
  void setup();
  void tear_down();

 public:
  void test_func1();
  void test_func2();
  void test_func3();
  void test_func4();
  void test_func5();
  void test_func6();
  void test_func7();
  void test_func8();

  TestConnection();
  
};

