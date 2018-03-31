#ifndef _TEST_DTLS_H_
#define _TEST_DTLS_H_
#include <cpptest.h>
#include "dtls_interface.h"            //a must
#include "test_dtls_adapter_layer.h"
#include "stub.h"
#include "net_sockets.h"




/*-------------dtls_write() function used--------------*/
#define BUF_MIN_LEN 1
#define BUF_MAX_LEN 1500
typedef enum test_dtls_write_error
{
	TEST_SSL_NULL_W      = 0x01,
	TEST_BUF_NULL_W      = 0x02,
	TEST_LEN_BELOW_MIN_W = 0x04, 
	TEST_LEN_UP_MAX_W    = 0x08
}dtls_write_error;
/*-------------dtls_write() function used--------------*/



/*-------------dtls_read() function used---------------*/
#define TIMEOUT_MIN_R 0x00000001
#define TIMEOUT_MAX_R 0x1fffffff
typedef enum test_dtls_read_error
{
	TEST_SSL_NULL_R		 	 = 0x01,
	TEST_BUF_NULL_R      	 = 0x02,
	TEST_LEN_BELOW_MIN_R 	 = 0x04,
	TEST_LEN_ABOVE_MAX_R 	 = 0x08,
	TEST_TIMEOUT_BELOW_MIN_R = 0x10,
	TEST_TIMEOUT_ABOVE_MAX_R = 0x20
	
}dtls_read_error;
/*------------ dtls_read() function used---------------*/




/*------------test net_sockets.c use----------*/
#define MBEDTLS_NET_PROTO_UDP 1
/*------------test net_sockets.c use----------*/


class TestDtls: public Test::Suite
{
	protected:
		void tear_down();
	
	
	public:
		void test_func_dtls_write();
		void test_func_dtls_read();
		void test_func_dtls_ssl_destroy();
		void test_func_dtls_ssl_new_with_psk();
		void test_func_dtls_shakehand();
		void test_func_mbedtls_net_connect();
		void test_func_mbedtls_net_usleep();
		void test_func_mbedtls_net_recv();
		void test_func_mbedtls_net_recv_timeout();
		void test_func_mbedtls_net_send();
		void test_func_mbedtls_net_free();
		
		TestDtls();
};

#endif

