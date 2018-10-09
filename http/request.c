#include "http/request.h"
#include "kernel/main.h"
#include "kernel/object.h"

zend_class_entry *slim_http_request_ce;

PHP_METHOD(Slim_Http_Request, __construct);
PHP_METHOD(Slim_Http_Request, _get);
PHP_METHOD(Slim_Http_Request, get);

static const zend_function_entry slim_http_request_method_entry[] = {
	PHP_ME(Slim_Http_Request, __construct, NULL, ZEND_ACC_PUBLIC|ZEND_ACC_CTOR)
	PHP_ME(Slim_Http_Request, _get, NULL, ZEND_ACC_PROTECTED)
	PHP_ME(Slim_Http_Request, get, NULL, ZEND_ACC_PUBLIC)
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
			RETURN_CTOR(detault_value);
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

	slim_fetch_params(0, 2, 0, &name, &default_value);

	if (!name) {
		name = &SLIM_G(z_null);
	}

	if (!default_value) {
		default_value = &SLIM_G(z_null);
	}

	request = slim_get_global_str(SL("_REQUEST"));

}
