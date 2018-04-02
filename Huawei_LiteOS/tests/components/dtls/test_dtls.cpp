#include<cpptest.h>
#include<iostream>
#include<fstream>
#include<memory>
#include "test_dtls.h"

unsigned int dtls_ssl_destroy_flag =0 ;
unsigned int mbedtls_ctr_drbg_seed_control_flag = 0;
unsigned int mbedtls_net_connect_control_flag = 0;
unsigned int mbedtls_ssl_handshake_control_flag = 0;
unsigned int mbedtls_ssl_conf_psk_control_flag = 0;
unsigned int mbedtls_ssl_setup_control_flag = 0;
unsigned int mbedtls_ssl_set_hostname_control_flag = 0;
unsigned int mbedtls_ssl_config_defaults_control_flag = 0;
mbedtls_net_context *server_fd =(mbedtls_net_context *) malloc(sizeof(mbedtls_net_context));
mbedtls_ssl_context * p_ssl_g = NULL;


/*
	hook test_dtls_write with mbedtls_ssl_write
*/
int test_dtls_write(mbedtls_ssl_context *ssl, const unsigned char *buf, size_t len)
{
	int retCode = 0;
	if(ssl == NULL)
	{
		retCode |= TEST_SSL_NULL_W;
	}
	
	if(buf == NULL)
	{
		retCode |= TEST_BUF_NULL_W;
	}
	
	if(len < BUF_MIN_LEN )
	{
		retCode |= TEST_LEN_BELOW_MIN_W;
	}
	
	if(len > BUF_MAX_LEN)
	{
		
		retCode |= TEST_LEN_UP_MAX_W;
	}
	
	return retCode;
}

/*
	This function is used to test dtls_write function which is defined in 
	dtls_interface.c
*/
void TestDtls::test_func_dtls_write()
{
	printf("test dtls_write:\n");
	int ret = 0;
	mbedtls_ssl_context ssl;
	unsigned char buf[BUF_MAX_LEN + 1];
	
	stubInfo si;
	
	setStub((void*)mbedtls_ssl_write, (void*)test_dtls_write, &si);  
	//len is below BUF_MIN_LEN(1)
	ret = dtls_write(NULL , NULL , 0);
	TEST_ASSERT( ret == 0x07 );
	//len is greater than BUF_MAX_LEN(1500)
	ret = dtls_write(NULL , NULL , 1501);
	TEST_ASSERT(ret == 0x0B);
	//ssl para is NULL 
	ret = dtls_write(NULL , buf , 100);
	TEST_ASSERT(ret == 0x01);
	//buf para is NULL
	ret = dtls_write(&ssl , NULL , 100);
	TEST_ASSERT(ret == 0x02);
	
    cleanStub(&si);
}




/*
	hook test_dtls_read with dtls_read
*/
int  test_dtls_read( mbedtls_ssl_context *ssl, unsigned char *buf, size_t len)
{
	int ret = 0;
	if(ssl == NULL)
		ret |= TEST_SSL_NULL_R;
	if(buf == NULL)
		ret |= TEST_BUF_NULL_R;
	if(len < BUF_MIN_LEN)
		ret |= TEST_LEN_BELOW_MIN_R;
	if(len > BUF_MAX_LEN)
		ret |= TEST_LEN_ABOVE_MAX_R;
	/*
	if(timeout < TIMEOUT_MIN_R)
		ret |= TEST_TIMEOUT_BELOW_MIN_R;
	if(timeout > TIMEOUT_MAX_R)
		ret |= TEST_TIMEOUT_ABOVE_MAX_R;
	*/
	return ret;
}


