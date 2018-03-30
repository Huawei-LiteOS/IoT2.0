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
mbedtls_net_context server_fd;


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
	printf("\n\n test dtls_write:");
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
	printf("\n\n test dtls_read:");
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
	use this function to substitute dtls_ssl_destroy 
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
	This function is used to test dtls_ssl_destroy
*/
void TestDtls::test_func_dtls_ssl_destroy()
{
	printf("\n\n test dtls_ssl_destroy:");
	stubInfo si;
	mbedtls_ssl_context ssl;
	
	setStub((void*)dtls_ssl_destroy , (void*)test_dtls_ssl_destroy, &si);
	
	dtls_ssl_destroy(NULL);
	TEST_ASSERT( dtls_ssl_destroy_flag == 0 );
	dtls_ssl_destroy(&ssl);
	TEST_ASSERT( dtls_ssl_destroy_flag == 1 );
	
	dtls_ssl_destroy_flag = 0; //clear dtls_ssl_destroy_flag 
	cleanStub(&si);
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
	This function is used to test dtls_ssl_new_with_psk
*/
void TestDtls::test_func_dtls_ssl_new_with_psk()
{
	printf("\n\n test dtls_ssl_new_with_psk:");
	
	/*testing variables*/
	unsigned char psk[16] = {0xef,0xe8,0x18,0x45,0xa3,0x53,0xc1,0x3c,0x0c,0x89,0x92,0xb3,0x1d,0x6b,0x6a,0x83};
    char *psk_identity = "666003"; 
	mbedtls_ssl_context * p_ssl = NULL;
	
	stubInfo si_0 , si_1 , si_2 , si_3 ;
	
	setStub((void*)mbedtls_ctr_drbg_seed , (void*)test_mbedtls_ctr_drbg_seed, &si_0);
	setStub((void*)dtls_ssl_destroy , (void*)test_dtls_ssl_destroy, &si_1);
	//setStub((void*)mbedtls_ssl_conf_psk , (void*)test_mbedtls_ssl_conf_psk , &si_2);
	//setStub((void*)mbedtls_ssl_setup , (void*)test_mbedtls_ssl_setup , &si_3);
	//setStub((void*)mbedtls_ssl_set_hostname , (void*)test_mbedtls_ssl_set_hostname , &si_4);
	
	/*---------branch control unit------------*/
	{
		mbedtls_ctr_drbg_seed_control_flag = 1;    
		mbedtls_ssl_conf_psk_control_flag = 0;
		mbedtls_ssl_setup_control_flag = 0;
		mbedtls_ssl_set_hostname_control_flag = 0;
	}
	p_ssl = dtls_ssl_new_with_psk((char*)psk , sizeof(psk) , psk_identity);
	TEST_ASSERT( p_ssl == NULL && dtls_ssl_destroy_flag == 1);
	printf("dtls_ssl_destroy_flag is %d\n" , dtls_ssl_destroy_flag);
	dtls_ssl_destroy_flag = 0;
	
	{//all branch OK!!
		mbedtls_ctr_drbg_seed_control_flag = 0;    
		mbedtls_ssl_conf_psk_control_flag = 0;
		mbedtls_ssl_setup_control_flag = 0;
		mbedtls_ssl_set_hostname_control_flag = 0;
	}   
	p_ssl = dtls_ssl_new_with_psk((char*)psk , sizeof(psk) , psk_identity);
	TEST_ASSERT(  p_ssl != NULL && dtls_ssl_destroy_flag == 0 );
	printf("dtls_ssl_destroy_flag is %d\n" , dtls_ssl_destroy_flag);
	
	//
	setStub((void*)mbedtls_ssl_conf_psk , (void*)test_mbedtls_ssl_conf_psk , &si_2);
	setStub((void*)mbedtls_ssl_setup , (void*)test_mbedtls_ssl_setup , &si_3);
	//setStub((void*)mbedtls_ssl_set_hostname , (void*)test_mbedtls_ssl_set_hostname , &si_4);
	
	//
	{
		mbedtls_ctr_drbg_seed_control_flag = 0;    
		mbedtls_ssl_conf_psk_control_flag = 1;
		mbedtls_ssl_setup_control_flag = 0;
		mbedtls_ssl_set_hostname_control_flag = 0;
	}
	p_ssl = dtls_ssl_new_with_psk((char*)psk , sizeof(psk) , psk_identity);
	TEST_ASSERT( p_ssl == NULL && dtls_ssl_destroy_flag == 1);
	printf("dtls_ssl_destroy_flag is %d\n" , dtls_ssl_destroy_flag);
	dtls_ssl_destroy_flag = 0;
	
	//
	{
		mbedtls_ctr_drbg_seed_control_flag = 0;    
		mbedtls_ssl_conf_psk_control_flag = 0;
		mbedtls_ssl_setup_control_flag = 1;
		mbedtls_ssl_set_hostname_control_flag = 0;
	}
	p_ssl = dtls_ssl_new_with_psk((char*)psk , sizeof(psk) , psk_identity);
	TEST_ASSERT( p_ssl == NULL && dtls_ssl_destroy_flag == 1);
	printf("dtls_ssl_destroy_flag is %d\n" , dtls_ssl_destroy_flag);
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
	
	//
	{//reset all flag
		mbedtls_ctr_drbg_seed_control_flag = 0;    
		mbedtls_ssl_conf_psk_control_flag = 0;
		mbedtls_ssl_setup_control_flag = 0;
		mbedtls_ssl_set_hostname_control_flag = 0;
	}
	
	//
	cleanStub(&si_0);
	cleanStub(&si_1);
	cleanStub(&si_2);
	cleanStub(&si_3);
	//cleanStub(&si_4);
	
	
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
		server_fd.fd = 5;
		return &server_fd;
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


void TestDtls::test_func_dtls_shakehand()
{
	printf("\n\n test dtls_shakehand");
	mbedtls_ssl_context ssl;
	const char *host = "coaps://192.168.1.110";
	const char *port = "5684";
	int ret = 0;
	
	stubInfo si_0 ,si_1;
	setStub((void*)mbedtls_net_connect , (void*)test_mbedtls_net_connect, &si_0);
	setStub((void*)mbedtls_ssl_handshake , (void*)test_mbedtls_ssl_handshake , &si_1);
	
	mbedtls_net_connect_control_flag = 0;   //go to wrong branch
	ret = dtls_shakehand(&ssl , host , port);
	//printf("ret is %ld\n",ret);
	TEST_ASSERT( ssl.p_timer == NULL );
	mbedtls_net_connect_control_flag = 0;
	
	mbedtls_net_connect_control_flag = 1; //go to right branch , go on following process
	mbedtls_ssl_handshake_control_flag = 0; //go to right branch
	ret = dtls_shakehand(&ssl , host , port);
	TEST_ASSERT( ret == 0 ); 
	
	mbedtls_net_connect_control_flag = 1; //go to right branch , go on following process
	mbedtls_ssl_handshake_control_flag = 1; //go to wrong branch
	ret = dtls_shakehand(&ssl , host , port);
	//printf("ret is %ld\n",ret);
	TEST_ASSERT( ssl.p_bio == NULL );
	
	cleanStub(&si_0);
	cleanStub(&si_1);
}



/*
	Add all functions to test array
*/
TestDtls::TestDtls()
{
	TEST_ADD(TestDtls::test_func_dtls_write);
	TEST_ADD(TestDtls::test_func_dtls_read);
	TEST_ADD(TestDtls::test_func_dtls_ssl_destroy);
	TEST_ADD(TestDtls::test_func_dtls_ssl_new_with_psk);
	TEST_ADD(TestDtls::test_func_dtls_shakehand);
}



