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

void * stub_atiny_net_connect(const char *host, const char *port, int proto)
{
  return NULL;
}

void TestConnection::test_connection_create()
{
  connection_t conn;
  lwm2m_object_t securityObj0;

  securityObj0.instanceList = NULL;
  connection_t * connP = connection_create(&conn, &securityObj0, 0, NULL);
  TEST_ASSERT((connP == NULL));

  connP = NULL;
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

int stub_atiny_net_send1( void *ctx, const unsigned char *buf, size_t len )
{
  //stub func for test lwm2m_buffer_send(), return len
  std::cout<<"return "<<len<< "\n";
  return len;
}


void TestConnection::test_lwm2m_buffer_send()
{
  connection_t conn;
  uint8_t buf[64] = "test str";
  stubInfo si;
  
  uint8_t ret = lwm2m_buffer_send(NULL, NULL, 0, NULL);
  TEST_ASSERT((ret == COAP_500_INTERNAL_SERVER_ERROR));

  setStub((void*)atiny_net_send, (void*)stub_atiny_net_send1, &si);
  
  ret = lwm2m_buffer_send(&conn, buf, ATINY_ERR_INVALID_CONTEXT, NULL);
  TEST_ASSERT((ret == COAP_500_INTERNAL_SERVER_ERROR));

  ret = lwm2m_buffer_send(&conn, buf, COAP_NO_ERROR, NULL);
  TEST_ASSERT((ret == 0));

  cleanStub(&si);

  
  atiny_net_context anc;
  char buf2[64] = {0};

  conn.net_context = &anc;
  anc.fd = open("./demo", O_CREAT|O_RDWR, 0777);
  unlink("./demo");
  ret = lwm2m_buffer_send(&conn, buf, strlen((char*)buf), NULL);

  read(anc.fd, buf2, 64);
  TEST_ASSERT((strncmp((char *)buf, buf2, strlen((char*)buf))));
  close(anc.fd);
}

void TestConnection::test_lwm2m_session_is_equal()
{
  connection_t connA;
  connection_t connB;

  
  bool ret = lwm2m_session_is_equal(NULL, NULL, NULL);
  TEST_ASSERT((ret == true));

  connection_t * p_connB = &connA;
  ret = lwm2m_session_is_equal((void *)&connA, (void*)p_connB, NULL);

  TEST_ASSERT((ret == true));

  ret = lwm2m_session_is_equal((void *)&connA, (void*)&connB, NULL);

  TEST_ASSERT((ret == false));

}
TestConnection::TestConnection(){

  TEST_ADD(TestConnection::test_connection_create);
  TEST_ADD(TestConnection::test_lwm2m_buffer_send);
  TEST_ADD(TestConnection::test_lwm2m_session_is_equal);
}

void TestConnection::setup()
{
  printf("setup in TestConnection\n");
  atiny_device_info_t *device_info = &this->prv_dev_info;
  atiny_param_t * atiny_params = &this->prv_atiny_params;
  atiny_security_param_t  *security_param = &(atiny_params->security_params[0]);
  void *handle = this->prv_handle;

  std::cout<<"init for TestConnection\n";
  device_info->endpoint_name = (char*)"xxxxxxxx";
  device_info->manufacturer = (char*)"test";

  atiny_params->server_params.binding = (char*)"UQS";
  atiny_params->server_params.life_time = 50000;
  atiny_params->server_params.storing = 0;
  atiny_params->server_params.storing_cnt = 0;

  security_param = &(atiny_params->security_params[0]);
  security_param->is_bootstrap = 0;
  security_param->server_ip = (char*)"139.159.209.89";
  security_param->server_port = (char*)"5683";
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



