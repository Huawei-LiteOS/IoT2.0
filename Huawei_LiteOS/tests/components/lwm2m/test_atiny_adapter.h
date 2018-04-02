#ifndef _TEST_ATINY_ADAPTER_H_
#define _TEST_ATINY_ADAPTER_H_

class TestAtinyAdapter:public Test::Suite {
 protected:
  void tear_down();
  void setup();

 public:
  void test_atiny_gettime_ms();
  void test_atiny_random();
  void test_atiny_malloc();
  void test_atiny_free();
  void test_atiny_snprintf();
  void test_atiny_printf();
  void test_atiny_mutex_create();
  void test_atiny_mutex_destroy();
  void test_atiny_mutex_lock();
  void test_atiny_mutex_unlock();

  TestAtinyAdapter();
};


#endif

