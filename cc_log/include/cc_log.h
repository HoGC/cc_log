/*
 * @Author: HoGC
 * @Date: 2022-04-17 02:10:54
 * @Last Modified time: 2022-04-17 02:10:54
 */
#ifndef __CC_LOG_H__
#define __CC_LOG_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <string.h>

#define CC_LOG_NONE                         0
#define CC_LOG_ERROR                        1
#define CC_LOG_WARN                         2
#define CC_LOG_INFO                         3
#define CC_LOG_DEBUG                        4
#define CC_LOG_MAX                          5
typedef uint8_t                             cc_log_level_t;

#ifndef CONFIG_CC_LOG_LOCAL_LEVEL
#define CONFIG_CC_LOG_LOCAL_LEVEL           CC_LOG_DEBUG
#endif

#ifndef CONFIG_CC_LOG_USE_TIME
#define CONFIG_CC_LOG_USE_TIME              1
#endif

#ifndef CONFIG_CC_LOG_USE_COLOR
#define CONFIG_CC_LOG_USE_COLOR             1
#endif

#ifndef CONFIG_CC_LOG_USE_NEW_LINE
#define CONFIG_CC_LOG_USE_NEW_LINE          1
#endif

#ifndef CONFIG_CC_FILE_FUN_INFO_LEVEL
#define CONFIG_CC_FILE_FUN_INFO_LEVEL       CC_LOG_ERROR   
#endif

#ifndef CONFIG_CC_LOG_FILE_LEVEL                                               
#define CONFIG_CC_LOG_FILE_LEVEL            CONFIG_CC_LOG_LOCAL_LEVEL
#endif

#ifndef CONFIG_CC_LOG_USE_FILE_PATH
#define CONFIG_CC_LOG_USE_FILE_PATH         0
#endif

#ifndef CONFIG_CC_LOG_BUF_MAX_SIZE
#define CONFIG_CC_LOG_BUF_MAX_SIZE          256 
#endif

#ifndef CONFIG_CC_LOG_ENABLE_TCPS_LOG
#define CONFIG_CC_LOG_ENABLE_TCPS_LOG       1
#endif

typedef uint64_t (*cc_log_get_time_like_t)(void);
typedef int (*cc_log_put_like_t)(const char *log, uint16_t len);

uint64_t cc_log_get_time(void);
int cc_log_printf(const char *format, ...);
void cc_log_hexdump(const uint8_t* buf, uint16_t len);

void cc_log_set_put(cc_log_put_like_t func);
void cc_log_set_get_time(cc_log_get_time_like_t func);

#if (defined(CONFIG_CC_LOG_ENABLE_TCPS_LOG) && CONFIG_CC_LOG_ENABLE_TCPS_LOG == 1)
int cc_tcps_log_put(const char *log, uint16_t len);
int cc_tcps_log_init(uint16_t port);
#endif

#ifndef CC_LOG_GET_TIME
#define CC_LOG_GET_TIME                     cc_log_get_time()
#endif

#ifndef CC_LOG_PRINTF
#define CC_LOG_PRINTF(format, ...)          cc_log_printf(format, ##__VA_ARGS__)
#endif

#ifndef CC_LOG_RAW_HEXDUMP
#define CC_LOG_RAW_HEXDUMP(buf, len)        cc_log_hexdump(buf, len)
#endif

#if CONFIG_CC_LOG_USE_COLOR
#define CC_LOG_COLOR_BLACK                  "30"
#define CC_LOG_COLOR_RED                    "31"
#define CC_LOG_COLOR_GREEN                  "32"
#define CC_LOG_COLOR_BROWN                  "33"
#define CC_LOG_COLOR_BLUE                   "34"
#define CC_LOG_COLOR_PURPLE                 "35"
#define CC_LOG_COLOR_CYAN                   "36"
#define CC_LOG_COLOR(COLOR)                 "\033[0;" COLOR "m"
#define CC_LOG_BOLD(COLOR)                  "\033[1;" COLOR "m"
#define CC_LOG_RESET_COLOR                  "\033[0m"
#define CC_LOG_COLOR_E                      CC_LOG_COLOR(CC_LOG_COLOR_RED)
#define CC_LOG_COLOR_W                      CC_LOG_COLOR(CC_LOG_COLOR_BROWN)
#define CC_LOG_COLOR_I                      CC_LOG_COLOR(CC_LOG_COLOR_GREEN)
#define CC_LOG_COLOR_D                      
#else
#define CC_LOG_COLOR_E
#define CC_LOG_COLOR_W
#define CC_LOG_COLOR_I
#define CC_LOG_COLOR_D
#define CC_LOG_RESET_COLOR
#endif //CONFIG_CC_LOG_USE_COLOR

#if CONFIG_CC_LOG_USE_NEW_LINE
#define CC_LOG_NEW_LINE                     "\n"
#else
#define CC_LOG_NEW_LINE
#endif //CONFIG_CC_LOG_USE_NEW_LINE

#if CONFIG_CC_LOG_USE_FILE_PATH
#define FILENAME(name)                      name
#else
#define FILENAME(name)                      strrchr(name, '/')?strrchr(name, '/')+1:name
#endif

