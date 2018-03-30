#include "object_comm.h"
#include "test_connection.h"
#include "connection.h"
#include "atiny_socket.h"
#include "stub.h"
#include <unistd.h>
#include <fcntl.h>

#define SERVER_URI_MAX_LEN 64
typedef struct
{
  int fd;
}
atiny_net_context;

void TestConnection::test_func1()
{
  connection_t conn;
  lwm2m_object_t securityObj;

  securityObj.instanceList = NULL;
  connection_t * connP = connection_create(&conn, &securityObj, 0, NULL);
  TEST_ASSERT((connP == NULL));
}

void * stub_atiny_net_connect(const char *host, const char *port, int proto)
{
  return NULL;
}

void TestConnection::test_func2()
{
  connection_t conn;
  connection_t * connP = NULL;
  lwm2m_context_t context;
  atiny_param_t* atiny_params = &this->prv_atiny_params;
  int serverId = 123;
  char serverUri[SERVER_URI_MAX_LEN];

  if(atiny_params->security_params[0].psk != NULL)
  {
    snprintf(serverUri, SERVER_URI_MAX_LEN, "coaps://%s:%s",
	       atiny_params->security_params[0].server_ip, atiny_params->security_params[0].server_port);
  }
  else
  {
    snprintf(serverUri, SERVER_URI_MAX_LEN, "coap://%s:%s",
	       atiny_params->security_params[0].server_ip, atiny_params->security_params[0].server_port);
  }

  lwm2m_object_t * securityObj = get_security_object(serverId, serverUri, atiny_params->security_params[0].psk_Id, atiny_params->security_params[0].psk,atiny_params->security_params[0].psk_len, false);
  TEST_ASSERT((securityObj != NULL));

  connP = connection_create(&conn, securityObj, 0, &context);
  TEST_ASSERT_MSG((connP != NULL), "Test in connection_create when atiny_net_connect is Failed!");
  TEST_ASSERT((connP->next == &conn));
  TEST_ASSERT((connP->securityObj == securityObj));
  TEST_ASSERT((connP->securityInstId == 0));
  TEST_ASSERT((connP->lwm2mH == &context));

  stubInfo si;
  setStub((void*)atiny_net_connect, (void *)stub_atiny_net_connect, &si);
  connP = connection_create(&conn, securityObj, 0, &context);
  TEST_ASSERT_MSG((connP == NULL), "Test in connection_create when atiny_net_connect return NULL is Failed!");
  cleanStub(&si);

  clean_security_object(securityObj);
}

void TestConnection::test_func3()
{
  uint8_t ret = lwm2m_buffer_send(NULL, NULL, 0, NULL);
  TEST_ASSERT((ret == COAP_500_INTERNAL_SERVER_ERROR));
}

int stub_atiny_net_send1( void *ctx, const unsigned char *buf, size_t len )
{
  //stub func for test lwm2m_buffer_send(), return len
  std::cout<<"return "<<len<< "\n";
  return len;
}


void TestConnection::test_func4()
{
  connection_t conn;
  uint8_t buf[64] = "test str";
  stubInfo si;

  setStub((void*)atiny_net_send, (void*)stub_atiny_net_send1, &si);
  
  uint8_t ret = lwm2m_buffer_send(&conn, buf, ATINY_ERR_INVALID_CONTEXT, NULL);
  TEST_ASSERT((ret == COAP_500_INTERNAL_SERVER_ERROR));

  ret = lwm2m_buffer_send(&conn, buf, COAP_NO_ERROR, NULL);
  TEST_ASSERT((ret == 0));

  cleanStub(&si);
}

void TestConnection::test_func5()
{
  connection_t conn;
  atiny_net_context anc;
  const char * buf = "hello test";
  char buf2[64] = {0};

  conn.net_context = &anc;
  anc.fd = open("./demo", O_CREAT|O_RDWR, 0777);
  unlink("./demo");
  uint8_t ret = lwm2m_buffer_send(&conn, (uint8_t*)buf, strlen(buf), NULL);

  read(anc.fd, buf2, 64);
  TEST_ASSERT((strncmp(buf, buf2, strlen(buf))));
  close(anc.fd);
}
void TestConnection::test_func6()
{
  bool ret = lwm2m_session_is_equal(NULL, NULL, NULL);
  TEST_ASSERT((ret == true));
}

void TestConnection::test_func7()
{
  connection_t connA;
  connection_t connB;
  bool ret = lwm2m_session_is_equal((void *)&connA, (void*)&connB, NULL);

  TEST_ASSERT((ret == false));
  
}

void TestConnection::test_func8()
{
  connection_t connA;
  connection_t * p_connB = &connA;
  bool ret = lwm2m_session_is_equal((void *)&connA, (void*)p_connB, NULL);

  TEST_ASSERT((ret == true));
}
TestConnection::TestConnection(){

  TEST_ADD(TestConnection::test_func1);
  TEST_ADD(TestConnection::test_func2);
  TEST_ADD(TestConnection::test_func3);
  TEST_ADD(TestConnection::test_func4);
  TEST_ADD(TestConnection::test_func5);
  TEST_ADD(TestConnection::test_func6);
  TEST_ADD(TestConnection::test_func7);
  TEST_ADD(TestConnection::test_func8);
}

void TestConnection::setup()
{
  atiny_device_info_t *device_info = &this->prv_dev_info;
  atiny_param_t * atiny_params = &this->prv_atiny_params;
  atiny_security_param_t  *security_param = &(atiny_params->security_params[0]);
  void *handle = this->prv_handle;

  std::cout<<"init for TestConnection\n";
  device_info->endpoint_name = "xxxxxxxx";
  device_info->manufacturer = "test";

  atiny_params->server_params.binding = "UQS";
  atiny_params->server_params.life_time = 50000;
  atiny_params->server_params.storing = 0;
  atiny_params->server_params.storing_cnt = 0;

  security_param = &(atiny_params->security_params[0]);
  security_param->is_bootstrap = 0;
  security_param->server_ip = "139.159.209.89";
  security_param->server_port = "5683";
  security_param->psk_Id = NULL;
  security_param->psk = NULL;
  security_param->psk_len = 0;
  int ret = atiny_init(atiny_params, &handle);
  TEST_ASSERT((ret == ATINY_OK));
  
}

void TestConnection::tear_down()
{
  std::cout<<"deinit for TestConnection\n";
}



