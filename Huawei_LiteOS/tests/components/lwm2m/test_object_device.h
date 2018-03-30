#ifndef _TEST_OBJECT_DEVICE_H_
#define _TEST_OBJECT_DEVICE_H_

class TestObjectDevice:public Test::Suite {
 protected:
  void tear_down();
  void setup();

 public:
  void test_prv_device_read();
  void test_prv_device_execute();


  TestObjectDevice();
};


#endif

