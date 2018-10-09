#include "http/responseinterface.h"
#include "http/response.h"
#include "http/response/exception.h"
#include "kernel/main.h"
#include "kernel/object.h"
#include "kernel/operators.h"
#include "kernel/exception.h"
#include "kernel/string.h"

zend_class_entry *slim_http_response_ce;

PHP_METHOD(Slim_Http_Response, __construct);
PHP_METHOD(Slim_Http_Response, setStatusCode);
PHP_METHOD(Slim_Http_Response, setContent);
PHP_METHOD(Slim_Http_Response, setJsonContent);
PHP_METHOD(Slim_Http_Response, isSent);
PHP_METHOD(Slim_Http_Response, send);

static const zend_function_entry slim_http_response_method_entry[] = {
    PHP_ME(Slim_Http_Response, __construct, NULL, ZEND_ACC_PUBLIC|ZEND_ACC_CTOR)
    //PHP_ME(Slim_Http_Response, setStatusCode, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(Slim_Http_Response, setContent, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(Slim_Http_Response, setJsonContent, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(Slim_Http_Response, isSent, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(Slim_Http_Response, send, NULL, ZEND_ACC_PUBLIC)
    PHP_FE_END
};

SLIM_INIT_CLASS(Slim_Http_Response)
{
    SLIM_REGISTER_CLASS(Slim\\Http, Response, http_response, slim_http_response_method_entry, 0);

    zend_declare_property_bool(slim_http_response_ce, SL("_sent"), 0, ZEND_ACC_PROTECTED);
    zend_declare_property_null(slim_http_response_ce, SL("_content"), ZEND_ACC_PROTECTED);
    //zend_declare_property_null(slim_http_response_ce, SL("_headers"), ZEND_ACC_PROTECTED);
    //zend_declare_property_null(slim_http_response_ce, SL("_cookies"), ZEND_ACC_PROTECTED);
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

    /*
    if (code && Z_TYPE_P(code) != IS_NULL) {
        SLIM_CALL_METHOD(NULL, getThis(), "setstatuscode", code, status);
    }
    */
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

PHP_METHOD(Slim_Http_Response, isSent)
{
    RETURN_MEMBER(getThis(), "_sent");
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
