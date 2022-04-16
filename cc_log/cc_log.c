/*
 * @Author: HoGC
 * @Date: 2022-04-17 01:57:59
 * @Last Modified time: 2022-04-17 01:57:59
 */
#include "cc_log.h"

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

static cc_log_put_like_t s_log_put_func = NULL;
static cc_log_get_time_like_t s_log_get_time_func = NULL;

void cc_log_set_put(cc_log_put_like_t func)
{
    s_log_put_func = func;
}

void cc_log_set_get_time(cc_log_get_time_like_t func)
{
    s_log_get_time_func = func;
}

uint64_t cc_log_get_time(void)
{
    uint64_t time = 0;
    if(s_log_get_time_func){
        time = s_log_get_time_func();
    }
    return time;
}

int cc_log_printf(const char *format, ...)
{
    int size = 0; 
    uint16_t log_size = 0; 
    char log_buf[CONFIG_CC_LOG_BUF_MAX_SIZE] = {0};
    va_list args;

    va_start(args, format);
    if(s_log_put_func){
        vsnprintf(log_buf, CONFIG_CC_LOG_BUF_MAX_SIZE, format, args); 
		log_size = strlen(log_buf);
        size = (*s_log_put_func)(log_buf, log_size);
    }else{
        size = vprintf(format, args);
    }
    va_end(args);
	return size;
}


#define width 16

void cc_log_hexdump(const uint8_t* buf, uint16_t len)
{

	if (len < 1 || buf == NULL) return;
 
	const char *hex_char_ts = "0123456789ABCDEF";
	int i = 0;
	char c = 0x00;
	char str_print_able[width+1];
	char str_hex_buffer[width * 3 + 1];

	for (i = 0; i < (len / width) * width; i += width)
	{
		int j = 0;
		for (j = 0; j < width; j++)
		{
			c = buf[i + j];
 
			// hex
			int z = j * 3;
			str_hex_buffer[z++] = hex_char_ts[(c >> 4) & 0x0F];
			str_hex_buffer[z++] = hex_char_ts[c & 0x0F];
			// str_hex_buffer[z++] = (j < 10 && !((j + 1) % 8)) ? '_' : ' ';
			str_hex_buffer[z++] = ' ';
 
			// string with space repalced
			if (c < 32 || c == '\0' || c == '\t' || c == '\r' || c == '\n' || c == '\b')
				str_print_able[j] = '.';
			else
				str_print_able[j] = c;
		}
		str_hex_buffer[width * 3] = 0x00;
		str_print_able[j] = 0x00;

        CC_LOG_PRINTF("       %04x %02d   %s %s  \n", i, width, str_hex_buffer, str_print_able);
	}
 
	// 处理剩下的不够16字节长度的部分
	int leftSize = len % width;
	if (leftSize < 1) return;
	int j = 0;
	int pos = i;
	for (; i < len; i++)
	{
		c = buf[i];
 
		// hex
		int z = j * 3;
		str_hex_buffer[z++] = hex_char_ts[(c >> 4) & 0x0F];
		str_hex_buffer[z++] = hex_char_ts[c & 0x0F];
		str_hex_buffer[z++] = ' ';
 
		// string with space repalced
		if (c < 32 || c == '\0' || c == '\t' || c == '\r' || c == '\n' || c == '\b')
			str_print_able[j] = '.';
		else
			str_print_able[j] = c;
		j++;
	}
	str_hex_buffer[leftSize * 3] = 0x00;
	str_print_able[j] = 0x00;
 
	for (j = leftSize; j < width; j++)
	{
		int z = j * 3;
		str_hex_buffer[z++] = ' ';
		str_hex_buffer[z++] = ' ';
		str_hex_buffer[z++] = ' ';
	}
	str_hex_buffer[width * 3] = 0x00;
    CC_LOG_PRINTF("       %04x %02d   %s %s  \n", pos, leftSize, str_hex_buffer, str_print_able);
}