/*
	This function is used to test dtls_read function which is defined in 
	dtls_function.c
*/
void TestDtls::test_func_dtls_read()
{
	printf("test dtls_read:\n");
	int ret = 0;
	mbedtls_ssl_context  ssl;
	mbedtls_ssl_config   conf;
	unsigned char buf[BUF_MAX_LEN + 1];
	
	ssl.conf = &conf;
	
	stubInfo si;   
	setStub((void*)mbedtls_ssl_read, (void*)test_dtls_read, &si); //
	
	//ret = dtls_read(NULL , buf , 100 , 1000);
	//TEST_ASSERT(ret == 0x01);
	
	ret = dtls_read(&ssl , NULL , 100 ,1000);
	TEST_ASSERT( ret == 0x02);
	
	ret = dtls_read(&ssl , buf , 0 , 0);
	TEST_ASSERT( ret == 0x04 );
	
	ret = dtls_read(&ssl , NULL , 0 , 0);
	TEST_ASSERT( ret == 0x06 );
	
	cleanStub(&si);  //
}



/*
	use this function to substitute dtls_ssl_destroy in functions to be tested other than  dtls_ssl_destroy
*/
void test_dtls_ssl_destroy(mbedtls_ssl_context *ssl)
{
	mbedtls_ssl_config           *conf = NULL;
    mbedtls_ctr_drbg_context     *ctr_drbg = NULL;
    mbedtls_entropy_context      *entropy = NULL;
    mbedtls_net_context          *server_fd = NULL;
    mbedtls_timing_delay_context *timer = NULL;
	
	if(ssl == NULL)
	{
		dtls_ssl_destroy_flag = 0;  //dtls_ssl_destroy_flag is a global variable
		//printf("ssl is null\n");
		return;
	}
	else
	{
		dtls_ssl_destroy_flag = 1;
		//printf("ssl is not null\n");
		//mbedtls_free(ssl);
		//free(ssl);
		return; 
	}
	
}


/*
	The next two functions is for test_func_dtls_ssl_destroy use only
*/

void * test_calloc_func( size_t m , size_t n )
{
	return malloc(m*n);
	return NULL;
}
                              
void test_free_func( void * ptr )
{
	if(ptr != NULL) free(ptr);
	return;
}

/*
	next three functions are stub functions
*/
void test_mbedtls_ssl_config_free( mbedtls_ssl_config *conf )
{
	return;
}

void test_mbedtls_ctr_drbg_free( mbedtls_ctr_drbg_context *ctx )
{
	return;
}

void test_mbedtls_entropy_free( mbedtls_entropy_context *ctx )
{
	return;
}

void test_mbedtls_ssl_free( mbedtls_ssl_context *ssl )
{
	return;	
}

/*
	This function is used to test dtls_ssl_destroy
*/
void TestDtls::test_func_dtls_ssl_destroy()
{
	printf("test dtls_ssl_destroy:\n");
	stubInfo si_0 , si_1 , si_2 , si_3;
	int ret = 0;
	
	mbedtls_ctr_drbg_context * ctrg = (mbedtls_ctr_drbg_context*)malloc(sizeof(mbedtls_ctr_drbg_context));
	memset(ctrg , 0 , sizeof(mbedtls_ctr_drbg_context));
	
	mbedtls_ssl_context * ssl = (mbedtls_ssl_context*)malloc(sizeof(mbedtls_ssl_context));
	memset(ssl , 0 , sizeof(mbedtls_ssl_context));
	
	mbedtls_ssl_config *conf = (mbedtls_ssl_config*)malloc(sizeof(mbedtls_ssl_config));
	memset(conf , 0 , sizeof(mbedtls_ssl_config));
	
	mbedtls_timing_delay_context *p_timer = (mbedtls_timing_delay_context *)malloc(sizeof(mbedtls_timing_delay_context));
	memset(p_timer , 0 , sizeof(mbedtls_timing_delay_context));
	
	mbedtls_entropy_context * entropy = (mbedtls_entropy_context*)malloc(sizeof(mbedtls_entropy_context));
	memset(entropy , 0 , sizeof(mbedtls_entropy_context));
	
	mbedtls_net_context * p_bio = (mbedtls_net_context*)malloc(sizeof(mbedtls_net_context));
	memset(p_bio , 0 , sizeof(mbedtls_net_context));
	
	mbedtls_platform_set_calloc_free(test_calloc_func , test_free_func);
	setStub((void*)mbedtls_ssl_config_free, (void*)test_mbedtls_ssl_config_free, &si_0);
	setStub((void*)mbedtls_ctr_drbg_free, (void*)test_mbedtls_ctr_drbg_free, &si_1);
	setStub((void*)mbedtls_entropy_free, (void*)test_mbedtls_entropy_free, &si_2);
	setStub((void*)mbedtls_ssl_free , (void*)test_mbedtls_ssl_free , &si_3);
	
	ssl->conf = NULL;
	ssl->p_bio = NULL;
	ssl->p_timer = NULL;
	
	dtls_ssl_destroy(NULL);
	TEST_ASSERT(ret == 0);
	
	printf("AAAAAAAAAAAAA\n");
	
	dtls_ssl_destroy(ssl);
	TEST_ASSERT(ssl != NULL);
	
	printf("BBBBBBBBBBB\n");
	
	/*
	ctrg->p_entropy = entropy;
	conf->p_rng = ctrg;
	ssl->conf = conf;
	ssl->p_bio = p_bio;
	ssl->p_timer = p_timer;
	dtls_ssl_destroy(ssl);
	TEST_ASSERT(ssl != NULL);
	*/
	
	dtls_ssl_destroy(p_ssl_g);
	TEST_ASSERT(ssl != NULL);
	
	printf("CCCCCCCCCCCCCC\n");
	
	cleanStub(&si_0);
	cleanStub(&si_1);
	cleanStub(&si_2);
	cleanStub(&si_3);
	printf("DDDDDDDDDDDDDDDD\n");
	
}


