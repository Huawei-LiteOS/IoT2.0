#ifndef _TEST_OBJECT_CONNECTIVITY_MONI_H_
#define _TEST_OBJECT_CONNECTIVITY_MONI_H_


class TestObjectConnectivityMoni:public Test::Suite {

 protected:
  void tear_down();
  void setup();

 public:
  void test_func1();


  TestObjectConnectivityMoni();
};



#endif