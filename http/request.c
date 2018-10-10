#include "http/request.h"

#include <main/php_variables.h>
#include <main/SAPI.h>
#include <Zend/zend_smart_str.h>
#include <ext/standard/file.h>

#include "kernel/main.h"
#include "kernel/object.h"
#include "kernel/hash.h"
#include "kernel/array.h"
#include "kernel/operators.h"
#include "kernel/fcall.h"
#include "kernel/concat.h"
#include "kernel/string.h"

#include "interned-strings.h"

zend_class_entry *slim_http_request_ce;

PHP_METHOD(Slim_Http_Request, __construct);
PHP_METHOD(Slim_Http_Request, _get);
PHP_METHOD(Slim_Http_Request, get);
PHP_METHOD(Slim_Http_Request, getPost);
PHP_METHOD(Slim_Http_Request, getPut);
PHP_METHOD(Slim_Http_Request, getQuery);
PHP_METHOD(Slim_Http_Request, getServer);
PHP_METHOD(Slim_Http_Request, getEnv);
PHP_METHOD(Slim_Http_Request, getParam);
PHP_METHOD(Slim_Http_Request, has);
PHP_METHOD(Slim_Http_Request, hasPost);
PHP_METHOD(Slim_Http_Request, hasPut);
PHP_METHOD(Slim_Http_Request, hasQuery);
PHP_METHOD(Slim_Http_Request, hasServer);
PHP_METHOD(Slim_Http_Request, hasHeader);
PHP_METHOD(Slim_Http_Request, getHeader);
PHP_METHOD(Slim_Http_Request, getScheme);
PHP_METHOD(Slim_Http_Request, isAjax);
PHP_METHOD(Slim_Http_Request, isSecure);
PHP_METHOD(Slim_Http_Request, getRawBody);
PHP_METHOD(Slim_Http_Request, getServerAddress);
PHP_METHOD(Slim_Http_Request, getServerName);
PHP_METHOD(Slim_Http_Request, getHttpHost);
PHP_METHOD(Slim_Http_Request, getClientAddress);
PHP_METHOD(Slim_Http_Request, getMethod);
PHP_METHOD(Slim_Http_Request, getURI);
PHP_METHOD(Slim_Http_Request, getQueryString);
PHP_METHOD(Slim_Http_Request, getUserAgent);
PHP_METHOD(Slim_Http_Request, isMethod);
PHP_METHOD(Slim_Http_Request, isPost);
PHP_METHOD(Slim_Http_Request, isGet);
PHP_METHOD(Slim_Http_Request, isPut);
PHP_METHOD(Slim_Http_Request, isPatch);
PHP_METHOD(Slim_Http_Request, isHead);
PHP_METHOD(Slim_Http_Request, isDelete);
PHP_METHOD(Slim_Http_Request, isOptions);


