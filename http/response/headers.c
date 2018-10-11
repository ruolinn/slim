#include "http/response/headers.h"

#include <main/SAPI.h>
#include <Zend/zend_smart_str.h>

#include "kernel/main.h"
#include "kernel/object.h"
#include "kernel/array.h"
#include "kernel/operators.h"
#include "kernel/concat.h"

zend_class_entry *slim_http_response_headers_ce;

PHP_METHOD(Slim_Http_Response_Headers, set);
PHP_METHOD(Slim_Http_Response_Headers, setRaw);
PHP_METHOD(Slim_Http_Response_Headers, remove);
PHP_METHOD(Slim_Http_Response_Headers, send);
PHP_METHOD(Slim_Http_Response_Headers, reset);
PHP_METHOD(Slim_Http_Response_Headers, toArray);


static const zend_function_entry slim_http_response_headers_method_entry[] = {
    PHP_ME(Slim_Http_Response_Headers, set, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(Slim_Http_Response_Headers, setRaw, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(Slim_Http_Response_Headers, remove, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(Slim_Http_Response_Headers, send, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(Slim_Http_Response_Headers, reset, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(Slim_Http_Response_Headers, toArray, NULL, ZEND_ACC_PUBLIC)
    PHP_FE_END
};

SLIM_INIT_CLASS(Slim_Http_Response_Headers)
{
    SLIM_REGISTER_CLASS(Slim\\Http\\Response, Headers, http_response_headers, slim_http_response_headers_method_entry, 0);

    zend_declare_property_null(slim_http_response_headers_ce, SL("_headers"), ZEND_ACC_PROTECTED);

    return SUCCESS;
}

PHP_METHOD(Slim_Http_Response_Headers, set)
{
    zval *name, *value;

    slim_fetch_params(0, 2, 0, &name, &value);
    slim_update_property_array(getThis(), SL("_headers"), name, value);
}

PHP_METHOD(Slim_Http_Response_Headers, setRaw)
{
    zval *header;

    slim_fetch_params(0, 1, 0, &header);

    slim_update_property_array(getThis(), SL("_headers"), header, &SLIM_G(z_null));
}

PHP_METHOD(Slim_Http_Response_Headers, remove)
{
    zval *header_index, headers = {};

    slim_fetch_params(0, 1, 0, &header_index);

    slim_read_property(&headers, getThis(), SL("_headers"), PH_NOISY|PH_READONLY);

    slim_array_unset(&headers, header_index, 0);

    slim_update_property(getThis(), SL("_headers"), &headers);
}

PHP_METHOD(Slim_Http_Response_Headers, send)
{
	zval headers = {}, *value;
	sapi_header_line ctr = { NULL, 0, 0 };
	zend_string *str_key;
	ulong idx;

	if (!SG(headers_sent)) {
		slim_read_property(&headers, getThis(), SL("_headers"), PH_NOISY|PH_READONLY);

		if (Z_TYPE(headers) != IS_ARRAY) {
			/* No headers to send */
			RETURN_TRUE;
		}

		ZEND_HASH_FOREACH_KEY_VAL(Z_ARRVAL(headers), idx, str_key, value) {
			zval header = {};
			if (str_key) {
				ZVAL_STR(&header, str_key);
			} else {
				ZVAL_LONG(&header, idx);
			}

			if (SLIM_IS_NOT_EMPTY(value)) {
				zval http_header = {};
				SLIM_CONCAT_VSV(&http_header, &header, ": ", value);
				ctr.line     = Z_STRVAL(http_header);
				ctr.line_len = Z_STRLEN(http_header);
				sapi_header_op(SAPI_HEADER_REPLACE, &ctr);
				zval_ptr_dtor(&http_header);
			} else if (Z_TYPE(header) == IS_STRING) {
				ctr.line     = Z_STRVAL(header);
				ctr.line_len = Z_STRLEN(header);
				sapi_header_op(SAPI_HEADER_REPLACE, &ctr);
			} else {
				zval tmp= {};
				ZVAL_DUP(&tmp, &header);
				convert_to_string(&tmp);

				ctr.line     = Z_STRVAL(tmp);
				ctr.line_len = Z_STRLEN(tmp);
				sapi_header_op(SAPI_HEADER_REPLACE, &ctr);
				zval_ptr_dtor(&tmp);
			}
		} ZEND_HASH_FOREACH_END();

		RETURN_TRUE;
	}

	RETURN_FALSE;
}

PHP_METHOD(Slim_Http_Response_Headers, reset)
{
    slim_update_property_empty_array(getThis(), SL("_headers"));
}

PHP_METHOD(Slim_Http_Response_Headers, toArray)
{
    RETURN_MEMBER(getThis(), "_headers");
}
