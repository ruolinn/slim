#include "router.h"
#include "router/route.h"
#include "router/exception.h"
#include "kernel/main.h"
#include "kernel/object.h"
#include "kernel/fcall.h"
#include "kernel/array.h"
#include "kernel/string.h"
#include "kernel/operators.h"
#include "kernel/exception.h"
#include "kernel/concat.h"
#include "kernel/debug.h"

//#include <main/SAPI.h>

#include "interned-strings.h"

zend_class_entry *slim_router_ce;

PHP_METHOD(Slim_Router, __construct);
PHP_METHOD(Slim_Router, setParams);
PHP_METHOD(Slim_Router, getParams);
PHP_METHOD(Slim_Router, add);
PHP_METHOD(Slim_Router, getMatchedRoute);
PHP_METHOD(Slim_Router, dispatch);
PHP_METHOD(Slim_Router, getRoutes);

static const zend_function_entry slim_router_method_entry[] = {
    PHP_ME(Slim_Router, __construct, NULL, ZEND_ACC_PUBLIC|ZEND_ACC_CTOR)
    PHP_ME(Slim_Router, setParams, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(Slim_Router, getParams, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(Slim_Router, add, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(Slim_Router, getMatchedRoute, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(Slim_Router, dispatch, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(Slim_Router, getRoutes, NULL, ZEND_ACC_PUBLIC)
    PHP_FE_END
};

SLIM_INIT_CLASS(Slim_Router)
{
    SLIM_REGISTER_CLASS(Slim, Router, router, slim_router_method_entry, 0);

    zend_declare_property_null(slim_router_ce, SL("_params"), ZEND_ACC_PROTECTED);
    zend_declare_property_null(slim_router_ce, SL("_routes"), ZEND_ACC_PROTECTED);
    zend_declare_property_null(slim_router_ce, SL("_matches"), ZEND_ACC_PROTECTED);
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

PHP_METHOD(Slim_Router, dispatch)
{
    zval *uri = NULL, *http_method, route_found = {}, routes = {}, *route, handled_uri;
    zval case_sensitive, service = {}, request = {}, matches = {}, parts = {};
    zval params = {}, params_str = {}, str_params = {}, params_merge = {}, default_params = {};
    zend_string *str_key;
    ulong idx;

    slim_fetch_params(0, 2, 0, &http_method, &uri);

    ZVAL_FALSE(&case_sensitive);

    // 可以添加过滤uri逻辑
    ZVAL_COPY_VALUE(&handled_uri, uri);

    ZVAL_FALSE(&route_found);

    slim_update_property_bool(getThis(), SL("_wasMatched"), 0);
    slim_update_property_null(getThis(), SL("_matchedRoute"));

    slim_read_property(&routes, getThis(), SL("_routes"), PH_NOISY|PH_READONLY);

    ZEND_HASH_REVERSE_FOREACH_VAL(Z_ARRVAL(routes), route) {
        zval pattern = {}, match_method = {}, case_pattern = {}, *position, paths = {};;

        SLIM_CALL_METHOD(&match_method, route, "ismethod", http_method);
        if (SLIM_IS_FALSE(&match_method)) {
            continue;
        }

        SLIM_CALL_METHOD(&pattern, route, "getcompiledpattern");

        ZVAL_NULL(&matches);
        if (Z_TYPE(pattern) == IS_STRING && Z_STRLEN(pattern) > 3 && Z_STRVAL(pattern)[1] == '^') {
            if (zend_is_true(&case_sensitive)) {
                SLIM_CONCAT_VS(&case_pattern, &pattern, "i");
                slim_preg_match(&route_found, &case_pattern, &handled_uri, &matches, 0, 0);
                zval_ptr_dtor(&case_pattern);
            } else {
                slim_preg_match(&route_found, &pattern, &handled_uri, &matches, 0, 0);
            }
        } else {
            ZVAL_BOOL(&route_found, slim_comparestr(&pattern, &handled_uri, &case_sensitive));
        }

        if (zend_is_true(&route_found)) {

            SLIM_CALL_METHOD(&paths, route, "getpaths");
            SLIM_ZVAL_DUP(&parts, &paths);

            if (Z_TYPE(matches) == IS_ARRAY && Z_TYPE(paths) == IS_ARRAY) {
                ZEND_HASH_FOREACH_KEY_VAL(Z_ARRVAL(paths), idx, str_key, position) {
                    zval tmp = {}, match_position = {};
                    if (str_key) {
                        ZVAL_STR(&tmp, str_key);
                    } else {
                        ZVAL_LONG(&tmp, idx);
                    }
                    if (!str_key || str_key->val[0] != '\0') {
                        if (slim_array_isset_fetch(&match_position, &matches, position, PH_READONLY)) {
                            slim_array_update(&parts, &tmp, &match_position, PH_COPY);
                        }
                    }
                } ZEND_HASH_FOREACH_END();

                slim_update_property(getThis(), SL("_matches"), &matches);
            }

            slim_update_property(getThis(), SL("_matchedRoute"), route);
            break;
        }
    } ZEND_HASH_FOREACH_END();

    slim_update_property_bool(getThis(), SL("_wasMatched"), zend_is_true(&route_found));
    SLIM_CALL_METHOD(NULL, getThis(), "setparams", &parts);

    if (!zend_is_true(&route_found)) {
        slim_update_property_null(getThis(), SL("_matches"));
        slim_update_property_null(getThis(), SL("_matchedRoute"));
        SLIM_THROW_EXCEPTION_STR(slim_router_route_exception_ce, "Not found");
        RETURN_FALSE;
    }

    if (zend_is_true(&route_found)) {
        RETURN_TRUE;
    }

    RETURN_FALSE;
}

PHP_METHOD(Slim_Router, getMatchedRoute)
{
	RETURN_MEMBER(getThis(), "_matchedRoute");
}

PHP_METHOD(Slim_Router, getRoutes)
{
    RETURN_MEMBER(getThis(), "_routes");
}


PHP_METHOD(Slim_Router, setParams)
{
    zval *params;

    slim_fetch_params(0, 1, 0, &params);

    slim_update_property(getThis(), SL("_params"), params);
    RETURN_THIS();
}


PHP_METHOD(Slim_Router, getParams)
{

    RETURN_MEMBER(getThis(), "_params");
}
