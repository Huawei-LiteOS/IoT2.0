#ifndef _TEST_OBJECT_APP_H_
#define _TEST_OBJECT_APP_H_

class TestObjectApp:public Test::Suite {
 protected:
  void tear_down();
  void setup();

 public:
  void test_prv_read();
  void test_prv_write();
  void test_prv_exec();

  TestObjectApp();
};


#endif