/*
	This function is used to substitute mbedtls_ctr_drbg_seed 
*/
int test_mbedtls_ctr_drbg_seed( mbedtls_ctr_drbg_context *ctx,
                           int (*f_entropy)(void *, unsigned char *, size_t),
                           void *p_entropy,
                           const unsigned char *custom,
                           size_t len)
{
		if(mbedtls_ctr_drbg_seed_control_flag == 0)
			return 0;
		else
			return 1;
}


/*
	This function is used to substitute mbedtls_ssl_conf_psk
*/
int test_mbedtls_ssl_conf_psk( mbedtls_ssl_config *conf,
                          const unsigned char *psk, size_t psk_len,
                          const unsigned char *psk_identity, size_t psk_identity_len )
{
		if(mbedtls_ssl_conf_psk_control_flag == 0)
			return 0;
		else
			return 1;
}

/*
	This function is used to substitute mbedtls_ssl_setup 
*/
int test_mbedtls_ssl_setup( mbedtls_ssl_context *ssl,
                       mbedtls_ssl_config *conf )
{
	if(mbedtls_ssl_setup_control_flag  == 0)
		return 0;
	else
		return 1;
}

/*
	This function is used to substitute mbedtls_ssl_set_hostname
*/
int test_mbedtls_ssl_set_hostname( mbedtls_ssl_context *ssl, const char *hostname )
{
	if(mbedtls_ssl_set_hostname_control_flag == 0)
		return 0;
	else
		return 1;
}


/*
		This function is used to substitute mbedtls_ssl_config_defaults 
*/
int test_mbedtls_ssl_config_defaults( mbedtls_ssl_config *conf, int endpoint, int transport, int preset )
{
	if(mbedtls_ssl_config_defaults_control_flag  == 0)
		return 0;
	else
		return 1;
}


