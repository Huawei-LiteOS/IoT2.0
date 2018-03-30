#ifndef _TEST_OBJECT_FIRMWARE_H_
#define _TEST_OBJECT_FIRMWARE_H_


typedef struct
{
    uint8_t state;
    bool supported;
    uint8_t result;
} firmware_data_t;


class TestObjectFirmware:public Test::Suite {
 protected:
  void tear_down();
  void setup();

 public:
  void test_prv_firmware_read();
  void test_prv_firmware_execute();


  TestObjectFirmware();
};


#endif

