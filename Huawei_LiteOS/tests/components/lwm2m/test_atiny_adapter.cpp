
#include <cpptest.h>
#include <iostream>
#include <memory>
#include "agenttiny.h"
#include "adapter_layer.h"
#include "test_atiny_adapter.h"

extern UINT64  g_ullTickCount;
extern UINT32  g_vuwIntCount;
extern UINT32  g_semCrt;
  void TestAtinyAdapter::test_atiny_gettime_ms(){

	uint64_t time;
	time = atiny_gettime_ms();
	TEST_ASSERT_MSG(time == g_ullTickCount, "test AtinyAdapter atiny_gettime_ms() failed");
	//except branch
	g_vuwIntCount = 1;
	time = atiny_gettime_ms();
	TEST_ASSERT_MSG(time == 0, "test AtinyAdapter atiny_gettime_ms() failed");
	g_vuwIntCount = 0;
  }

  void TestAtinyAdapter::test_atiny_random(){
    int uwRet;
	size_t len = 24;
	unsigned char output[len];
	uwRet = atiny_random(output, len);
	TEST_ASSERT_MSG(uwRet == 0, "test AtinyAdapter atiny_random() failed");

	uwRet = atiny_random(output, 0);
	TEST_ASSERT_MSG(uwRet == 0, "test AtinyAdapter atiny_random() failed");

	uwRet = atiny_random(NULL, 0);
	TEST_ASSERT_MSG(uwRet == 0, "test AtinyAdapter atiny_random() failed");

  }
  void TestAtinyAdapter::test_atiny_malloc(){
    size_t size = 100;
	void * mem = NULL;
	mem = atiny_malloc(size);
	TEST_ASSERT_MSG(mem != NULL, "test AtinyAdapter atiny_malloc() failed");
	free(mem);

	mem = atiny_malloc(0);
	TEST_ASSERT_MSG(mem != NULL, "test AtinyAdapter atiny_malloc() failed");
	free(mem);

	mem = atiny_malloc(-1);
	TEST_ASSERT_MSG(mem == NULL, "test AtinyAdapter atiny_malloc() failed");
	printf("mem = %p\r\n",mem);
	if(mem != NULL)
	{
      free(mem);	
	}
  }

  void TestAtinyAdapter::test_atiny_free(){
    size_t size = 100;
	void * mem = NULL;
	mem = atiny_malloc(size);
	TEST_ASSERT_MSG(mem != NULL, "test AtinyAdapter atiny_malloc() failed");

	atiny_free(mem);
	TEST_ASSERT_MSG(mem != NULL, "test AtinyAdapter atiny_free() failed");
  }

  void TestAtinyAdapter::test_atiny_snprintf(){
    size_t size = 100;
	int ret;
	char buf[size];
	char *str = (char *)"test_atiny_snprintf";
	ret = atiny_snprintf(buf,size,"%s",str);
	TEST_ASSERT_MSG(ret == strlen(str), "test AtinyAdapter atiny_snprintf() failed");
	printf("test_atiny_snprintf ret = %d, strlen = %d\n",ret,strlen(str));

	ret = memcmp(buf,str,ret);
    TEST_ASSERT_MSG(ret == 0, "test AtinyAdapter atiny_snprintf() failed");
  }

  void TestAtinyAdapter::test_atiny_printf(){
	int ret;
	char *str = (char *)"test_atiny_snprintf";
	ret = atiny_printf("%s",str);
	TEST_ASSERT_MSG(ret == strlen(str), "test AtinyAdapter atiny_printf() failed");
  }

  void TestAtinyAdapter::test_atiny_mutex_create(){
	void * mutex = NULL;
	g_semCrt = 0;
	mutex = atiny_mutex_create();
	TEST_ASSERT_MSG(mutex != NULL, "test AtinyAdapter atiny_mutex_create() failed");
	//except branch
	g_semCrt = -1;
	mutex = atiny_mutex_create();
	TEST_ASSERT_MSG(mutex == NULL, "test AtinyAdapter atiny_mutex_create() failed");

	g_vuwIntCount = 1;
	mutex = atiny_mutex_create();
	TEST_ASSERT_MSG(mutex == NULL, "test AtinyAdapter atiny_mutex_create() failed");
	g_vuwIntCount = 0;
  }

  void TestAtinyAdapter::test_atiny_mutex_destroy(){
	int ret = 0;
	atiny_mutex_destroy(NULL);
	TEST_ASSERT_MSG(ret == 0, "test AtinyAdapter atiny_mutex_destroy() failed");
  }

  void TestAtinyAdapter::test_atiny_mutex_lock(){
	int ret = 0;
	atiny_mutex_lock(NULL);
	TEST_ASSERT_MSG(ret == 0, "test AtinyAdapter atiny_mutex_lock() failed");
  }

  void TestAtinyAdapter::test_atiny_mutex_unlock(){
	int ret = 0;
	atiny_mutex_unlock(NULL);
	TEST_ASSERT_MSG(ret == 0, "test AtinyAdapter atiny_mutex_unlock() failed");
  }

  TestAtinyAdapter::TestAtinyAdapter(){
	TEST_ADD(TestAtinyAdapter::test_atiny_gettime_ms);
    TEST_ADD(TestAtinyAdapter::test_atiny_random);
	TEST_ADD(TestAtinyAdapter::test_atiny_malloc);
	TEST_ADD(TestAtinyAdapter::test_atiny_free);
    TEST_ADD(TestAtinyAdapter::test_atiny_snprintf);
	TEST_ADD(TestAtinyAdapter::test_atiny_printf);
	TEST_ADD(TestAtinyAdapter::test_atiny_mutex_create);
	TEST_ADD(TestAtinyAdapter::test_atiny_mutex_destroy);
	TEST_ADD(TestAtinyAdapter::test_atiny_mutex_lock);
	TEST_ADD(TestAtinyAdapter::test_atiny_mutex_unlock);
  }

  void TestAtinyAdapter::setup(){
    std::cout<<"in TestAtinyAdapter\n";
  }

  void TestAtinyAdapter::tear_down(){
    std::cout<<"in TestAtinyAdapter\n";
  }