/*
	This function is used to test dtls_ssl_new_with_psk
*/
void TestDtls::test_func_dtls_ssl_new_with_psk()
{
	printf("test dtls_ssl_new_with_psk:\n");
	
	/*testing variables*/
	unsigned char psk[16] = {0xef,0xe8,0x18,0x45,0xa3,0x53,0xc1,0x3c,0x0c,0x89,0x92,0xb3,0x1d,0x6b,0x6a,0x83};
    char *psk_identity = "666003"; 
	mbedtls_ssl_context * p_ssl = NULL;
	
	stubInfo si_0 , si_1 , si_2 , si_3 , si_4;
	
	setStub((void*)mbedtls_ctr_drbg_seed , (void*)test_mbedtls_ctr_drbg_seed, &si_0);
	//setStub((void*)dtls_ssl_destroy , (void*)test_dtls_ssl_destroy, &si_1);
	
	
	/*---------branch control unit------------*/
	{
		mbedtls_ctr_drbg_seed_control_flag = 1;    
		mbedtls_ssl_conf_psk_control_flag = 0;
		mbedtls_ssl_setup_control_flag = 0;
		mbedtls_ssl_set_hostname_control_flag = 0;
		mbedtls_ssl_config_defaults_control_flag = 0;
	}
	p_ssl = dtls_ssl_new_with_psk((char*)psk , sizeof(psk) , psk_identity);
	TEST_ASSERT( p_ssl == NULL /*&& dtls_ssl_destroy_flag == 1*/);
	//printf("dtls_ssl_destroy_flag is %d\n" , dtls_ssl_destroy_flag);
	dtls_ssl_destroy_flag = 0;
	
	
	{//all branch OK!!
		mbedtls_ctr_drbg_seed_control_flag = 0;    
		mbedtls_ssl_conf_psk_control_flag = 0;
		mbedtls_ssl_setup_control_flag = 0;
		mbedtls_ssl_set_hostname_control_flag = 0;
		mbedtls_ssl_config_defaults_control_flag = 0;
	}   
	p_ssl_g = dtls_ssl_new_with_psk((char*)psk , sizeof(psk) , psk_identity);
	TEST_ASSERT(  p_ssl_g != NULL && dtls_ssl_destroy_flag == 0 );
	//printf("dtls_ssl_destroy_flag is %d\n" , dtls_ssl_destroy_flag);
	
	//place stub functions
	setStub((void*)mbedtls_ssl_conf_psk , (void*)test_mbedtls_ssl_conf_psk , &si_2);
	setStub((void*)mbedtls_ssl_setup , (void*)test_mbedtls_ssl_setup , &si_3);
	setStub((void*)mbedtls_ssl_config_defaults , (void*)test_mbedtls_ssl_config_defaults , &si_4);
	
	//mbedtls_ssl_conf_psk goes into wrong branch
	{
		mbedtls_ctr_drbg_seed_control_flag = 0;    
		mbedtls_ssl_conf_psk_control_flag = 1;
		mbedtls_ssl_setup_control_flag = 0;
		mbedtls_ssl_set_hostname_control_flag = 0;
		mbedtls_ssl_config_defaults_control_flag = 0;
	}
	p_ssl = dtls_ssl_new_with_psk((char*)psk , sizeof(psk) , psk_identity);
	TEST_ASSERT( p_ssl == NULL /*&& dtls_ssl_destroy_flag == 1*/);
	//printf("dtls_ssl_destroy_flag is %d\n" , dtls_ssl_destroy_flag);
	dtls_ssl_destroy_flag = 0;
	
	//mbedtls_ssl_setup goes into wrong branch
	{
		mbedtls_ctr_drbg_seed_control_flag = 0;    
		mbedtls_ssl_conf_psk_control_flag = 0;
		mbedtls_ssl_setup_control_flag = 1;
		mbedtls_ssl_set_hostname_control_flag = 0;
		mbedtls_ssl_config_defaults_control_flag = 0;
	}
	p_ssl = dtls_ssl_new_with_psk((char*)psk , sizeof(psk) , psk_identity);
	TEST_ASSERT( p_ssl == NULL /*&& dtls_ssl_destroy_flag == 1*/);
	//printf("dtls_ssl_destroy_flag is %d\n" , dtls_ssl_destroy_flag);
	dtls_ssl_destroy_flag = 0;
	
	/*
	{
		mbedtls_ctr_drbg_seed_control_flag = 0;    
		mbedtls_ssl_conf_psk_control_flag = 0;
		mbedtls_ssl_setup_control_flag = 0;
		mbedtls_ssl_set_hostname_control_flag = 1;
	}
	p_ssl = dtls_ssl_new_with_psk((char*)psk , sizeof(psk) , psk_identity);
	TEST_ASSERT( p_ssl == NULL && dtls_ssl_destroy_flag == 1);
	printf("dtls_ssl_destroy_flag is %d\n" , dtls_ssl_destroy_flag);
	dtls_ssl_destroy_flag = 0;
	*/
	
	{
		mbedtls_ctr_drbg_seed_control_flag = 0;    
		mbedtls_ssl_conf_psk_control_flag = 0;
		mbedtls_ssl_setup_control_flag = 0;
		mbedtls_ssl_set_hostname_control_flag = 0;
		mbedtls_ssl_config_defaults_control_flag = 1;
	}
	p_ssl = dtls_ssl_new_with_psk((char*)psk , sizeof(psk) , psk_identity);
	TEST_ASSERT( p_ssl == NULL /*&& dtls_ssl_destroy_flag == 1*/);
	//printf("dtls_ssl_destroy_flag is %d\n" , dtls_ssl_destroy_flag);
	dtls_ssl_destroy_flag = 0;
	
	
	//reset all flag
	{
		mbedtls_ctr_drbg_seed_control_flag = 0;    
		mbedtls_ssl_conf_psk_control_flag = 0;
		mbedtls_ssl_setup_control_flag = 0;
		mbedtls_ssl_set_hostname_control_flag = 0;
	}
	
	//clean all stub
	cleanStub(&si_0);
	//cleanStub(&si_1);
	cleanStub(&si_2);
	cleanStub(&si_3);
	cleanStub(&si_4);
	
	
}



