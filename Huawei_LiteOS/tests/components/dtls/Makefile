
CC=gcc
CPP=g++
LD=ld
AR=ar

FOR32 := true
BIN:= bin
#LIB:= libagenttiny.a
LIB:= libdtls.a          

HW_LITEOSDIR:=../../..
ifeq ($(FOR32), true)
CFLAGS := -m32  -march=i386 -mtune=i386
else
CFLAGS := 
endif
CFLAGS += -DSTM32F429_439xx -DUSE_STDPERIPH_DRIVER -DUSE_MBED_TLS -DMBEDTLS_CONFIG_FILE="<los_mbedtls_config.h>" -DWITH_LWIP -DLWM2M_LITTLE_ENDIAN -DLWM2M_CLIENT_MODE -DNDEBUG
CFLAGS += -DLWIP_TIMEVAL_PRIVATE=0 -DBYTE_ORDER
CFLAGS += -fprofile-arcs -ftest-coverage

#compile dynamic library of agenttiny.c

SRC:= $(wildcard $(HW_LITEOSDIR)/components/security/mbedtls/mbedtls_port/*.c)
SRC+= $(wildcard $(HW_LITEOSDIR)/components/security/mbedtls/mbedtls-2.6.0/library/*.c)
SRC+= $(HW_LITEOSDIR)/tests/stub.c
#$(wildcard *.c, $(HW_LITEOSDIR)/components/connectivity/agent_tiny/lwm2m_client/*.c)
#SRC+= $(HW_LITEOSDIR)/components/connectivity/agent_tiny/examples/agent_tiny_example.c
#SRC+= $(wildcard $(HW_LITEOSDIR)/components/connectivity/lwm2m/core/*.c)
#SRC+= $(wildcard $(HW_LITEOSDIR)/components/connectivity/lwm2m/core/er-coap-13/*.c)
#SRC+= $(wildcard $(HW_LITEOSDIR)/components/connectivity/agent_tiny/osadapter/*.c) 


LIB_OBJ:=$(patsubst %.c, %.o, $(SRC))
INCLUDES:= -I$(HW_LITEOSDIR)/components/security/mbedtls/mbedtls_port/ \
		   -I$(HW_LITEOSDIR)/components/security/mbedtls/mbedtls-2.6.0/include/ \
		   -I$(HW_LITEOSDIR)/components/connectivity/agent_tiny/osadapter/ \
		   -I$(HW_LITEOSDIR)/tests \
		   -I$(HW_LITEOSDIR)/components/dtls \
		   -I$(HW_LITEOSDIR)/components/security/mbedtls/mbedtls-2.6.0/include/mbedtls
	#-I$(HW_LITEOSDIR)/components/connectivity/lwm2m/core/ \
    #-I$(HW_LITEOSDIR)/components/connectivity/lwm2m/core/er-coap-13 \
	#-I$(HW_LITEOSDIR)/components/connectivity/agent_tiny/osadapter/ \
	#-I$(HW_LITEOSDIR)/components/connectivity/agent_tiny/comm/include/ \
	#-I$(HW_LITEOSDIR)/kernel/include/ \
	#-I$(HW_LITEOSDIR)/targets/STM32F429IGTX_FIRE/OS_CONFIG/ \
	#-I$(HW_LITEOSDIR)/targets/STM32F429IGTX_FIRE/Inc/ \
	#-I$(HW_LITEOSDIR)/components/security/mbedtls/mbedtls_port/ \
	#-I$(HW_LITEOSDIR)/drivers/bsp/stm32/STM32F4xx_StdPeriph_Driver/inc/ \
	#-I$(HW_LITEOSDIR)/kernel/base/include/ \
	#-I$(HW_LITEOSDIR)/kernel/compat/cmsis/ \
	#-I$(HW_LITEOSDIR)/tests/Include \
	#-I$(HW_LITEOSDIR)/tests\
	#-I$(HW_LITEOSDIR)/components/connectivity/agent_tiny/lwm2m_client/ \
	#-I$(HW_LITEOSDIR)/arch/arm/cortex-m4/ \
	#-I$(HW_LITEOSDIR)/drivers/boards/STM32F429IGTX_FIRE/dwt/ \
	#-I$(HW_LITEOSDIR)/components/net/lwip-2.0.3/src/include/ \
	#-I$(HW_LITEOSDIR)/components/net/lwip_port/ \
	#-I$(HW_LITEOSDIR)/drivers/boards/STM32F429IGTX_FIRE/usart/ \
	#-I$(HW_LITEOSDIR)/components/security/mbedtls/mbedtls-2.6.0/include/ \
    #-I$(HW_LITEOSDIR)/components/connectivity/agent_tiny/examples/ \


CPP_OBJ:=main.o test_dtls.o 
#test_agenttiny.o 
C_OBJ:= test_dtls_adapter_layer.o
#adapter_layer.o 
all:lib $(BIN)
lib:$(LIB)

TEST_GCDA:=$(patsubst %.o, %.gcda, $(CPP_OBJ))
TEST_GCDA+=$(patsubst %.o, %.gcda, $(C_OBJ))
TEST_GCDA+=$(patsubst %.o, %.gcda, $(LIB_OBJ))

TEST_GCNO:=$(patsubst %.o, %.gcno, $(CPP_OBJ))
TEST_GCNO+=$(patsubst %.o, %.gcno, $(C_OBJ))
TEST_GCNO+=$(patsubst %.o, %.gcno, $(LIB_OBJ))

$(BIN):$(C_OBJ) $(CPP_OBJ)
	$(CPP) $(CFLAGS) $(INCLUDES) -o $@ $^ -ldtls -L. -lcpptest -lpthread

$(CPP_OBJ):%.o:%.cpp
	$(CPP) -c $< $(INCLUDES) $(CFLAGS) -o $@

$(C_OBJ):%.o:%.c
	$(CC) -c $< $(INCLUDES) $(CFLAGS) -o $@

$(LIB):$(LIB_OBJ)
#	$(CPP) -shared -o $@ $^
	$(AR) -r $@ $^

$(LIB_OBJ):%.o:%.c
	$(CC) $(CFLAGS)	$(INCLUDES) -c $< -o $@

clean:
	rm -rf *.so *.o *.a $(LIB_OBJ) $(C_OBJ) $(CPP_OBJ) *.htm bin $(TEST_GCDA) $(TEST_GCNO) gcov

run:
	./bin
	
	
cov:
	mkdir -p gcov
	./$(BIN)
	lcov -c -o gcov/gcov.info -d $(HW_LITEOSDIR)/components/security  -b . --rc lcov_branch_coverage=1
	genhtml -o gcov/html gcov/gcov.info --rc genhtml_branch_coverage=1
