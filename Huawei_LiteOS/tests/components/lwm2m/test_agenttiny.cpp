#include <cpptest.h>
#include <iostream>
#include <fstream>
#include <memory>
#include "test_agenttiny.h"

unsigned long g_lwm2m_context           = 0;
unsigned long g_object_security_pointer = 0;
unsigned long g_object_server_pointer   = 0;
unsigned long g_object_device_pointer   = 0;
unsigned long g_object_firmware_pointer = 0;
unsigned long g_object_conn_m_pointer   = 0;
unsigned long g_object_app_pointer      = 0;
unsigned long g_atiny_mutex_pointer     = 0;


extern  "C"
{
    extern int  atiny_init_objects(atiny_param_t* atiny_params, const atiny_device_info_t* device_info, handle_data_t *handle);
    extern lwm2m_context_t * lwm2m_init(void * userData);
    extern lwm2m_object_t * get_security_object(int serverId,
                                     const char* serverUri,
                                     char * bsPskId,
                                     char * psk,
                                     uint16_t pskLen,
                                     bool isBootstrap);
    extern lwm2m_object_t * get_server_object(int serverId,
                                   const char* binding,
                                   int lifetime,
                                   bool storing);
    extern lwm2m_object_t * get_object_device(atiny_param_t *atiny_params, const char* manufacturer);
    extern lwm2m_object_t * get_object_firmware(atiny_param_t *atiny_params);
    extern lwm2m_object_t * get_object_conn_m(atiny_param_t* atiny_params);
    extern lwm2m_object_t * get_platform_object(atiny_param_t* atiny_params);
    extern void atiny_detroy(void* handle);
	extern int atiny_queue_rpt_data(const lwm2m_uri_t *uri, const data_report_t *data);
	extern void observe_handleAck(lwm2m_transaction_t * transacP, void * message);
	 
    lwm2m_context_t * stub_lwm2m_init(void * userData)
    {
        return (lwm2m_context_t *)g_lwm2m_context;
    }
	 
     lwm2m_object_t * stub_get_security_object(int serverId,
                                     const char* serverUri,
                                     char * bsPskId,
                                     char * psk,
                                     uint16_t pskLen,
                                     bool isBootstrap)
    {
        return (lwm2m_object_t *)g_object_security_pointer;
    }

    lwm2m_object_t * stub_get_server_object(int serverId,
                                   const char* binding,
                                   int lifetime,
                                   bool storing)
    {
        return (lwm2m_object_t *)g_object_server_pointer;
    }

    lwm2m_object_t * stub_get_object_device(atiny_param_t *atiny_params, const char* manufacturer)
    {
        return (lwm2m_object_t *)g_object_device_pointer;
    }

    lwm2m_object_t * stub_get_object_firmware(atiny_param_t *atiny_params)
    {
        return (lwm2m_object_t *)g_object_firmware_pointer;
    }
	 
    lwm2m_object_t * stub_get_object_conn_m(atiny_param_t* atiny_params)
    {
        return (lwm2m_object_t *)g_object_conn_m_pointer;
    }
	 
    lwm2m_object_t * stub_get_platform_object(atiny_param_t* atiny_params)
    {
        return (lwm2m_object_t *)g_object_app_pointer;
    }

    void *stub_atiny_mutex_create(void)
    {
        printf("call stub_atiny_mutex_create ,%p\n",(void *)g_atiny_mutex_pointer);
        return (void *)g_atiny_mutex_pointer;
    }

    void stub_atiny_mutex_destroy(void *mutex)
    {
    }
    void stub_atiny_mutex_lock(void *mutex)
    {
        printf("call stub_atiny_mutex_lock ,%p\n",mutex);
    }
    void stub_atiny_mutex_unlock(void *mutex)
    {
        printf("call stub_atiny_mutex_unlock ,%p\n",mutex);
    }
}


