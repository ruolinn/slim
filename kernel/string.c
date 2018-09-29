#include "kernel/string.h"
#include "kernel/main.h"
#include "kernel/fcall.h"

#include <ctype.h>

#include <Zend/zend_smart_str.h>
#include <ext/standard/php_string.h>
#include <ext/standard/php_rand.h>
#include <ext/standard/php_lcg.h>
#include <ext/standard/php_http.h>
#include <ext/standard/base64.h>
#include <ext/standard/md5.h>
#include <ext/standard/crc32.h>
#include <ext/standard/url.h>
#include <ext/standard/html.h>
#include <ext/date/php_date.h>
#include <ext/pcre/php_pcre.h>

int slim_memnstr_str_str(const char *haystack, unsigned int haystack_length, char *needle, unsigned int needle_length) {

    if (haystack && haystack_length >= needle_length) {
        return php_memnstr(haystack, needle, needle_length, haystack + strlen(haystack)) ? 1 : 0;
    }

    return 0;
}

void slim_fast_explode_str_str(zval *result, const char *delimiter, unsigned int delimiter_length, const char *str, unsigned int str_length)
{
    zend_string *delim, *s;

    delim = zend_string_init(delimiter, delimiter_length, 0);
    s = zend_string_init(str, str_length, 0);

    array_init(result);
    php_explode(delim, s, result, LONG_MAX);
    zend_string_release(s);
    zend_string_release(delim);
}


void slim_fast_explode_str(zval *result, const char *delimiter, unsigned int delimiter_length, zval *str)
{
    zend_string *delim;

    if (unlikely(Z_TYPE_P(str) != IS_STRING)) {
        ZVAL_NULL(result);
        zend_error(E_WARNING, "Invalid arguments supplied for explode()");
        return;
    }

    delim = zend_string_init(delimiter, delimiter_length, 0);

    array_init(result);
    php_explode(delim, Z_STR_P(str), result, LONG_MAX);
    zend_string_release(delim);
}

int slim_comparestr(const zval *str, const zval *compared, zval *case_sensitive){

    if (Z_TYPE_P(str) != IS_STRING || Z_TYPE_P(compared) != IS_STRING) {
        return 0;
    }

    if (!Z_STRLEN_P(compared) || !Z_STRLEN_P(str) || Z_STRLEN_P(compared) != Z_STRLEN_P(str)) {
        return 0;
    }

    if (Z_STRVAL_P(str) == Z_STRVAL_P(compared)) {
        return 1;
    }

    if (!case_sensitive || !zend_is_true(case_sensitive)) {
        return !strcmp(Z_STRVAL_P(str), Z_STRVAL_P(compared));
    }

    return !strcasecmp(Z_STRVAL_P(str), Z_STRVAL_P(compared));
}
