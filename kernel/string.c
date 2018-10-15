#include "kernel/string.h"
#include "kernel/main.h"
#include "kernel/fcall.h"
#include "kernel/operators.h"

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

void slim_substr(zval *return_value, zval *str, unsigned long from, long length)
{
	uint str_len;

	if (Z_TYPE_P(str) != IS_STRING) {

		if (Z_TYPE_P(str) == IS_NULL || SLIM_IS_BOOL(str)) {
			ZVAL_FALSE(return_value);
			return;
		}

		if (Z_TYPE_P(str) == IS_LONG) {
			ZVAL_NULL(return_value);
			return;
		}

		zend_error(E_WARNING, "Invalid arguments supplied for slim_substr()");
		ZVAL_FALSE(return_value);
		return;
	}

	str_len = (uint)(Z_STRLEN_P(str));
	if (str_len < from){
		ZVAL_FALSE(return_value);
		return;
	}

	if (length < 0) {
		length = str_len - from + length;
	} else if (!length || (str_len < length + from)) {
		length = str_len - from;
	}

	if (length){
		ZVAL_STRINGL(return_value, Z_STRVAL_P(str) + from, length);
	} else {
		ZVAL_NULL(return_value);
	}
}

void slim_camelize(zval *return_value, const zval *str)
{
    int i, len;
    smart_str camelize_str = {0};
    char *marker, ch;

    if (unlikely(Z_TYPE_P(str) != IS_STRING)) {
        zend_error(E_WARNING, "Invalid arguments supplied for camelize()");
        RETURN_EMPTY_STRING();
        return;
    }

    marker = Z_STRVAL_P(str);
    len    = Z_STRLEN_P(str);

    for (i = 0; i < len; i++) {
        ch = marker[i];
        if (i == 0 || ch == '-' || ch == '_' || ch == '\\') {
            if (ch == '-' || ch == '_') {
                i++;
            } else if (ch == '\\') {
                smart_str_appendc(&camelize_str, marker[i]);
                i++;
            }

            if (i < len) {
                smart_str_appendc(&camelize_str, toupper(marker[i]));
            }
        } else {
            smart_str_appendc(&camelize_str, tolower(marker[i]));
        }
    }

    smart_str_0(&camelize_str);

    if (camelize_str.s) {
        ZVAL_NEW_STR(return_value, camelize_str.s);
    } else {
        ZVAL_EMPTY_STRING(return_value);
    }

}

void slim_uncamelize(zval *return_value, const zval *str)
{
    int i;
    smart_str uncamelize_str = {0};
    char *marker, ch;

    if (Z_TYPE_P(str) != IS_STRING) {
        zend_error(E_WARNING, "Invalid arguments supplied for camelize()");
        return;
    }

    marker = Z_STRVAL_P(str);
    for (i = 0; i < Z_STRLEN_P(str); i++) {
        ch = *marker;
        if (ch == '\0') {
            break;
        }
        if (ch >= 'A' && ch <= 'Z') {
            if (i > 0) {
                smart_str_appendc(&uncamelize_str, '_');
            }
            smart_str_appendc(&uncamelize_str, (*marker) + 32);
        } else {
            smart_str_appendc(&uncamelize_str, (*marker));
        }
        marker++;
    }
    smart_str_0(&uncamelize_str);

    if (uncamelize_str.s) {
        RETURN_NEW_STR(uncamelize_str.s);
    } else {
        RETURN_EMPTY_STRING();
    }
}

void slim_strtolower_inplace(zval *s)
{
    if (likely(Z_TYPE_P(s) == IS_STRING)) {
        zend_str_tolower(Z_STRVAL_P(s), Z_STRLEN_P(s));
    }
}

int slim_start_with_str(const zval *str, char *compared, unsigned int compared_length)
{
    if (Z_TYPE_P(str) != IS_STRING || compared_length > (uint)(Z_STRLEN_P(str))) {
        return 0;
    }

    return !memcmp(Z_STRVAL_P(str), compared, compared_length);
}

int slim_preg_match(zval *retval, zval *regex, zval *subject, zval *matches, zend_long flags, int global)
{
    pcre_cache_entry *pce;
    zend_long start_offset = 0;
    int use_flags = flags ? 1 : 0;

    if (Z_TYPE_P(subject) != IS_STRING) {
        convert_to_string_ex(subject);
    }

    ZVAL_FALSE(retval);
    if (ZEND_SIZE_T_INT_OVFL(Z_STRLEN_P(subject))) {
        php_error_docref(NULL, E_WARNING, "Subject is too long");
        return FAILURE;
    }

    /* Compile regex or get it from cache. */
    if ((pce = pcre_get_compiled_regex_cache(Z_STR_P(regex))) == NULL) {
        return FAILURE;
    }

    //pce->refcount++;
    php_pcre_match_impl(pce, Z_STRVAL_P(subject), Z_STRLEN_P(subject), retval, matches, global, use_flags, flags, start_offset);
    //pce->refcount--;
    return SUCCESS;
}

void slim_fast_explode(zval *result, zval *delimiter, zval *str)
{
    if (unlikely(Z_TYPE_P(str) != IS_STRING || Z_TYPE_P(delimiter) != IS_STRING)) {
        ZVAL_NULL(result);
        zend_error(E_WARNING, "Invalid arguments supplied for explode()");
        return;
    }

    array_init(result);
    php_explode(Z_STR_P(delimiter), Z_STR_P(str), result, LONG_MAX);
}

int slim_start_with(const zval *str, const zval *compared, zval *case_sensitive)
{
    int sensitive = 0;
    int i;
    char *op1_cursor, *op2_cursor;

    if (Z_TYPE_P(str) != IS_STRING || Z_TYPE_P(compared) != IS_STRING) {
        return 0;
    }

    if (!Z_STRLEN_P(compared) || !Z_STRLEN_P(str) || Z_STRLEN_P(compared) > Z_STRLEN_P(str)) {
        return 0;
    }

    if (case_sensitive) {
        sensitive = zend_is_true(case_sensitive);
    }

    if (!sensitive) {
        return !memcmp(Z_STRVAL_P(str), Z_STRVAL_P(compared), Z_STRLEN_P(compared));
    }

    op1_cursor = Z_STRVAL_P(str);
    op2_cursor = Z_STRVAL_P(compared);
    for (i = 0; i < Z_STRLEN_P(compared); i++) {
        if (tolower(*op1_cursor) != tolower(*op2_cursor)) {
            return 0;
        }

        op1_cursor++;
        op2_cursor++;
    }

    return 1;
}