void TestAgenttiny::test_atiny_init()
{
  atiny_param_t * atiny_param = NULL;
  void * handle = NULL;
  atiny_security_param_t *security_param = NULL;

  TEST_ASSERT_MSG((atiny_init(atiny_param, &handle) == ATINY_ARG_INVALID), "Test atiny_init(NULL, NULL) Failed");

  atiny_param = &this->prv_atiny_params;
  handle = this->prv_handle;
  atiny_param->server_params.binding = (char *)"UQS";
  atiny_param->server_params.life_time = 20;
  atiny_param->server_params.storing = FALSE;
  atiny_param->server_params.storing_cnt = 0;

  security_param = &(atiny_param->security_params[0]);
  security_param->is_bootstrap = FALSE;
  security_param->server_ip = (char *)"139.159.209.89";
  security_param->server_port = (char *)"5684";
  //security_param->psk_Id = "666003";

  TEST_ASSERT_EQUALS(atiny_init(atiny_param, &handle), ATINY_OK);
  TEST_ASSERT((handle != NULL));
  TEST_ASSERT(0 == memcmp(((handle_data_t *)handle)->atiny_params.server_params.binding, "UQS", strlen("UQS")));
  TEST_ASSERT((((handle_data_t *)this->prv_handle)->atiny_params.server_params.life_time == 20));
  TEST_ASSERT((((handle_data_t *)this->prv_handle)->atiny_params.server_params.storing == FALSE));
  TEST_ASSERT((((handle_data_t *)this->prv_handle)->atiny_params.server_params.storing_cnt == 0));

  TEST_ASSERT(((handle_data_t*)this->prv_handle)->atiny_params.security_params[0].is_bootstrap == FALSE);
  TEST_ASSERT(0 == memcmp(((handle_data_t*)this->prv_handle)->atiny_params.security_params[0].server_ip, "139.159.209.89", strlen("139.159.209.89")));
  TEST_ASSERT(0 == memcmp(((handle_data_t*)this->prv_handle)->atiny_params.security_params[0].server_port, "5684", strlen("5684")));
}