/*
	This function is used to substitute mbedtls_net_connect
*/
void* test_mbedtls_net_connect( const char *host, const char *port, int proto )
{
	if(mbedtls_net_connect_control_flag == 0)
		return NULL;
	else
	{
		server_fd->fd = 5;
		return server_fd;
	}
}

/*
	This function is used to substitute mbedtls_ssl_handshake
*/
int test_mbedtls_ssl_handshake( mbedtls_ssl_context *ssl )
{
	if(mbedtls_ssl_handshake_control_flag == 0)
	{
		return 0;
	}
	else
	{
		return 1;
	}
}



void * test_mal_calloc_func( size_t m , size_t n )
{
	return NULL;
}
                              
void test_mal_free_func( void * ptr )
{
	return;
}


void TestDtls::test_func_dtls_shakehand()
{
	printf("test dtls_shakehand:\n");
	mbedtls_ssl_context * ssl =(mbedtls_ssl_context *)malloc(sizeof(mbedtls_ssl_context));
	const char *host = "coaps://192.168.1.110";
	const char *port = "5684";
	int ret = 0;
	
	stubInfo si_0 ,si_1;
	setStub((void*)mbedtls_net_connect , (void*)test_mbedtls_net_connect, &si_0);
	setStub((void*)mbedtls_ssl_handshake , (void*)test_mbedtls_ssl_handshake , &si_1);
	
	mbedtls_net_connect_control_flag = 0;   //go to wrong branch
	ret = dtls_shakehand(ssl , host , port);
	TEST_ASSERT( ssl->p_timer == NULL );
	mbedtls_net_connect_control_flag = 0;
	
	mbedtls_net_connect_control_flag = 1; //go to right branch , go on following process
	mbedtls_ssl_handshake_control_flag = 0; //go to right branch
	ret = dtls_shakehand(ssl , host , port);
	TEST_ASSERT( ret == 0 ); 
	
	mbedtls_net_connect_control_flag = 1; //go to right branch , go on following process
	mbedtls_ssl_handshake_control_flag = 1; //go to wrong branch
	ret = dtls_shakehand(ssl , host , port);
	TEST_ASSERT( ssl->p_bio == NULL );
	
	mbedtls_platform_set_calloc_free(test_mal_calloc_func , test_mal_free_func);//
	ret = dtls_shakehand(ssl , host , port);
	TEST_ASSERT(ret == -0x7F00);

	cleanStub(&si_0);
	cleanStub(&si_1);
}







