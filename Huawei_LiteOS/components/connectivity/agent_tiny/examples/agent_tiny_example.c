#include "agent_tiny_example.h"
#include "agenttiny.h"

#define ATINY_POWER_VOLTAGE_MIN 3800
#define ATINY_POWER_VOLTAGE_MAX 5000
#define ATINY_NETWORK_BEARER    5
#define ATINY_SIGNAL_STRENGTH   90
#define ATINY_CELL_ID           21103
#define ATINY_LINK_QUALITY      98
#define ATINY_LINK_UTRILIZATION 10
#define ATINY_POWER_SOURCE_1    1
#define ATINY_POWER_SOURCE_2    5
#define ATINY_POWER_CURRENT_1   125
#define ATINY_POWER_CURRENT_2   900

int atiny_get_bind_mode(char* mode,int len)
{
    atiny_printf("bind type is UQS......\r\n");
	atiny_snprintf(mode, len, "UQS");
    return ATINY_OK;
}
int atiny_get_power_current_1(int* arg)
{
	*arg = ATINY_POWER_CURRENT_1;
    return ATINY_OK;
}
int atiny_get_power_current_2(int* arg)
{
	*arg = ATINY_POWER_CURRENT_2;
    return ATINY_OK;
}

int atiny_get_power_source_1(int* arg)
{
	*arg = ATINY_POWER_SOURCE_1;
    return ATINY_OK;
}
int atiny_get_power_source_2(int* arg)
{
	*arg = ATINY_POWER_SOURCE_2;
    return ATINY_OK;
}

int atiny_get_dev_err(int* arg)
{
	*arg = ATINY_OK;
    return ATINY_OK;
}

int atiny_get_model_mode(char* mode,int len)
{
    atiny_snprintf(mode, len, "Lightweight M2M Client");
    return ATINY_OK;
}

int atiny_do_dev_reboot(void)
{
    atiny_printf("device is rebooting......\r\n");
    return ATINY_OK;
}

int atiny_do_factory_reset(void)
{
    atiny_printf("\n\t FACTORY RESET\r\n\n");
    return ATINY_OK;
}

int atiny_get_serial_number(char* num,int len)
{
	atiny_snprintf(num, len, "345000123");
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

int atiny_get_baterry_level(int* voltage)
{
    *voltage = ATINY_POWER_VOLTAGE_MAX;
	return ATINY_OK;
}

int atiny_get_memory_free(int* voltage)
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

int atiny_get_firmware_state(int* state)
{
    *state = 0;
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

int atiny_get_link_quality(int* quality)
{
    *quality = ATINY_LINK_QUALITY;
    return ATINY_OK;    
}

int atiny_get_link_utilization(int* utilization)
{
    *utilization = ATINY_LINK_UTRILIZATION;
    return ATINY_OK;    
}

int atiny_write_app_write(void* user_data, int len)
{
    atiny_printf("write num19 object success\r\n");
    return ATINY_OK;        
}

int atiny_update_psk(char* psk_id, int len)
{
	//memcpy_s(g_psk_value,psk_id,len,16);
    atiny_printf("update psk success\r\n");
    return ATINY_OK;        
}
int atiny_cmd_ioctl(atiny_cmd_e cmd, char* arg, int len)
{
    int result = ATINY_OK;
    switch(cmd)
    {
		case ATINY_GET_BINDING_MODES:
		 	result = atiny_get_bind_mode(arg, len);
			break;
		case ATINY_GET_MODEL_NUMBER:
			result = atiny_get_model_mode(arg, len);
			break;
        case ATINY_DO_DEV_REBOOT:
             result = atiny_do_dev_reboot();
            break;
        case ATINY_GET_MIN_VOLTAGE:
            result = atiny_get_min_voltage((int*)arg);
            break;
        case ATINY_GET_MAX_VOLTAGE:
            result = atiny_get_max_voltage((int*)arg);
            break;
		
		case ATINY_GET_BATERRY_LEVEL:
			result = atiny_get_baterry_level((int*)arg);
            break;
   		case ATINY_GET_MEMORY_FREE:
			result = atiny_get_memory_free((int*)arg);
            break;
		case ATINY_GET_DEV_ERR:
			result = atiny_get_dev_err((int*)arg);
            break;
		case ATINY_GET_POWER_CURRENT_1:
			result = atiny_get_power_current_1((int*)arg);
            break;
   		case ATINY_GET_POWER_CURRENT_2:
			result = atiny_get_power_current_2((int*)arg);
            break;
   		case ATINY_GET_POWER_SOURCE_1:
			result = atiny_get_power_source_1((int*)arg);
            break;
   		case ATINY_GET_POWER_SOURCE_2:
			result = atiny_get_power_source_2((int*)arg);
            break;
   		case ATINY_DO_FACTORY_RESET:
			result = atiny_do_factory_reset();
            break;
		case ATINY_GET_SERIAL_NUMBER:
			result = atiny_get_serial_number(arg, len);
            break;
        case ATINY_GET_FIRMWARE_VER:
            result = atiny_get_firmware_ver(arg, len);
            break;
        case ATINY_TRIG_FIRMWARE_UPDATE:
            result = atiny_trig_firmware_update();
            break;
        case ATINY_GET_FIRMWARE_STATE:
			result = atiny_get_firmware_state((int*)arg);
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
		case ATINY_GET_LINK_QUALITY:
			result = atiny_get_link_quality((int*)arg);
            break;
   		case ATINY_GET_LINK_UTILIZATION:
			result = atiny_get_link_utilization((int*)arg);
            break;
        case ATINY_WRITE_APP_DATA:
            result = atiny_write_app_write((int*)arg, len);
            break;
        case ATINY_UPDATE_PSK:
            result = atiny_update_psk(arg, len);
            break;
        default:
            break;           
    }
    return result;
}
