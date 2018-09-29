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
