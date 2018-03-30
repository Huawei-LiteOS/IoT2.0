#ifndef _TEST_OBJECT_DEVICE_H_
#define _TEST_OBJECT_DEVICE_H_

class TestObjectDevice:public Test::Suite {
 protected:
  void tear_down();
  void setup();

 public:
  void test_func1();
  void test_func2();


  TestObjectDevice();
};


#endif