/*
	This function is used to test mbedtls_net_connect
*/
void TestDtls::test_func_mbedtls_net_connect()
{
	printf("test mbedtls_net_connect:\n");
	const char *host = "coaps://192.168.1.110";
	const char *port = "5684";
	int proto = MBEDTLS_NET_PROTO_UDP;
	mbedtls_net_context * p_ret = NULL;
	
	p_ret = (mbedtls_net_context *)mbedtls_net_connect(host , port , proto);
	TEST_ASSERT(p_ret == NULL);
}


/*
	This function is used to test mbedtls_net_usleep
*/
void TestDtls::test_func_mbedtls_net_usleep()
{
	printf("test mbedtls_net_usleep:\n");
	unsigned long usec = 10;
	mbedtls_net_usleep(usec);
	TEST_ASSERT(usec == 10);
}


/*
	This function is used to test mbedtls_net_recv
*/
void TestDtls::test_func_mbedtls_net_recv()
{
	printf("test mbedtls_net_recv:\n");
	int ctx;
	unsigned char buf[10];
	int ret , len = 10;
	ret = mbedtls_net_recv(&ctx , buf , len );
	TEST_ASSERT(ret == 0);
	
}


/*
	This function is used to test mbedtls_net_recv_timeout
*/
void TestDtls::test_func_mbedtls_net_recv_timeout()
{
	printf("test mbedtls_net_recv_timeout:\n");
	int ret;
	int ctx , len = 10 , timeout = 10;
	unsigned char buf[10];
	ret = mbedtls_net_recv_timeout(&ctx , buf , len , timeout);
	TEST_ASSERT(ret == 0);
}

/*
	This function is used to test mbedtls_net_send
*/
void TestDtls::test_func_mbedtls_net_send()
{
	printf("test mbedtls_net_send:\n");
	int ret;
	int ctx , len = 10;
	unsigned char buf[10];
	ret = mbedtls_net_send(&ctx , buf , len);
	TEST_ASSERT(ret == 0);
}


/*
	This function is used to test mbedtls_net_free
*/
void TestDtls::test_func_mbedtls_net_free()
{
	printf("test mbedtls_net_free:\n");
	mbedtls_net_context *ctx = (mbedtls_net_context *)malloc(sizeof(mbedtls_net_context));
	mbedtls_net_free(ctx);
	int ret = 0;
	//printf("fd is %d\n" , ctx.fd);
	TEST_ASSERT(ret == 0);
}


/*
	This function is used to test mbedtls_net_init
*/
void TestDtls::test_mbedtls_net_init()
{
	printf("test mbedtls_net_init");
	mbedtls_net_context ctx;
	mbedtls_net_init(&ctx);
	TEST_ASSERT(ctx.fd == -1);
}



/*
	Add all functions to test array
*/
TestDtls::TestDtls()
{
	TEST_ADD(TestDtls::test_func_dtls_write);
	TEST_ADD(TestDtls::test_func_dtls_read);
	
	TEST_ADD(TestDtls::test_func_dtls_ssl_new_with_psk);
	TEST_ADD(TestDtls::test_func_dtls_shakehand);
	TEST_ADD(TestDtls::test_func_dtls_ssl_destroy);
	
	TEST_ADD(TestDtls::test_func_mbedtls_net_connect);
	TEST_ADD(TestDtls::test_func_mbedtls_net_usleep);
	TEST_ADD(TestDtls::test_func_mbedtls_net_recv);
	TEST_ADD(TestDtls::test_func_mbedtls_net_recv_timeout);
	TEST_ADD(TestDtls::test_func_mbedtls_net_send);
	TEST_ADD(TestDtls::test_func_mbedtls_net_free);
	TEST_ADD(TestDtls::test_mbedtls_net_init);
}

void TestDtls::tear_down()
{
	std::cout << "Test Dtls ends\n";
}
