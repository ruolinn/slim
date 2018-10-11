#include "http/responseinterface.h"
#include "http/response/headers.h"
#include "http/response.h"
#include "http/response/exception.h"

#include <ext/date/php_date.h>

#include "kernel/main.h"
#include "kernel/object.h"
#include "kernel/operators.h"
#include "kernel/exception.h"
#include "kernel/string.h"
#include "kernel/fcall.h"
#include "kernel/concat.h"

zend_class_entry *slim_http_response_ce;

PHP_METHOD(Slim_Http_Response, __construct);
PHP_METHOD(Slim_Http_Response, setStatusCode);
PHP_METHOD(Slim_Http_Response, setHeaders);
PHP_METHOD(Slim_Http_Response, getHeaders);
PHP_METHOD(Slim_Http_Response, setCookies);
PHP_METHOD(Slim_Http_Response, getCookies);
PHP_METHOD(Slim_Http_Response, setHeader);
PHP_METHOD(Slim_Http_Response, setRawHeader);
PHP_METHOD(Slim_Http_Response, resetHeaders);
PHP_METHOD(Slim_Http_Response, setExpires);
PHP_METHOD(Slim_Http_Response, setNotModified);
PHP_METHOD(Slim_Http_Response, setContentType);
PHP_METHOD(Slim_Http_Response, setEtag);
PHP_METHOD(Slim_Http_Response, redirect);
PHP_METHOD(Slim_Http_Response, setContent);
PHP_METHOD(Slim_Http_Response, setJsonContent);
PHP_METHOD(Slim_Http_Response, appendContent);
PHP_METHOD(Slim_Http_Response, getContent);
PHP_METHOD(Slim_Http_Response, getJsonContent);
PHP_METHOD(Slim_Http_Response, isSent);
PHP_METHOD(Slim_Http_Response, sendHeaders);
PHP_METHOD(Slim_Http_Response, sendCookies);
PHP_METHOD(Slim_Http_Response, send);
PHP_METHOD(Slim_Http_Response, setFileToSend);