void TestAgenttiny::test_atiny_bind()
{
  atiny_device_info_t dev_info;

  dev_info.endpoint_name = NULL;
  dev_info.manufacturer = NULL;
  dev_info.dev_type = NULL;

  int ret = atiny_bind(NULL, NULL);
  TEST_ASSERT((ret == ATINY_ARG_INVALID));

  ret = atiny_bind(&dev_info, this->prv_handle);
  TEST_ASSERT((ret == ATINY_ARG_INVALID));
  
  ret = atiny_bind(&dev_info, this->prv_handle);
  TEST_ASSERT((ret == ATINY_ARG_INVALID));
  dev_info.manufacturer = NULL;(char *)"prv_manu";
  //TODO
}
extern int atiny_state_is_ready(void *phandle);
void TestAgenttiny::test_atiny_state_is_ready()
{
  handle_data_t handle;
  lwm2m_context_t context;

  handle.lwm2m_context = &context;

  context.state = STATE_READY;
  TEST_ASSERT((atiny_state_is_ready(&handle) == true));

  context.state = STATE_INITIAL;
  TEST_ASSERT((atiny_state_is_ready(&handle) == false));
}
void TestAgenttiny::test_atiny_init_objects()
{
  atiny_param_t st_atiny_param;
  atiny_param_t * atiny_param = NULL;
  handle_data_t handle;
  handle_data_t atiny_handle;
  
  atiny_security_param_t *security_param = NULL;

  atiny_device_info_t dev_info;
  dev_info.endpoint_name = (char *)"66660003";
  dev_info.manufacturer  = (char *)"huawei";
  unsigned char psk_c[16] = {0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x99,0x99};

  atiny_param = &st_atiny_param;
  
  memset(atiny_param,0,sizeof(*atiny_param)); 
  atiny_param->server_params.binding = (char *)"UQS";
  atiny_param->server_params.life_time = 20;
  atiny_param->server_params.storing = FALSE;
  atiny_param->server_params.storing_cnt = 0;

  security_param = &(atiny_param->security_params[0]);
  security_param->is_bootstrap = FALSE;
  security_param->server_ip = (char *)"139.159.209.89";
  security_param->server_port = (char *)"5684";
  

  /*¶ÔËø²Ù×÷´ò×®*/
  stubInfo si_create_mutex;
  stubInfo si_destroy_mutex;
  stubInfo si_mutex_lock;
  stubInfo si_mutex_unlock;
  setStub((void *)atiny_mutex_create,(void *)stub_atiny_mutex_create,&si_create_mutex);
  setStub((void *)atiny_mutex_destroy,(void *)stub_atiny_mutex_destroy,&si_destroy_mutex);
  setStub((void *)atiny_mutex_lock,(void *)stub_atiny_mutex_lock,&si_mutex_lock);
  setStub((void *)atiny_mutex_unlock,(void *)stub_atiny_mutex_unlock,&si_mutex_unlock);
  
  memset(&handle,0,sizeof(handle_data_t));
  memcpy(&handle.atiny_params,atiny_param,sizeof(atiny_param_t));
  TEST_ASSERT_EQUALS(atiny_init_objects(NULL, &dev_info, (handle_data_t *)&handle), ATINY_ARG_INVALID);
  printf("after call atiny_init_objects!!!");
  atiny_detroy((void *)&handle);
  
  
  

  
  
  g_atiny_mutex_pointer = 0;
  memset(&handle,0,sizeof(handle_data_t));
  memcpy(&handle.atiny_params,atiny_param,sizeof(atiny_param_t));
  TEST_ASSERT_EQUALS(atiny_init_objects(atiny_param, &dev_info, (handle_data_t *)&handle), ATINY_RESOURCE_NOT_ENOUGH);
  atiny_detroy((void *)&handle);
  printf("\n");


  
  stubInfo si_lwm2m_init;
  setStub((void *)lwm2m_init,(void *)stub_lwm2m_init,&si_lwm2m_init);
  g_atiny_mutex_pointer = 0x1000000;
  g_lwm2m_context = 0;
  memset(&handle,0,sizeof(handle_data_t));
  memcpy(&handle.atiny_params,atiny_param,sizeof(atiny_param_t));
  TEST_ASSERT_EQUALS(atiny_init_objects(atiny_param, &dev_info, (handle_data_t *)&handle), ATINY_MALLOC_FAILED);
  atiny_detroy((void *)&handle); 
  cleanStub(&si_lwm2m_init);



  stubInfo si_get_security_object;
  setStub((void *)get_security_object,(void *)stub_get_security_object,&si_get_security_object);
  g_atiny_mutex_pointer     = 0x1000000;
  g_object_security_pointer = 0;
  memset(&handle,0,sizeof(handle_data_t));
  memcpy(&handle.atiny_params,atiny_param,sizeof(atiny_param_t));
  TEST_ASSERT_EQUALS(atiny_init_objects(atiny_param, &dev_info, (handle_data_t *)&handle), ATINY_MALLOC_FAILED);
  atiny_detroy((void *)&handle);
  cleanStub(&si_get_security_object);

  
  stubInfo si_get_server_object;
  setStub((void *)get_server_object,(void *)stub_get_server_object,&si_get_server_object);
  g_atiny_mutex_pointer = 0x1000000; 
  g_object_server_pointer   = 0;
  memset(&handle,0,sizeof(handle_data_t));
  memcpy(&handle.atiny_params,atiny_param,sizeof(atiny_param_t));
  TEST_ASSERT_EQUALS(atiny_init_objects(atiny_param, &dev_info, (handle_data_t *)&handle), ATINY_MALLOC_FAILED);
  atiny_detroy((void *)&handle);
  cleanStub(&si_get_server_object);



  stubInfo si_get_object_device;
  setStub((void *)get_object_device,(void *)stub_get_object_device,&si_get_object_device);
  g_atiny_mutex_pointer = 0x1000000; 
  g_object_device_pointer   = 0;
  memset(&handle,0,sizeof(handle_data_t));
  memcpy(&handle.atiny_params,atiny_param,sizeof(atiny_param_t));
  TEST_ASSERT_EQUALS(atiny_init_objects(atiny_param, &dev_info, (handle_data_t *)&handle), ATINY_MALLOC_FAILED);
  atiny_detroy((void *)&handle);
  cleanStub(&si_get_object_device);



  stubInfo si_get_object_firmware;
  setStub((void *)get_object_firmware,(void *)stub_get_object_firmware,&si_get_object_firmware);
  g_atiny_mutex_pointer = 0x1000000; 
  g_object_firmware_pointer = 0;
  memset(&handle,0,sizeof(handle_data_t));
  memcpy(&handle.atiny_params,atiny_param,sizeof(atiny_param_t));
  TEST_ASSERT_EQUALS(atiny_init_objects(atiny_param, &dev_info, (handle_data_t *)&handle), ATINY_MALLOC_FAILED);
  atiny_detroy((void *)&handle);
  cleanStub(&si_get_object_firmware);

  stubInfo si_get_object_conn_m;
  setStub((void *)get_object_conn_m,(void *)stub_get_object_conn_m,&si_get_object_conn_m);
  g_atiny_mutex_pointer = 0x1000000; 
  g_object_conn_m_pointer   = 0;
  memset(&handle,0,sizeof(handle_data_t));
  memcpy(&handle.atiny_params,atiny_param,sizeof(atiny_param_t));
  TEST_ASSERT_EQUALS(atiny_init_objects(atiny_param, &dev_info, (handle_data_t *)&handle), ATINY_MALLOC_FAILED);
  atiny_detroy((void *)&handle);
  cleanStub(&si_get_object_conn_m);

  stubInfo si_get_platform_object;
  setStub((void *)get_platform_object,(void *)stub_get_platform_object,&si_get_platform_object);
  g_atiny_mutex_pointer = 0x1000000; 
  g_object_app_pointer      = 0;
  memset(&handle,0,sizeof(handle_data_t));
  memcpy(&handle.atiny_params,atiny_param,sizeof(atiny_param_t));
  TEST_ASSERT_EQUALS(atiny_init_objects(atiny_param, &dev_info, (handle_data_t *)&handle), ATINY_MALLOC_FAILED);
  atiny_detroy((void *)&handle);
  cleanStub(&si_get_platform_object);
  
  printf("after si_get_platform_object \n");
  
  
  
  



  security_param->psk_Id = NULL;
  security_param->psk = NULL;
  
  memset(&handle,0,sizeof(handle_data_t));
  memcpy(&handle.atiny_params,atiny_param,sizeof(atiny_param_t));
  TEST_ASSERT_EQUALS(atiny_init_objects(atiny_param, &dev_info, (handle_data_t *)&handle), ATINY_OK);
  atiny_detroy((void *)&handle);
  
#if 1
  security_param->psk_Id = (char *)"66660003";
  security_param->psk = (char *)psk_c;
  security_param->psk_len = 16;
  memset(&handle,0,sizeof(handle_data_t));
  memcpy(&handle.atiny_params,atiny_param,sizeof(atiny_param_t));
  TEST_ASSERT_EQUALS(atiny_init_objects(atiny_param, &dev_info, (handle_data_t *)&handle), ATINY_OK); 
  atiny_detroy((void *)&handle);
#endif
  
  cleanStub(&si_create_mutex);
  cleanStub(&si_destroy_mutex);
  cleanStub(&si_mutex_lock);
  cleanStub(&si_mutex_unlock);

  
}

