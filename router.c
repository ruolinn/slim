#include "router.h"
#include "router/route.h"
#include "kernel/main.h"
#include "kernel/object.h"
#include "kernel/fcall.h"
#include "kernel/array.h"
#include "kernel/string.h"
#include "kernel/operators.h"

//#include <main/SAPI.h>

zend_class_entry *slim_router_ce;

PHP_METHOD(Slim_Router, __construct);
PHP_METHOD(Slim_Router, add);
PHP_METHOD(Slim_Router, getMatchedRoute);
PHP_METHOD(Slim_Router, handle);
PHP_METHOD(Slim_Router, getPathInfo);

static const zend_function_entry slim_router_method_entry[] = {
    PHP_ME(Slim_Router, __construct, NULL, ZEND_ACC_PUBLIC|ZEND_ACC_CTOR)
    PHP_ME(Slim_Router, add, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(Slim_Router, getMatchedRoute, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(Slim_Router, handle, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(Slim_Router, getPathInfo, NULL, ZEND_ACC_PUBLIC)
    PHP_FE_END
};

SLIM_INIT_CLASS(Slim_Router)
{
    SLIM_REGISTER_CLASS(Slim, Router, router, slim_router_method_entry, 0);

    zend_declare_property_null(slim_router_ce, SL("_params"), ZEND_ACC_PROTECTED);
    zend_declare_property_null(slim_router_ce, SL("_routes"), ZEND_ACC_PROTECTED);
    zend_declare_property_null(slim_router_ce, SL("_matchedRoute"), ZEND_ACC_PROTECTED);
    zend_declare_property_bool(slim_router_ce, SL("_wasMatched"), 0, ZEND_ACC_PROTECTED);

    return SUCCESS;
}


PHP_METHOD(Slim_Router, __construct)
{
    zval routes = {};

    array_init(&routes);
    slim_update_property_empty_array(getThis(), SL("_params"));
    slim_update_property(getThis(), SL("_routes"), &routes);

    zval_ptr_dtor(&routes);
}


PHP_METHOD(Slim_Router, add)
{
    zval *pattern, *callable, *http_methods;

    slim_fetch_params(0, 3, 0, &http_methods, &pattern, &callable);

    if (!http_methods) {
        http_methods = &SLIM_G(z_null);
    }

    if (!callable) {
        callable = &SLIM_G(z_null);
    }


    object_init_ex(return_value, slim_router_route_ce);
    SLIM_CALL_METHOD(NULL, return_value, "__construct", http_methods, pattern, callable);

    slim_update_property_array_append(getThis(), SL("_routes"), return_value);
}

PHP_METHOD(Slim_Router, handle)
{
    zval *uri = NULL, real_uri = {}, route_found = {}, routes = {}, *route, handled_uri;
    zval case_sensitive;

    slim_fetch_params(0, 0, 1, &uri);

    if (!uri || !zend_is_true(uri)) {
        SLIM_CALL_METHOD(&real_uri, getThis(), "getpathinfo");
    } else {
        ZVAL_COPY_VALUE(&real_uri, uri);
    }

    ZVAL_FALSE(&case_sensitive);

    ZVAL_FALSE(&route_found);
    ZVAL_COPY_VALUE(&handled_uri, &real_uri);


    slim_read_property(&routes, getThis(), SL("_routes"), PH_NOISY|PH_READONLY);

    ZEND_HASH_REVERSE_FOREACH_VAL(Z_ARRVAL(routes), route) {
        zval pattern;

        SLIM_CALL_METHOD(&pattern, route, "getcompiledpattern");
        ZVAL_BOOL(&route_found, slim_comparestr(&pattern, &handled_uri, &case_sensitive));

        if (zend_is_true(&route_found)) {
            slim_update_property(getThis(), SL("_matchedRoute"), route);
            break;
        }
    } ZEND_HASH_FOREACH_END();

    slim_update_property_bool(getThis(), SL("_wasMatched"), zend_is_true(&route_found));

    if (zend_is_true(&route_found)) {
        // @ TODO 未找到异常
        RETURN_TRUE;
    }

    RETURN_FALSE;
}

PHP_METHOD(Slim_Router, getPathInfo)
{
    zval *_SERVER, url = {}, real_uri = {};

    _SERVER = slim_get_global_str(SL("_SERVER"));
    if (slim_array_isset_fetch_str(&url, _SERVER, SL("REQUEST_URI"), PH_READONLY)) {
        zval url_parts = {};
        slim_fast_explode_str(&url_parts, SL("?"), &url);

        slim_array_fetch_long(&real_uri, &url_parts, 0, PH_NOISY|PH_COPY);
        if (SLIM_IS_NOT_EMPTY(&real_uri)) {
            zval_ptr_dtor(&url_parts);
            RETVAL_ZVAL(&real_uri, 0, 0);
            return;
        }
        zval_ptr_dtor(&url_parts);
    }

    RETURN_STRING("/");
}


PHP_METHOD(Slim_Router, getMatchedRoute)
{
	RETURN_MEMBER(getThis(), "_matchedRoute");
}
