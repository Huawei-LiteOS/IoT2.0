#include "agent_tiny_example.h"
#include "agenttiny.h"

#define ATINY_POWER_VOLTAGE_MIN 3800
#define ATINY_POWER_VOLTAGE_MAX 5000
#define ATINY_NETWORK_BEARER    5
#define ATINY_SIGNAL_STRENGTH   90
#define ATINY_CELL_ID           21103

int atiny_do_dev_reboot(void)
{
    atiny_printf("device is rebooting......\r\n");
    return ATINY_OK;
}

int atiny_get_min_voltage(int* voltage)
{
    *voltage = ATINY_POWER_VOLTAGE_MIN;
	return ATINY_OK;
}

int atiny_get_max_voltage(int* voltage)
{
    *voltage = ATINY_POWER_VOLTAGE_MAX;
	return ATINY_OK;
}

int atiny_get_firmware_ver(char* version, int len)
{
    atiny_snprintf(version, len, "example_ver001");
    return ATINY_OK;
}

int atiny_trig_firmware_update(void)
{
    atiny_printf("firmware is updating......\r\n");
    return ATINY_OK;
}
int atiny_get_firmware_result(int* result)
{
    *result = 0;
    return ATINY_OK;    
}
int atiny_get_network_bearer(int* network_brearer)
{
    *network_brearer = ATINY_NETWORK_BEARER;
    return ATINY_OK;
}

int atiny_get_signal_strength(int* singal_strength)
{
    *singal_strength = ATINY_SIGNAL_STRENGTH;
    return ATINY_OK;
}

int atiny_get_cell_id(long* cell_id)
{
    *cell_id = ATINY_CELL_ID;
    return ATINY_OK;    
}

int atiny_write_app_write(void* user_data, int len)
{
    atiny_printf("write num19 object success\r\n");
    return ATINY_OK;        
}

int atiny_udate_psk()
{
    atiny_printf("update psk success\r\n");
    return ATINY_OK;        
}
int atiny_cmd_ioctl(atiny_cmd_e cmd, char* arg, int len)
{
    int result = ATINY_OK;
    switch(cmd)
    {
        case ATINY_DO_DEV_REBOOT:
             result = atiny_do_dev_reboot();
            break;
        case ATINY_GET_MIN_VOLTAGE:
            result = atiny_get_min_voltage((int*)arg);
            break;
        case ATINY_GET_MAX_VOLTAGE:
            result = atiny_get_max_voltage((int*)arg);
            break;
        case ATINY_GET_FIRMWARE_VER:
            result = atiny_get_firmware_ver(arg, len);
            break;
        case ATINY_TRIG_FIRMWARE_UPDATE:
            result = atiny_trig_firmware_update();
            break;
        case ATINY_GET_FIRMWARE_STATE:
            break;
        case ATINY_GET_FIRMWARE_RESULT:
            result = atiny_get_firmware_result((int*)arg);
            break;
        case ATINY_GET_NETWORK_BEARER:
            result = atiny_get_network_bearer((int*)arg);
            break;
        case ATINY_GET_SIGNAL_STRENGTH:
            result = atiny_get_signal_strength((int*)arg);
            break;
        case ATINY_GET_CELL_ID:
            result = atiny_get_cell_id((long*)arg);
            break;
        case ATINY_WRITE_APP_DATA:
            result = atiny_write_app_write((int*)arg, len);
            break;
        case ATINY_UDATE_PSK:
            break;
        default:
            break;           
    }
    return result;
}