void TestAgenttiny::test_atiny_destory()
{
  
}

void TestAgenttiny::test_atiny_deinit()
{
  handle_data_t * handle = NULL;
  //  atiny_deinit(handle);

  handle = (handle_data_t*)this->prv_handle;
  atiny_deinit(handle);
  TEST_ASSERT((handle->atiny_quit == 1));
  
}

int stub_lwm2m_stringToUri(const char * buffer,
		      size_t buffer_len,
		      lwm2m_uri_t * uriP)
{
  return 0;
}

static int gi_stub_ret = 0;
int stub_atiny_queue_rpt_data(const lwm2m_uri_t *uri, const data_report_t *data)
{
  return gi_stub_ret;
}
void TestAgenttiny::test_atiny_data_report()
{
  void * handle = NULL;
  data_report_t report_data;
  lwm2m_context_t* atiny_context;
  
  int ret = atiny_data_report(handle, NULL);
  TEST_ASSERT((ret == ATINY_ARG_INVALID));

  ret = atiny_data_report(handle, &report_data);
  TEST_ASSERT((ret == ATINY_ARG_INVALID));
  
  handle = this->prv_handle;
  atiny_context = ((handle_data_t*)handle)->lwm2m_context;
  atiny_context->state = STATE_INITIAL;
  report_data.buf = (uint8_t*)"test data";
  report_data.len = strlen("test data");
  ret = atiny_data_report(handle, &report_data);
  std::cout<<"ret = "<<ret<<"\n";
  TEST_ASSERT((ret == ATINY_CLIENT_UNREGISTERED));


  stubInfo si;
  stubInfo si2;
  setStub((void*)lwm2m_stringToUri, (void*)stub_lwm2m_stringToUri, &si);
  setStub((void*)atiny_queue_rpt_data, (void*)stub_atiny_queue_rpt_data, &si2);

  gi_stub_ret = ATINY_OK;
  atiny_context->state = STATE_READY;
  report_data.type = FIRMWARE_UPDATE_STATE;
  ret = atiny_data_report(handle, &report_data);
  TEST_ASSERT(ret == ATINY_OK);
  
  gi_stub_ret = ATINY_BUF_OVERFLOW;
  ret = atiny_data_report(handle, &report_data);
  TEST_ASSERT(ret == ATINY_BUF_OVERFLOW);

  
  cleanStub(&si2);
  cleanStub(&si);
}

