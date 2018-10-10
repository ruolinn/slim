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

#ifdef SLIM_USE_PHP_JSON
#include <ext/json/php_json.h>
#endif

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

int slim_json_encode(zval *retval, zval *v, int opts)
{
    int flag;

#ifdef SLIM_USE_PHP_JSON
    smart_str buf = {0};
# if PHP_VERSION_ID >= 70100
    flag = php_json_encode(&buf, v, opts);
# else
    JSON_G(error_code) = PHP_JSON_ERROR_NONE;
    php_json_encode(&buf, v, opts);
    if (JSON_G(error_code) != PHP_JSON_ERROR_NONE && !(opts & PHP_JSON_PARTIAL_OUTPUT_ON_ERROR)) {
        flag = FAILURE;
    } else {
        flag = SUCCESS;
    }
# endif
    if (flag == SUCCESS) {
        smart_str_0(&buf); /* copy? */
        if (buf.s) {
            ZVAL_NEW_STR(retval, buf.s);
            return flag;
        }
        ZVAL_EMPTY_STRING(retval);
        return flag;
    } else {
        smart_str_free(&buf);
        ZVAL_FALSE(retval);
    }
#else
    zval zopts = {};
    ZVAL_LONG(&zopts, opts);
    SLIM_CALL_FUNCTION_FLAG(flag, retval, "json_encode", v, &zopts);
#endif
    return flag;
}

void slim_append_printable_zval(smart_str *implstr, zval *tmp)
{
    zval tmp_val = {};
    unsigned int str_len;

    switch (Z_TYPE_P(tmp)) {
		case IS_STRING:
        smart_str_appendl(implstr, Z_STRVAL_P(tmp), Z_STRLEN_P(tmp));
        break;

		case IS_LONG:
        smart_str_append_long(implstr, Z_LVAL_P(tmp));
        break;

		case IS_TRUE:
        smart_str_appendl(implstr, "1", sizeof("1") - 1);
        break;

		case IS_FALSE:
        break;

		case IS_NULL:
        break;

		case IS_DOUBLE: {
        char *stmp;
        str_len = spprintf(&stmp, 0, "%.*G", (int) EG(precision), Z_DVAL_P(tmp));
        smart_str_appendl(implstr, stmp, str_len);
        efree(stmp);
        break;
		}

		case IS_OBJECT: {
        zval expr = {};
        int copy = zend_make_printable_zval(tmp, &expr);
        smart_str_appendl(implstr, Z_STRVAL(expr), Z_STRLEN(expr));
        if (copy) {
            zval_ptr_dtor(&expr);
        }
        break;
		}

		default:
        ZVAL_DUP(&tmp_val, tmp);
        convert_to_string(&tmp_val);
        smart_str_append(implstr, Z_STR(tmp_val));
        break;
    }
}

int slim_memnstr_str(const zval *haystack, char *needle, unsigned int needle_length)
{

    if (Z_TYPE_P(haystack) != IS_STRING) {
        zend_error(E_WARNING, "Invalid arguments supplied for memnstr()");
        return 0;
    }

    if ((uint)(Z_STRLEN_P(haystack)) >= needle_length) {
        return php_memnstr(Z_STRVAL_P(haystack), needle, needle_length, Z_STRVAL_P(haystack) + Z_STRLEN_P(haystack)) ? 1 : 0;
    }

    return 0;
}
