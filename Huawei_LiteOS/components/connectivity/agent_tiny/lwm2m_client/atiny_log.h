#ifndef ATINY_LOG_H
#define ATINY_LOG_H
#include "agenttiny.h"

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))
#endif



#ifdef __cplusplus
extern "C" {
#endif


#define ATINY_LOG(arg...) atiny_log_level(__FUNCTION__, __LINE__, arg)
void atiny_log_level(const char *function, long line_number, atiny_log_e log_level, const char *fmt, ...);

#ifdef __cplusplus
}
#endif

#endif
