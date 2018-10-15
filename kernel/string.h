#ifndef SLIM_KERNEL_STRING_H
#define SLIM_KERNEL_STRING_H

#include "php_slim.h"

#include <Zend/zend_smart_str.h>

#ifdef SLIM_USE_PHP_JSON
# include <ext/json/php_json.h>
#endif

#define slim_smart_str_appendl(dest, src, len) \
    smart_str_appendl_ex((dest), (src), (len), 0)

void slim_append_printable_zval(smart_str *implstr, zval *tmp);

int slim_memnstr_str(const zval *haystack, char *needle, unsigned int needle_length);
int slim_memnstr_str_str(const char *haystack, unsigned int haystack_length, char *needle, unsigned int needle_length);
void slim_fast_explode(zval *result, zval *delimiter, zval *str);
void slim_fast_explode_str(zval *result, const char *delimiter, unsigned int delimiter_length, zval *str);
void slim_fast_explode_str_str(zval *result, const char *delimiter, unsigned int delimiter_length, const char *str, unsigned int str_length);
void slim_strtolower_inplace(zval *s);

int slim_comparestr(const zval *str, const zval *compared, zval *case_sensitive);

int slim_json_encode(zval *return_value, zval *v, int opts);
int slim_json_decode(zval *return_value, zval *v, zend_bool assoc);

void slim_substr(zval *return_value, zval *str, unsigned long from, long length);

void slim_camelize(zval *return_value, const zval *str);
void slim_uncamelize(zval *return_value, const zval *str);

int slim_start_with(const zval *str, const zval *compared, zval *case_sensitive);
int slim_start_with_str(const zval *str, char *compared, unsigned int compared_length);

int slim_preg_match(zval *return_value, zval *regex, zval *subject, zval *matches, zend_long flags, int global);

#endif
