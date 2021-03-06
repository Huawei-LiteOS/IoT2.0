#include <cpptest.h>
#include "connection.h"
#include "agenttiny.h"

class TestConnection: public Test::Suite {

public:
    atiny_device_info_t prv_dev_info;
    atiny_param_t prv_atiny_params;
    void * prv_handle;
    lwm2m_object_t *securityObj;

protected:
    void setup();
    void tear_down();

public:
    void test_connection_create();
    void test_lwm2m_connect_server();
    void test_lwm2m_buffer_send();
    void test_lwm2m_session_is_equal();
    void test_lwm2m_close_connection();

    TestConnection();
  
};

