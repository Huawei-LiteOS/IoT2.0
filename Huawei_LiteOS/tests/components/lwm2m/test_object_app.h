#ifndef _TEST_OBJECT_APP_H_
#define _TEST_OBJECT_APP_H_

class TestObjectApp:public Test::Suite {
 protected:
  void tear_down();
  void setup();

 public:
  void test_func1();
  void test_func2();
  void test_func3();

  TestObjectApp();
};


#endif

