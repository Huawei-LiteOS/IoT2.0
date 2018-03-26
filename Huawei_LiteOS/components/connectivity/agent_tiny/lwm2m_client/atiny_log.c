#include "atiny_log.h"
#include "agenttiny.h"
#include <string.h>
#include <stdio.h>

static atiny_log_e g_atiny_log_level = LOG_INFO;

void atiny_set_log_level(atiny_log_e level)
{
    g_atiny_log_level = level;    
}

atiny_log_e atiny_get_log_level(void)
{
    return g_atiny_log_level;
}


const char *atiny_get_log_level_name(atiny_log_e log_level){
    const char *aNames[] = {"FATAL",
                            "ERR",
                            "WARNING",
                            "INFO",
                            "DEBUG"};
    if(log_level >= ARRAY_SIZE(aNames)){
        return "UNKOWN";
    }

    return aNames[log_level];
}

void atiny_log_level(const char *function, long line_number, atiny_log_e log_level, const char *fmt, ...)
{
    va_list ap;
    char buf[256];  
    char *pos = buf;
    int leftLen;
    int printLen;

    if((log_level > atiny_get_log_level()))
    {
        return;
    }
       
    pos += snprintf(buf, sizeof(buf), "[%s,%s,%ld] ", atiny_get_log_level_name(log_level), function, line_number);
    va_start(ap, fmt); //??¦Ì??¨¦¡À?2?¨ºy¨¢D¡À¨ª    

    leftLen = sizeof(buf) - ((pos - buf) + 3);
    printLen = vsnprintf(pos, leftLen, fmt, ap);
     
    
    if((printLen) < 0 || (printLen >= leftLen))
    {
        pos = &buf[sizeof(buf) - 3];
    }
    else
    {
        pos += printLen;
    }

    *pos     = '\r';
    *(pos+1) = '\n';
    *(pos+2) = '\0';

    va_end(ap); //¨º¨ª¡¤?¡Á¨º?¡ä
    atiny_log("%s\r\n", buf);
}