#if CONFIG_CC_LOG_USE_TIME
#define CC_LOG_FORMAT(level, format)  CC_LOG_COLOR_ ## level "%lld ["#level"] [%12s]: "format CC_LOG_RESET_COLOR CC_LOG_NEW_LINE, CC_LOG_GET_TIME
#define CC_LOG_FILE_FUN_INFO_FORMAT(level, format)  CC_LOG_COLOR_ ## level "%lld ["#level" %s %s:%d] [%12s]: "format CC_LOG_RESET_COLOR CC_LOG_NEW_LINE, CC_LOG_GET_TIME, FILENAME(__FILE__), __FUNCTION__, __LINE__
#define CC_LOG_FILE_FUN_INFO_FORMAT(level, format)  CC_LOG_COLOR_ ## level "%lld ["#level" %s %s:%d] [%12s]: "format CC_LOG_RESET_COLOR CC_LOG_NEW_LINE, CC_LOG_GET_TIME, FILENAME(__FILE__), __FUNCTION__, __LINE__
#else
#define CC_LOG_FORMAT(level, format)  CC_LOG_COLOR_ ## level "["#level"] [%s]: "format CC_LOG_RESET_COLOR CC_LOG_NEW_LINE
#define CC_LOG_FILE_FUN_INFO_FORMAT(level, format)  CC_LOG_COLOR_ ## level "["#level" %s %s:%d] [%12s]: "format CC_LOG_RESET_COLOR CC_LOG_NEW_LINE, FILENAME(__FILE__), __FUNCTION__, __LINE__
#endif //CONFIG_CC_LOG_USE_TIME

#if (CONFIG_CC_LOG_LOCAL_LEVEL >= CC_LOG_ERROR && CONFIG_CC_LOG_FILE_LEVEL >= CC_LOG_ERROR)
    #if (CONFIG_CC_FILE_FUN_INFO_LEVEL >= CC_LOG_ERROR)
        #define CC_LOGE( tag, format, ... ) CC_LOG_PRINTF(CC_LOG_FILE_FUN_INFO_FORMAT(E, format), tag, ##__VA_ARGS__)
    #else
        #define CC_LOGE( tag, format, ... ) CC_LOG_PRINTF(CC_LOG_FORMAT(E, format), tag, ##__VA_ARGS__)
    #endif
#else
    #define CC_LOGE(_fmt_, ...)
#endif

#if (CONFIG_CC_LOG_LOCAL_LEVEL >= CC_LOG_WARN && CONFIG_CC_LOG_FILE_LEVEL >= CC_LOG_WARN)
    #if (CONFIG_CC_FILE_FUN_INFO_LEVEL >= CC_LOG_WARN)
        #define CC_LOGW( tag, format, ... ) CC_LOG_PRINTF(CC_LOG_FILE_FUN_INFO_FORMAT(W, format), tag, ##__VA_ARGS__)
    #else
        #define CC_LOGW( tag, format, ... ) CC_LOG_PRINTF(CC_LOG_FORMAT(W, format), tag, ##__VA_ARGS__)
    #endif
#else
    #define CC_LOGW(_fmt_, ...)
#endif

#if (CONFIG_CC_LOG_LOCAL_LEVEL >= CC_LOG_INFO && CONFIG_CC_LOG_FILE_LEVEL >= CC_LOG_INFO)
    #if (CONFIG_CC_FILE_FUN_INFO_LEVEL >= CC_LOG_INFO)
        #define CC_LOGI( tag, format, ... ) CC_LOG_PRINTF(CC_LOG_FILE_FUN_INFO_FORMAT(I, format), tag, ##__VA_ARGS__)
    #else
        #define CC_LOGI( tag, format, ... ) CC_LOG_PRINTF(CC_LOG_FORMAT(I, format), tag, ##__VA_ARGS__)
    #endif
#else
    #define CC_LOGI(_fmt_, ...)
#endif

#if (CONFIG_CC_LOG_LOCAL_LEVEL >= CC_LOG_DEBUG && CONFIG_CC_LOG_FILE_LEVEL >= CC_LOG_DEBUG)
    #if (CONFIG_CC_FILE_FUN_INFO_LEVEL >= CC_LOG_DEBUG)
        #define CC_LOGD( tag, format, ... ) CC_LOG_PRINTF(CC_LOG_FILE_FUN_INFO_FORMAT(D, format), tag, ##__VA_ARGS__)
    #else
        #define CC_LOGD( tag, format, ... ) CC_LOG_PRINTF(CC_LOG_FORMAT(D, format), tag, ##__VA_ARGS__)
    #endif
#else
    #define CC_LOGD(_fmt_, ...)
#endif

#define CC_LOG_HEXDUMP_FORMAT  "hexdump"
#define CC_LOGE_HEXDUMP(tag, buf, len) CC_LOGE(tag, CC_LOG_HEXDUMP_FORMAT);CC_LOG_RAW_HEXDUMP(buf, len)
#define CC_LOGW_HEXDUMP(tag, buf, len) CC_LOGW(tag, CC_LOG_HEXDUMP_FORMAT);CC_LOG_RAW_HEXDUMP(buf, len)
#define CC_LOGI_HEXDUMP(tag, buf, len) CC_LOGI(tag, CC_LOG_HEXDUMP_FORMAT);CC_LOG_RAW_HEXDUMP(buf, len)
#define CC_LOGD_HEXDUMP(tag, buf, len) CC_LOGD(tag, CC_LOG_HEXDUMP_FORMAT);CC_LOG_RAW_HEXDUMP(buf, len)

#ifdef __cplusplus
}
#endif

#endif // __CC_LOG_H__
