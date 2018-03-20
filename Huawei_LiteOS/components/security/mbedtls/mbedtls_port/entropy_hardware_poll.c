#include "atiny_adapter.h"
#include "entropy.h"

int mbedtls_hardware_poll(void *data,
							unsigned char *output,size_t len, size_t *olen);
int mbedtls_hardware_poll(void *data,
							unsigned char *output,size_t len, size_t *olen)
{
    ((void)data);
    *olen = 0;
    if (0 != atiny_random(output, len))
        return MBEDTLS_ERR_ENTROPY_SOURCE_FAILED;
    *olen = len;
    return 0;
}