static const zend_function_entry slim_http_response_method_entry[] = {
    PHP_ME(Slim_Http_Response, __construct, NULL, ZEND_ACC_PUBLIC|ZEND_ACC_CTOR)
    PHP_ME(Slim_Http_Response, setStatusCode, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(Slim_Http_Response, setHeaders, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(Slim_Http_Response, getHeaders, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(Slim_Http_Response, setCookies, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(Slim_Http_Response, getCookies, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(Slim_Http_Response, setHeader, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(Slim_Http_Response, setRawHeader, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(Slim_Http_Response, resetHeaders, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(Slim_Http_Response, setExpires, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(Slim_Http_Response, setNotModified, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(Slim_Http_Response, setContentType, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(Slim_Http_Response, setEtag, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(Slim_Http_Response, redirect, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(Slim_Http_Response, setContent, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(Slim_Http_Response, setJsonContent, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(Slim_Http_Response, appendContent, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(Slim_Http_Response, getContent, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(Slim_Http_Response, getJsonContent, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(Slim_Http_Response, isSent, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(Slim_Http_Response, sendHeaders, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(Slim_Http_Response, sendCookies, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(Slim_Http_Response, send, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(Slim_Http_Response, setFileToSend, NULL, ZEND_ACC_PUBLIC)
    PHP_FE_END
};

SLIM_INIT_CLASS(Slim_Http_Response)
{
    SLIM_REGISTER_CLASS(Slim\\Http, Response, http_response, slim_http_response_method_entry, 0);

    zend_declare_property_bool(slim_http_response_ce, SL("_sent"), 0, ZEND_ACC_PROTECTED);
    zend_declare_property_null(slim_http_response_ce, SL("_content"), ZEND_ACC_PROTECTED);
    zend_declare_property_null(slim_http_response_ce, SL("_headers"), ZEND_ACC_PROTECTED);
    zend_declare_property_null(slim_http_response_ce, SL("_cookies"), ZEND_ACC_PROTECTED);
    //zend_declare_property_null(slim_http_response_ce, SL("_file"), ZEND_ACC_PROTECTED);

    zend_class_implements(slim_http_response_ce, 1, slim_http_responseinterface_ce);

    return SUCCESS;
}

PHP_METHOD(Slim_Http_Response, __construct)
{
    zval *content = NULL, *code = NULL, *status = NULL;

    slim_fetch_params(0, 0, 3, &content, &code, &status);

    if (!status) {
        status = &SLIM_G(z_null);
    }

    if (content && Z_TYPE_P(content) != IS_NULL) {
        slim_update_property(getThis(), SL("_content"), content);
    }

    if (code && Z_TYPE_P(code) != IS_NULL) {
        SLIM_CALL_METHOD(NULL, getThis(), "setstatuscode", code, status);
    }
}

PHP_METHOD(Slim_Http_Response, setStatusCode)
{
	zval *code, *message, headers = {}, current_headers_raw = {}, header_value = {}, status_value = {}, status_header = {};
	zend_string *str_key;

	slim_fetch_params(0, 2, 0, &code, &message);

	SLIM_CALL_METHOD(&headers, getThis(), "getheaders");

	/**
	 * We use HTTP/1.1 instead of HTTP/1.0
	 *
	 * Before that we would like to unset any existing HTTP/x.y headers
	 */
	SLIM_CALL_METHOD(&current_headers_raw, &headers, "toarray");

	if (Z_TYPE(current_headers_raw) == IS_ARRAY) {
		ZEND_HASH_FOREACH_STR_KEY(Z_ARRVAL(current_headers_raw), str_key) {
			zval header_name = {};
			if (str_key) {
				ZVAL_STR(&header_name, str_key);
				if ((size_t)(Z_STRLEN(header_name)) > sizeof("HTTP/x.y ")-1 && !memcmp(Z_STRVAL(header_name), "HTTP/", 5)) {
					SLIM_CALL_METHOD(NULL, &headers, "remove", &header_name);
				}
			}
		} ZEND_HASH_FOREACH_END();
	}

	SLIM_CONCAT_SVSV(&header_value, "HTTP/1.1 ", code, " ", message);
	SLIM_CALL_METHOD(NULL, &headers, "setraw", &header_value);

	/**
	 * We also define a 'Status' header with the HTTP status
	 */
	SLIM_CONCAT_VSV(&status_value, code, " ", message);

	ZVAL_STRING(&status_header, "Status");
	SLIM_CALL_METHOD(NULL, &headers, "set", &status_header, &status_value);
	slim_update_property(getThis(), SL("_headers"), &headers);
	RETURN_THIS();
}

PHP_METHOD(Slim_Http_Response, setHeaders)
{
    zval *headers;

    slim_fetch_params(0, 1, 0, &headers);

    if (Z_TYPE_P(headers) != IS_OBJECT) {
        SLIM_THROW_EXCEPTION_STR(slim_http_response_exception_ce, "The headers bag is not valid");
        return;
    }

    slim_update_property(getThis(), SL("_headers"), headers);
    RETURN_THIS();
}

PHP_METHOD(Slim_Http_Response, getHeaders)
{
    zval headers = {};

    slim_read_property(&headers, getThis(), SL("_headers"), PH_NOISY|PH_READONLY);
    if (Z_TYPE(headers) == IS_NULL) {
        object_init_ex(return_value, slim_http_response_headers_ce);
        slim_update_property(getThis(), SL("_headers"), return_value);
        return;
    }

    RETURN_CTOR(&headers);
}

PHP_METHOD(Slim_Http_Response, setCookies)
{
    zval *cookies;

    slim_fetch_params(0, 1, 0, &cookies);

    if (Z_TYPE_P(cookies) != IS_OBJECT) {
        SLIM_THROW_EXCEPTION_STR(slim_http_response_exception_ce, "The cookies bag is not valid");
        return;
    }
    slim_update_property(getThis(), SL("_cookies"), cookies);

    RETURN_THIS();
}

PHP_METHOD(Slim_Http_Response, getCookies)
{
    RETURN_MEMBER(getThis(), "_cookies");
}

PHP_METHOD(Slim_Http_Response, setHeader)
{
    zval *name, *value, headers = {};

    slim_fetch_params(0, 2, 0, &name, &value);

    SLIM_CALL_METHOD(&headers, getThis(), "getheaders");
    SLIM_CALL_METHOD(NULL, &headers, "set", name, value);

    RETURN_THIS();
}

PHP_METHOD(Slim_Http_Response, setRawHeader)
{
    zval *header, headers = {};

    slim_fetch_params(0, 1, 0, &header);

    SLIM_CALL_METHOD(&headers, getThis(), "getheaders");
    SLIM_CALL_METHOD(NULL, &headers, "setraw", header);

    RETURN_THIS();
}

PHP_METHOD(Slim_Http_Response, resetHeaders)
{
    zval headers = {};

    SLIM_CALL_METHOD(&headers, getThis(), "getheaders");
    SLIM_CALL_METHOD(NULL, &headers, "reset");

    RETURN_THIS();
}

PHP_METHOD(Slim_Http_Response, setExpires)
{
	zval *datetime, headers = {}, date = {}, utc_zone = {}, timezone = {}, format = {}, utc_format = {}, utc_date = {}, expires_header = {};
	zend_class_entry *datetime_ce, *datetimezone_ce;

	slim_fetch_params(0, 1, 0, &datetime);

	datetime_ce = php_date_get_date_ce();
	SLIM_VERIFY_CLASS_EX(datetime, datetime_ce, slim_http_response_exception_ce);

	SLIM_CALL_METHOD(&headers, getThis(), "getheaders");

	if (slim_clone(&date, datetime) == FAILURE) {
		return;
	}

	/**
	 * All the expiration times are sent in UTC
	 */
	ZVAL_STRING(&utc_zone, "UTC");

	datetimezone_ce = php_date_get_timezone_ce();
	object_init_ex(&timezone, datetimezone_ce);

	SLIM_CALL_METHOD(NULL, &timezone, "__construct", &utc_zone);

	/**
	 * Change the timezone to utc
	 */
	SLIM_CALL_METHOD(NULL, &date, "settimezone", &timezone);

	ZVAL_STRING(&format, "D, d M Y H:i:s");

	SLIM_CALL_METHOD(&utc_format, &date, "format", &format);

	SLIM_CONCAT_VS(&utc_date, &utc_format, " GMT");

	/**
	 * The 'Expires' header set this info
	 */
	ZVAL_STRING(&expires_header, "Expires");

	SLIM_CALL_METHOD(NULL, getThis(), "setheader", &expires_header, &utc_date);

	RETURN_THIS();
}

PHP_METHOD(Slim_Http_Response, setNotModified)
{
    zval code = {}, status = {};

    ZVAL_LONG(&code, 304);
    ZVAL_STRING(&status, "Not modified");

    SLIM_CALL_METHOD(NULL, getThis(), "setstatuscode", &code, &status);

    RETURN_THIS();
}

PHP_METHOD(Slim_Http_Response, setContentType)
{
    zval *content_type, *charset = NULL, headers = {}, name = {};

    slim_fetch_params(0, 1, 1, &content_type, &charset);

    SLIM_CALL_METHOD(&headers, getThis(), "getheaders");

    ZVAL_STRING(&name, "Content-Type");

    if (!charset || Z_TYPE_P(charset) == IS_NULL) {
        SLIM_CALL_METHOD(NULL, &headers, "set", &name, content_type);
    } else {
        zval header_value = {};
        SLIM_CONCAT_VSV(&header_value, content_type, "; charset=", charset);
        SLIM_CALL_METHOD(NULL, &headers, "set", &name, &header_value);
        zval_ptr_dtor(&header_value);
    }
    zval_ptr_dtor(&name);
    zval_ptr_dtor(&headers);
    RETURN_THIS();
}

PHP_METHOD(Slim_Http_Response, setEtag)
{
    zval *etag, name = {}, headers = {};

    slim_fetch_params(0, 1, 0, &etag);

    ZVAL_STRING(&name, "ETag");

    SLIM_CALL_METHOD(&headers, getThis(), "getheaders");
    SLIM_CALL_METHOD(NULL, &headers, "set", &name, etag);

    RETURN_THIS();
}

PHP_METHOD(Slim_Http_Response, redirect)
{
    // @ TODO 稍后实现
}

PHP_METHOD(Slim_Http_Response, setContent)
{
    zval *content;

    slim_fetch_params(0, 1, 0, &content);

    slim_update_property(getThis(), SL("_content"), content);

    RETURN_THIS();
}

PHP_METHOD(Slim_Http_Response, setJsonContent)
{
    zval *content, *json_options = NULL, json_content = {};
    int options = 0;

    slim_fetch_params(0, 1, 1, &content, &json_options);

    if (json_options) {
        options = slim_get_intval(json_options);
    }

    RETURN_ON_FAILURE(slim_json_encode(&json_content, content, options));

    slim_update_property(getThis(), SL("_content"), &json_content);

    RETURN_THIS();
}

PHP_METHOD(Slim_Http_Response, appendContent)
{
    zval *_content, content = {}, temp_content = {};

    slim_fetch_params(0, 1, 0, &_content);

    slim_read_property(&content, getThis(), SL("_content"), PH_NOISY|PH_READONLY);

    concat_function(&temp_content, &content, _content);

    slim_update_property(getThis(), SL("_content"), &temp_content);
    zval_ptr_dtor(&temp_content);
    RETURN_THIS();
}

PHP_METHOD(Slim_Http_Response, getContent)
{
    RETURN_MEMBER(getThis(), "_content");
}

PHP_METHOD(Slim_Http_Response, getJsonContent)
{
    zval content = {};

    slim_read_property(&content, getThis(), SL("_content"), PH_NOISY|PH_READONLY);
    SLIM_CALL_FUNCTION(return_value, "json_decode", &content);
}

PHP_METHOD(Slim_Http_Response, isSent)
{
    RETURN_MEMBER(getThis(), "_sent");
}

PHP_METHOD(Slim_Http_Response, sendHeaders)
{

    zval headers = {};

    slim_read_property(&headers, getThis(), SL("_headers"), PH_NOISY|PH_READONLY);
    if (Z_TYPE(headers) == IS_OBJECT) {
        SLIM_CALL_METHOD(NULL, &headers, "send");
    }

    RETURN_THIS();
}

PHP_METHOD(Slim_Http_Response, sendCookies)
{
    zval cookies = {};

    slim_read_property(&cookies, getThis(), SL("_cookies"), PH_NOISY|PH_READONLY);
    if (Z_TYPE(cookies) == IS_OBJECT) {
        SLIM_CALL_METHOD(NULL, &cookies, "send");
    }

    RETURN_THIS();
}

PHP_METHOD(Slim_Http_Response, send)
{
    zval sent = {}, content = {};

    slim_read_property(&sent, getThis(), SL("_sent"), PH_NOISY|PH_READONLY);
    if (SLIM_IS_FALSE(&sent)) {
        slim_read_property(&content, getThis(), SL("_content"), PH_NOISY|PH_READONLY);
        if (Z_TYPE(content) != IS_NULL) {
            zend_print_zval(&content, 0);
            goto gotoend;
        }

gotoend:
        slim_update_property_bool(getThis(), SL("_sent"), 1);
        RETURN_THIS();
    }

    SLIM_THROW_EXCEPTION_STR(slim_http_response_exception_ce, "Response was already, sent");
}

PHP_METHOD(Slim_Http_Response, setFileToSend)
{
    // @ TODO 稍后实现
}