static int gs_callback_status = -1;
void test_atiny_ack_callback(atiny_report_type_e type, int cookie, DATA_SEND_STATUS status)
{
  gs_callback_status = status;

}

void TestAgenttiny::test_observe_handleAck()
{
  lwm2m_transaction_t trans;
  void* message = NULL;
  
  trans.cfg.callback = (lwm2m_data_process)test_atiny_ack_callback;
  trans.ack_received = 1;

  observe_handleAck(&trans, message);
  TEST_ASSERT(gs_callback_status == SENT_SUCCESS);

  gs_callback_status = -1;
  trans.ack_received = 0;
  trans.retrans_counter = 5;
  observe_handleAck(&trans, message);
  TEST_ASSERT(gs_callback_status == SENT_TIME_OUT);
  

}
TestAgenttiny::TestAgenttiny()
{
  TEST_ADD(TestAgenttiny::test_atiny_init);
  TEST_ADD(TestAgenttiny::test_atiny_bind);
  TEST_ADD(TestAgenttiny::test_atiny_state_is_ready);
  TEST_ADD(TestAgenttiny::test_atiny_init_objects);
  TEST_ADD(TestAgenttiny::test_atiny_destory);
  TEST_ADD(TestAgenttiny::test_atiny_deinit);
  TEST_ADD(TestAgenttiny::test_observe_handleAck);
  TEST_ADD(TestAgenttiny::test_atiny_data_report);

}

//protected:
void TestAgenttiny::setup()
{
  printf("setup in TestAgenttiny\n");
  atiny_param_t* atiny_params = &(this->prv_atiny_params);

  atiny_params->server_params.binding = (char *)"UQS";
  atiny_params->server_params.life_time = 20;
  atiny_params->server_params.storing = FALSE;
  atiny_params->server_params.storing_cnt = 0;

  atiny_init(atiny_params, &this->prv_handle);

  ((handle_data_t*)this->prv_handle)->lwm2m_context = (lwm2m_context_t*)malloc(sizeof(lwm2m_context_t));
  TEST_ASSERT((atiny_params != NULL));
  TEST_ASSERT(this->prv_handle != NULL);
}

void TestAgenttiny::tear_down()
{
  free(((handle_data_t*)this->prv_handle)->lwm2m_context);
  atiny_deinit(this->prv_handle);
  printf("tear_down in TestAgenttiny\n");
}