static const zend_function_entry slim_http_request_method_entry[] = {
	PHP_ME(Slim_Http_Request, __construct, NULL, ZEND_ACC_PUBLIC|ZEND_ACC_CTOR)
	PHP_ME(Slim_Http_Request, _get, NULL, ZEND_ACC_PROTECTED)
	PHP_ME(Slim_Http_Request, get, NULL, ZEND_ACC_PUBLIC)
  PHP_ME(Slim_Http_Request, getPost, NULL, ZEND_ACC_PUBLIC)
  PHP_ME(Slim_Http_Request, getPut, NULL, ZEND_ACC_PUBLIC)
  PHP_ME(Slim_Http_Request, getQuery, NULL, ZEND_ACC_PUBLIC)
  PHP_ME(Slim_Http_Request, getServer, NULL, ZEND_ACC_PUBLIC)
  PHP_ME(Slim_Http_Request, getEnv, NULL, ZEND_ACC_PUBLIC)
  PHP_ME(Slim_Http_Request, getParam, NULL, ZEND_ACC_PUBLIC)
  PHP_ME(Slim_Http_Request, has, NULL, ZEND_ACC_PUBLIC)
  PHP_ME(Slim_Http_Request, hasPost, NULL, ZEND_ACC_PUBLIC)
  PHP_ME(Slim_Http_Request, hasPut, NULL, ZEND_ACC_PUBLIC)
  PHP_ME(Slim_Http_Request, hasQuery, NULL, ZEND_ACC_PUBLIC)
  PHP_ME(Slim_Http_Request, hasServer, NULL, ZEND_ACC_PUBLIC)
  PHP_ME(Slim_Http_Request, hasHeader, NULL, ZEND_ACC_PUBLIC)
  PHP_ME(Slim_Http_Request, getHeader, NULL, ZEND_ACC_PUBLIC)
  PHP_ME(Slim_Http_Request, getScheme, NULL, ZEND_ACC_PUBLIC)
  PHP_ME(Slim_Http_Request, isAjax, NULL, ZEND_ACC_PUBLIC)
  PHP_ME(Slim_Http_Request, isSecure, NULL, ZEND_ACC_PUBLIC)
  PHP_ME(Slim_Http_Request, getRawBody, NULL, ZEND_ACC_PUBLIC)
  PHP_ME(Slim_Http_Request, getServerAddress, NULL, ZEND_ACC_PUBLIC)
  PHP_ME(Slim_Http_Request, getServerName, NULL, ZEND_ACC_PUBLIC)
  PHP_ME(Slim_Http_Request, getHttpHost, NULL, ZEND_ACC_PUBLIC)
  PHP_ME(Slim_Http_Request, getClientAddress, NULL, ZEND_ACC_PUBLIC)
  PHP_ME(Slim_Http_Request, getMethod, NULL, ZEND_ACC_PUBLIC)
  PHP_ME(Slim_Http_Request, getURI, NULL, ZEND_ACC_PUBLIC)
  PHP_ME(Slim_Http_Request, getQueryString, NULL, ZEND_ACC_PUBLIC)
  PHP_ME(Slim_Http_Request, getUserAgent, NULL, ZEND_ACC_PUBLIC)
  PHP_ME(Slim_Http_Request, isMethod, NULL, ZEND_ACC_PUBLIC)
  PHP_ME(Slim_Http_Request, isPost, NULL, ZEND_ACC_PUBLIC)
  PHP_ME(Slim_Http_Request, isGet, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(Slim_Http_Request, isPut, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(Slim_Http_Request, isPatch, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(Slim_Http_Request, isHead, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(Slim_Http_Request, isDelete, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(Slim_Http_Request, isOptions, NULL, ZEND_ACC_PUBLIC)
	PHP_FE_END
};


SLIM_INIT_CLASS(Slim_Http_Request)
{
	SLIM_REGISTER_CLASS(Slim\\Http, Request, http_request, slim_http_request_method_entry, 0);

	zend_declare_property_null(slim_http_request_ce, SL("_rawBody"), ZEND_ACC_PROTECTED);
	zend_declare_property_null(slim_http_request_ce, SL("_put"), ZEND_ACC_PROTECTED);
	zend_declare_property_null(slim_http_request_ce, SL("_data"), ZEND_ACC_PROTECTED);

	return SUCCESS;
}

PHP_METHOD(Slim_Http_Request, __construct)
{
	zval *data = NULL;

	slim_fetch_params(0, 0, 1, &data);

	if (!data || Z_TYPE_P(data) != IS_ARRAY) {
		slim_update_property_empty_array(getThis(), SL("_data"));
	} else {
		slim_update_property(getThis(), SL("_data"), data);
	}
}

PHP_METHOD(Slim_Http_Request, _get)
{
	zval *data, *name, *default_value;
	zval value;

	slim_fetch_params(0, 3, 0, &data, &name, &default_value);

	if (Z_TYPE_P(name) != IS_NULL) {
		if (!slim_array_isset_fetch(&value, data, name, PH_READONLY)) {
			RETURN_CTOR(default_value);
		}
	} else {
		ZVAL_COPY_VALUE(&value, data);
	}

	if (SLIM_IS_EMPTY(&value)) {
		RETURN_CTOR(default_value);
	}

	RETURN_CTOR(&value);
}

PHP_METHOD(Slim_Http_Request, get)
{
	zval *name = NULL, *default_value = NULL, *request;
	zval put = {}, merged = {}, data = {}, merged2 = {};

	slim_fetch_params(0, 0, 2, &name, &default_value);

	if (!name) {
		name = &SLIM_G(z_null);
	}

	if (!default_value) {
		default_value = &SLIM_G(z_null);
	}

	request = slim_get_global_str(SL("_REQUEST"));
  SLIM_CALL_METHOD(&put, getThis(), "getput");

  slim_fast_array_merge(&merged, request, &put);
  zval_ptr_dtor(&put);

  slim_read_property(&data, getThis(), SL("_data"), PH_NOISY|PH_READONLY);

  slim_fast_array_merge(&merged2, &merged, &data);
  zval_ptr_dtor(&merged);

  SLIM_RETURN_CALL_SELF("_get", &merged2, name, default_value);

  zval_ptr_dtor(&merged2);
}

PHP_METHOD(Slim_Http_Request, getPost)
{
    zval *name = NULL, *default_value = NULL, *post;

    slim_fetch_params(0, 0, 2, &name, &default_value);

    if (!name) {
        name = &SLIM_G(z_null);
    }

    if (!default_value) {
        default_value = &SLIM_G(z_null);
    }

    post = slim_get_global_str(SL("_POST"));
    SLIM_RETURN_CALL_SELF("_get", post, name, default_value);
}

PHP_METHOD(Slim_Http_Request, getPut)
{
    zval *name = NULL, *default_value = NULL, is_put = {}, put = {}, new_put = {};

    slim_fetch_params(0, 0, 2, &name, &default_value);

    if (!name) {
        name = &SLIM_G(z_null);
    }

    if (!default_value) {
        default_value = &SLIM_G(z_null);
    }

    SLIM_CALL_METHOD(&is_put, getThis(), "isput");

    if (!zend_is_true(&is_put)) {
        RETURN_EMPTY_ARRAY();
    } else {
        slim_read_property(&put, getThis(), SL("_put"), PH_NOISY|PH_READONLY);
        if (Z_TYPE(put) != IS_ARRAY) {
            zval raw = {};
            char *tmp;
            SLIM_CALL_METHOD(&raw, getThis(), "getrawbody");
            SLIM_ENSURE_IS_STRING(&raw);

            array_init(&new_put);

            tmp = estrndup(Z_STRVAL(raw), Z_STRLEN(raw));
            sapi_module.treat_data(PARSE_STRING, tmp, &new_put);
            efree(tmp);

            slim_update_property(getThis(), SL("_put"), &new_put);
            zval_ptr_dtor(&raw);
        } else {
            ZVAL_COPY(&new_put, &put);
        }
    }

    SLIM_RETURN_CALL_SELF("_get", &new_put, name, default_value);
    zval_ptr_dtor(&new_put);

}

PHP_METHOD(Slim_Http_Request, getQuery)
{
    zval *name = NULL, *default_value = NULL, *get;

    slim_fetch_params(0, 0, 2, &name, &default_value);

    if (!name) {
        name = &SLIM_G(z_null);
    }

    if (!default_value) {
        default_value = &SLIM_G(z_null);
    }

    get = slim_get_global_str(SL("_GET"));

    SLIM_RETURN_CALL_SELF("_get", get, name, default_value);
}

PHP_METHOD(Slim_Http_Request, getServer)
{
    zval *name, *_SERVER;

    slim_fetch_params(0, 1, 0, &name);

    _SERVER = slim_get_global_str(SL("_SERVER"));
    if (!slim_array_isset_fetch(return_value, _SERVER, name, PH_COPY)) {
        RETURN_NULL();
    }
}

PHP_METHOD(Slim_Http_Request, getEnv)
{
    zval *name;

    slim_fetch_params(0, 1, 0, &name);

    SLIM_CALL_FUNCTION(return_value, "getenv", name);
}

PHP_METHOD(Slim_Http_Request, getParam)
{
    // @ TODO 后面实现
}

PHP_METHOD(Slim_Http_Request, has)
{
    zval *name, *_REQUEST;

    slim_fetch_params(0, 1, 0, &name);

    _REQUEST = slim_get_global_str(SL("_REQUEST"));
    RETURN_BOOL(slim_array_isset(_REQUEST, name));
}

PHP_METHOD(Slim_Http_Request, hasPost)
{
    zval *name, *_POST;

    slim_fetch_params(0, 1, 0, &name);

    _POST = slim_get_global_str(SL("_POST"));
    RETURN_BOOL(slim_array_isset(_POST, name));
}

PHP_METHOD(Slim_Http_Request, hasPut)
{
    zval *name, is_put = {}, put = {}, raw = {}, new_put = {};
    char *tmp;

    slim_fetch_params(0, 1, 0, &name);

    SLIM_CALL_METHOD(&is_put, getThis(), "isput");

    if (!zend_is_true(&is_put)) {
        slim_read_global_str(&new_put, SL("_PUT"));
    } else {
        slim_read_property(&put, getThis(), SL("_put"), PH_NOISY|PH_READONLY);
        if (Z_TYPE(put) != IS_ARRAY) {
            SLIM_CALL_METHOD(&raw, getThis(), "getrawbody");

            array_init(&new_put);

            SLIM_ENSURE_IS_STRING(&raw);
            tmp = estrndup(Z_STRVAL(raw), Z_STRLEN(raw));
            sapi_module.treat_data(PARSE_STRING, tmp, &new_put);

            slim_update_property(getThis(), SL("_put"), &new_put);
        } else {
            ZVAL_COPY_VALUE(&new_put, &put);
        }
    }

    RETVAL_BOOL(slim_array_isset(&new_put, name));
}

PHP_METHOD(Slim_Http_Request, hasQuery){

    zval *name, *_GET;

    slim_fetch_params(0, 1, 0, &name);

    _GET = slim_get_global_str(SL("_GET"));
    RETURN_BOOL(slim_array_isset(_GET, name));
}

PHP_METHOD(Slim_Http_Request, hasServer){

    zval *name, *_SERVER;

    slim_fetch_params(0, 1, 0, &name);

    _SERVER = slim_get_global_str(SL("_SERVER"));
    RETURN_BOOL(slim_array_isset(_SERVER, name));
}

PHP_METHOD(Slim_Http_Request, hasHeader){

    zval *header, *_SERVER, key = {};

    slim_fetch_params(0, 1, 0, &header);

    _SERVER = slim_get_global_str(SL("_SERVER"));
    if (slim_array_isset(_SERVER, header)) {
        RETURN_TRUE;
    }

    SLIM_CONCAT_SV(&key, "HTTP_", header);
    if (slim_array_isset(_SERVER, &key)) {
        RETURN_TRUE;
    }

    RETURN_FALSE;
}

PHP_METHOD(Slim_Http_Request, getHeader)
{
    zval *header, *_SERVER, key = {};

    slim_fetch_params(0, 1, 0, &header);

    _SERVER = slim_get_global_str(SL("_SERVER"));
    if (slim_array_isset_fetch(return_value, _SERVER, header, PH_COPY)) {
        return;
    }

    SLIM_CONCAT_SV(&key, "HTTP_", header);
    if (slim_array_isset_fetch(return_value, _SERVER, &key, PH_COPY)) {
        zval_ptr_dtor(&key);
        return;
    }
    zval_ptr_dtor(&key);

    RETURN_EMPTY_STRING();
}

PHP_METHOD(Slim_Http_Request, getScheme)
{
    zval https_header = {}, https = {};

    ZVAL_STRING(&https_header, "HTTPS");

    SLIM_CALL_METHOD(&https, getThis(), "getserver", &https_header);
    zval_ptr_dtor(&https_header);

    if (zend_is_true(&https)) {
        if (SLIM_IS_STRING(&https, "off")) {
            RETVAL_STRING("http");
        } else {
            RETVAL_STRING("https");
        }
    } else {
        RETVAL_STRING("http");
    }
    zval_ptr_dtor(&https);
}

PHP_METHOD(Slim_Http_Request, isAjax)
{
    zval requested_header = {}, xml_http_request = {}, requested_with = {};

    ZVAL_STRING(&requested_header, "HTTP_X_REQUESTED_WITH");
    ZVAL_STRING(&xml_http_request, "XMLHttpRequest");

    SLIM_CALL_METHOD(&requested_with, getThis(), "getheader", &requested_header);
    is_equal_function(return_value, &requested_with, &xml_http_request);
    return;
}

PHP_METHOD(Slim_Http_Request, isSecure)
{
    zval scheme = {}, https = {};

    SLIM_CALL_METHOD(&scheme, getThis(), "getscheme");

    ZVAL_STRING(&https, "https");
    is_identical_function(return_value, &https, &scheme);
}

PHP_METHOD(Slim_Http_Request, getRawBody)
{
    zval raw = {}, *zcontext = NULL;
    zend_string *content;
    php_stream_context *context;
    php_stream *stream;
    long int maxlen;

    slim_read_property(&raw, getThis(), SL("_rawBody"), PH_NOISY|PH_READONLY);
    if (Z_TYPE(raw) == IS_STRING) {
        RETURN_CTOR(&raw);
    }

    context = php_stream_context_from_zval(zcontext, 0);
    stream = php_stream_open_wrapper_ex("php://input", "rb", REPORT_ERRORS, NULL, context);
    maxlen    = PHP_STREAM_COPY_ALL;

    if (!stream) {
        RETURN_FALSE;
    }

    content = php_stream_copy_to_mem(stream, maxlen, 0);
    if (content != NULL) {
        RETVAL_STR(content);
        slim_update_property(getThis(), SL("_rawBody"), return_value);
    } else {
        RETVAL_FALSE;
    }

    php_stream_close(stream);
}

PHP_METHOD(Slim_Http_Request, getServerAddress)
{

    zval *server, server_addr = {};

    server = slim_get_global_str(SL("_SERVER"));
    if (slim_array_isset_fetch_str(&server_addr, server, SL("SERVER_ADDR"), PH_READONLY)) {
        RETURN_CTOR(&server_addr);
    }

    RETURN_STRING("127.0.0.1");
}

PHP_METHOD(Slim_Http_Request, getServerName)
{

    zval *server, server_name = {};

    server = slim_get_global_str(SL("_SERVER"));
    if (slim_array_isset_fetch_str(&server_name, server, SL("SERVER_NAME"), PH_READONLY)) {
        RETURN_CTOR(&server_name);
    }

    RETURN_STRING("localhost");
}

PHP_METHOD(Slim_Http_Request, getHttpHost)
{
	zval host = {}, http_host = {}, scheme = {}, server_name = {}, name = {}, server_port = {}, port = {}, http = {}, standard_port = {}, is_std_name = {}, is_std_port = {}, is_std_http = {};
	zval https = {}, secure_port = {}, is_secure_scheme = {}, is_secure_port = {}, is_secure_http = {};

	ZVAL_STRING(&host, "HTTP_HOST");
	SLIM_CALL_METHOD(&http_host, getThis(), "getserver", &host);
	zval_ptr_dtor(&host);

	if (zend_is_true(&http_host)) {
		RETVAL_ZVAL(&http_host, 0, 0);
		return;
	}
	zval_ptr_dtor(&http_host);

	SLIM_CALL_METHOD(&scheme, getThis(), "getscheme");

	ZVAL_STRING(&server_name, "SERVER_NAME");
	SLIM_CALL_METHOD(&name, getThis(), "getserver", &server_name);
	zval_ptr_dtor(&server_name);


	ZVAL_STRING(&server_port, "SERVER_PORT");
	SLIM_CALL_METHOD(&port, getThis(), "getserver", &server_port);
	zval_ptr_dtor(&server_port);

	ZVAL_STRING(&http, "http");
	ZVAL_LONG(&standard_port, 80);

	is_equal_function(&is_std_name, &scheme, &http);
	zval_ptr_dtor(&http);

	is_equal_function(&is_std_port, &port, &standard_port);

	slim_and_function(&is_std_http, &is_std_name, &is_std_port);

	ZVAL_STRING(&https, "https");
	ZVAL_LONG(&secure_port, 443);

	is_equal_function(&is_secure_scheme, &scheme, &https);
	zval_ptr_dtor(&scheme);
	zval_ptr_dtor(&https);

	is_equal_function(&is_secure_port, &port, &secure_port);
	slim_and_function(&is_secure_http, &is_secure_scheme, &is_secure_port);

	if (SLIM_IS_TRUE(&is_std_http)) {
		RETVAL_ZVAL(&name, 0, 0);
		return;
	}

	if (SLIM_IS_TRUE(&is_secure_http)) {
		RETVAL_ZVAL(&name, 0, 0);
		return;
	}

	SLIM_CONCAT_VSV(return_value, &name, ":", &port);
	zval_ptr_dtor(&name);
	zval_ptr_dtor(&port);
}

PHP_METHOD(Slim_Http_Request, getClientAddress)
{
	zval *trust_forwarded_header = NULL, *_SERVER, address = {};

	slim_fetch_params(0, 0, 1, &trust_forwarded_header);

	if (!trust_forwarded_header) {
		trust_forwarded_header = &SLIM_G(z_false);
	}

	_SERVER = slim_get_global_str(SL("_SERVER"));

	if (zend_is_true(trust_forwarded_header)) {
		if (!slim_array_isset_fetch_str(&address, _SERVER, SL("HTTP_X_FORWARDED_FOR"), PH_READONLY)) {
			if (!slim_array_isset_fetch_str(&address, _SERVER, SL("REMOTE_ADDR"), PH_READONLY)) {
				slim_array_fetch_str(&address, _SERVER, SL("REMOTE_ADDR"), PH_NOISY|PH_READONLY);
			}
		}
	} else if (!slim_array_isset_fetch_str(&address, _SERVER, SL("REMOTE_ADDR"), PH_READONLY)) {
		slim_array_fetch_str(&address, _SERVER, SL("REMOTE_ADDR"), PH_NOISY|PH_READONLY);
	}

	if (Z_TYPE(address) == IS_STRING) {
		if (slim_memnstr_str(&address, SL(","))) {
			zval addresses = {};

			slim_fast_explode_str(&addresses, SL(","), &address);

			slim_array_fetch_long(return_value, &addresses, 0, PH_NOISY|PH_COPY);
			zval_ptr_dtor(&addresses);
			return;
		}

		RETURN_CTOR(&address);
	}

	RETURN_NULL();
}

PHP_METHOD(Slim_Http_Request, getMethod)
{
    const char *m = slim_http_request_getmethod_helper();
    if (m) {
        RETURN_STRING(m);
    }

    RETURN_EMPTY_STRING();
}

PHP_METHOD(Slim_Http_Request, getURI)
{

    zval *value, *_SERVER, key = {};

    ZVAL_STRING(&key, "REQUEST_URI");

    _SERVER = slim_get_global_str(SL("_SERVER"));
    value = (Z_TYPE_P(_SERVER) == IS_ARRAY) ? slim_hash_get(Z_ARRVAL_P(_SERVER), &key, BP_VAR_UNSET) : NULL;
    if (value && Z_TYPE_P(value) == IS_STRING) {
        RETURN_ZVAL(value, 1, 0);
    }

    RETURN_EMPTY_STRING();
}

PHP_METHOD(Slim_Http_Request, getQueryString)
{
    zval *value, *_SERVER, key = {};

    ZVAL_STRING(&key, "QUERY_STRING");

    _SERVER = slim_get_global_str(SL("_SERVER"));
    value = (Z_TYPE_P(_SERVER) == IS_ARRAY) ? slim_hash_get(Z_ARRVAL_P(_SERVER), &key, BP_VAR_UNSET) : NULL;
    if (value && Z_TYPE_P(value) == IS_STRING) {
        RETURN_ZVAL(value, 1, 0);
    }

    RETURN_EMPTY_STRING();
}

PHP_METHOD(Slim_Http_Request, getUserAgent)
{
    zval *server, user_agent = {};

    server = slim_get_global_str(SL("_SERVER"));
    if (slim_array_isset_fetch_str(&user_agent, server, SL("HTTP_USER_AGENT"), PH_READONLY)) {
        RETURN_CTOR(&user_agent);
    }

    RETURN_EMPTY_STRING();
}

PHP_METHOD(Slim_Http_Request, isMethod)
{
    zval *methods, http_method = {}, *method;

    slim_fetch_params(0, 1, 0, &methods);

    SLIM_CALL_METHOD(&http_method, getThis(), "getmethod");

    if (Z_TYPE_P(methods) == IS_STRING) {
        is_equal_function(return_value, methods, &http_method);
        zval_ptr_dtor(&http_method);
        return;
    }

    ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(methods), method) {
        if (SLIM_IS_EQUAL(method, &http_method)) {
            zval_ptr_dtor(&http_method);
            RETURN_TRUE;
        }
    } ZEND_HASH_FOREACH_END();
    zval_ptr_dtor(&http_method);

    RETURN_FALSE;
}

PHP_METHOD(Slim_Http_Request, isPost)
{
    zval post = {}, method = {};

    if (Z_OBJCE_P(getThis()) == slim_http_request_ce) {
        RETURN_BOOL(!strcmp(slim_http_request_getmethod_helper(), "POST"));
    }

    ZVAL_STR(&post, IS(POST));

    SLIM_CALL_METHOD(&method, getThis(), "getmethod");
    is_equal_function(return_value, &method, &post);
    zval_ptr_dtor(&method);
}

PHP_METHOD(Slim_Http_Request, isGet)
{
	zval get = {}, method = {};

	if (Z_OBJCE_P(getThis()) == slim_http_request_ce) {
		RETURN_BOOL(!strcmp(slim_http_request_getmethod_helper(), "GET"));
	}

	ZVAL_STR(&get, IS(GET));

	SLIM_CALL_METHOD(&method, getThis(), "getmethod");
	is_equal_function(return_value, &method, &get);
	zval_ptr_dtor(&method);
}

PHP_METHOD(Slim_Http_Request, isPut)
{
	zval put = {}, method = {};

	if (Z_OBJCE_P(getThis()) == slim_http_request_ce) {
		RETURN_BOOL(!strcmp(slim_http_request_getmethod_helper(), "PUT"));
	}

	ZVAL_STR(&put, IS(PUT));

	SLIM_CALL_METHOD(&method, getThis(), "getmethod");
	is_equal_function(return_value, &method, &put);
	zval_ptr_dtor(&method);
}

PHP_METHOD(Slim_Http_Request, isPatch)
{
	zval patch = {}, method = {};

	if (Z_OBJCE_P(getThis()) == slim_http_request_ce) {
		RETURN_BOOL(!strcmp(slim_http_request_getmethod_helper(), "PATCH"));
	}

	ZVAL_STR(&patch, IS(PATCH));

	SLIM_CALL_METHOD(&method, getThis(), "getmethod");
	is_equal_function(return_value, &method, &patch);
	zval_ptr_dtor(&method);
}

PHP_METHOD(Slim_Http_Request, isHead)
{
	zval head = {}, method = {};

	if (Z_OBJCE_P(getThis()) == slim_http_request_ce) {
		RETURN_BOOL(!strcmp(slim_http_request_getmethod_helper(), "HEAD"));
	}

	ZVAL_STR(&head, IS(HEAD));

	SLIM_CALL_METHOD(&method, getThis(), "getmethod");
	is_equal_function(return_value, &method, &head);
	zval_ptr_dtor(&method);
}

PHP_METHOD(Slim_Http_Request, isDelete)
{
	zval delete = {}, method = {};

	if (Z_OBJCE_P(getThis()) == slim_http_request_ce) {
		RETURN_BOOL(!strcmp(slim_http_request_getmethod_helper(), "DELETE"));
	}

	ZVAL_STR(&delete, IS(DELETE));

	SLIM_CALL_METHOD(&method, getThis(), "getmethod");
	is_equal_function(return_value, &method, &delete);
	zval_ptr_dtor(&method);
}

PHP_METHOD(Slim_Http_Request, isOptions)
{
	zval options = {}, method = {};

	if (Z_OBJCE_P(getThis()) == slim_http_request_ce) {
		RETURN_BOOL(!strcmp(slim_http_request_getmethod_helper(), "OPTIONS"));
	}

	SLIM_CALL_METHOD(&method, getThis(), "getmethod");
	is_equal_function(return_value, &method, &options);
	zval_ptr_dtor(&method);
}

static const char* slim_http_request_getmethod_helper()
{
    zval *value, *_SERVER, key = {};
    const char *method = SG(request_info).request_method;

    if (unlikely(!method)) {
        ZVAL_STRING(&key, "REQUEST_METHOD");

        _SERVER = slim_get_global_str(SL("_SERVER"));
        if (Z_TYPE_P(_SERVER) == IS_ARRAY) {
            value = slim_hash_get(Z_ARRVAL_P(_SERVER), &key, BP_VAR_UNSET);
            zval_ptr_dtor(&key);
            if (value && Z_TYPE_P(value) == IS_STRING) {
                return Z_STRVAL_P(value);
            }
        }

        return "";
    }

    return method;
}
