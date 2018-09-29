#ifndef SLIM_KERNEL_STRING_H
#define SLIM_KERNEL_STRING_H

#include "php_slim.h"

int slim_memnstr_str_str(const char *haystack, unsigned int haystack_length, char *needle, unsigned int needle_length);
void slim_fast_explode_str(zval *result, const char *delimiter, unsigned int delimiter_length, zval *str);
void slim_fast_explode_str_str(zval *result, const char *delimiter, unsigned int delimiter_length, const char *str, unsigned int str_length);

int slim_comparestr(const zval *str, const zval *compared, zval *case_sensitive);

#endif
