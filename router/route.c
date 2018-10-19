#include "router/route.h"
#include "kernel/main.h"
#include "kernel/object.h"
#include "kernel/fcall.h"
#include "kernel/operators.h"
#include "kernel/string.h"
#include "kernel/framework/router.h"
#include "kernel/concat.h"
#include "kernel/array.h"

zend_class_entry *slim_router_route_ce;

PHP_METHOD(Slim_Router_Route, __construct);
PHP_METHOD(Slim_Router_Route, getCompiledPattern);
PHP_METHOD(Slim_Router_Route, getCallable);
PHP_METHOD(Slim_Router_Route, getMethods);
PHP_METHOD(Slim_Router_Route, isMethod);
PHP_METHOD(Slim_Router_Route, prepare);
PHP_METHOD(Slim_Router_Route, compilePattern);
PHP_METHOD(Slim_Router_Route, getPaths);
PHP_METHOD(Slim_Router_Route, middleware);
PHP_METHOD(Slim_Router_Route, getMiddleware);

static const zend_function_entry slim_router_route_method_entry[] = {
    PHP_ME(Slim_Router_Route, __construct, NULL, ZEND_ACC_PUBLIC|ZEND_ACC_CTOR)
    PHP_ME(Slim_Router_Route, getCompiledPattern, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(Slim_Router_Route, getCallable, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(Slim_Router_Route, getMethods, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(Slim_Router_Route, isMethod, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(Slim_Router_Route, prepare, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(Slim_Router_Route, compilePattern, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(Slim_Router_Route, getPaths, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(Slim_Router_Route, middleware, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(Slim_Router_Route, getMiddleware, NULL, ZEND_ACC_PUBLIC)
    PHP_FE_END
};

SLIM_INIT_CLASS(Slim_Router_Route)
{
    SLIM_REGISTER_CLASS(Slim\\Router, Route, router_route, slim_router_route_method_entry, 0);

    zend_declare_property_null(slim_router_route_ce, SL("_id"), ZEND_ACC_PROTECTED);
    zend_declare_property_null(slim_router_route_ce, SL("_pattern"), ZEND_ACC_PROTECTED);
    zend_declare_property_null(slim_router_route_ce, SL("_compiledPattern"), ZEND_ACC_PROTECTED);
    zend_declare_property_null(slim_router_route_ce, SL("_methods"), ZEND_ACC_PROTECTED);
    zend_declare_property_null(slim_router_route_ce, SL("_callable"), ZEND_ACC_PROTECTED);
    zend_declare_property_long(slim_router_route_ce, SL("_uniqueId"), 0, ZEND_ACC_STATIC|ZEND_ACC_PROTECTED);
    zend_declare_property_null(slim_router_route_ce, SL("_paths"), ZEND_ACC_PROTECTED);
    zend_declare_property_null(slim_router_route_ce, SL("_middleware"), ZEND_ACC_PROTECTED);

    return SUCCESS;
}

PHP_METHOD(Slim_Router_Route, __construct)
{
    zval *pattern, *callable = NULL, *http_methods = NULL, unique_id = {};

    slim_fetch_params(0, 3, 0, &http_methods, &pattern, &callable);

    SLIM_CALL_METHOD(NULL, getThis(), "prepare", pattern);

    slim_update_property(getThis(), SL("_methods"), http_methods);
    slim_update_property(getThis(), SL("_callable"), callable);


    slim_read_static_property_ce(&unique_id, slim_router_route_ce, SL("_uniqueId"), PH_READONLY);

    if (Z_TYPE(unique_id) == IS_NULL) {
        ZVAL_LONG(&unique_id, 0);
    }

    slim_update_property(getThis(), SL("_id"), &unique_id);

    increment_function(&unique_id);
    slim_update_static_property_ce(slim_router_route_ce, SL("_uniqueId"), &unique_id);
}

PHP_METHOD(Slim_Router_Route, getCompiledPattern)
{
    RETURN_MEMBER(getThis(), "_compiledPattern");
}

PHP_METHOD(Slim_Router_Route, getCallable)
{
    RETURN_MEMBER(getThis(), "_callable");
}

PHP_METHOD(Slim_Router_Route, getMethods)
{
    RETURN_MEMBER(getThis(), "_methods");
}

PHP_METHOD(Slim_Router_Route, isMethod)
{
    zval *http_method = NULL, methods = {}, *method = NULL;

    slim_fetch_params(0, 1, 0, &http_method);

    SLIM_CALL_METHOD(&methods, getThis(), "getmethods");

    if (Z_TYPE(methods) == IS_STRING) {
        is_equal_function(return_value, http_method, &methods);
        zval_ptr_dtor(&methods);
        return;
    }

    ZEND_HASH_FOREACH_VAL(Z_ARRVAL(methods), method) {
        if (SLIM_IS_EQUAL(http_method, method)) {
            RETURN_TRUE;
        }
    } ZEND_HASH_FOREACH_END();

    RETURN_FALSE;
}

PHP_METHOD(Slim_Router_Route, prepare)
{
    zval *pattern, pcre_pattern = {}, compiled_pattern = {}, route_paths = {};

    slim_fetch_params(0, 1, 0, &pattern);

    array_init(&route_paths);

    if (!slim_start_with_str(pattern, SL("#"))) {

        SLIM_CALL_METHOD(&pcre_pattern, getThis(), "compilepattern", pattern);

        if (slim_memnstr_str(pattern, SL("{"))) {
            slim_extract_named_params(&compiled_pattern, &pcre_pattern, &route_paths);
        } else {
            ZVAL_COPY(&compiled_pattern, &pcre_pattern);
        }
        zval_ptr_dtor(&pcre_pattern);
    } else {
        ZVAL_COPY(&compiled_pattern, pattern);
    }

    slim_update_property(getThis(), SL("_pattern"), pattern);

    slim_update_property(getThis(), SL("_compiledPattern"), &compiled_pattern);

    slim_update_property(getThis(), SL("_paths"), &route_paths);
    zval_ptr_dtor(&compiled_pattern);
    zval_ptr_dtor(&route_paths);
}

PHP_METHOD(Slim_Router_Route, compilePattern)
{
    zval *pattern, compiled_pattern = {};

    slim_fetch_params(0, 1, 0, &pattern);

    ZVAL_DUP(&compiled_pattern, pattern);

    if (slim_memnstr_str(&compiled_pattern, SL("("))) {
        SLIM_CONCAT_SVS(return_value, "#^", &compiled_pattern, "$#u");
        zval_ptr_dtor(&compiled_pattern);
        return;
    }

    if (slim_memnstr_str(&compiled_pattern, SL("["))) {
        SLIM_CONCAT_SVS(return_value, "#^", &compiled_pattern, "$#u");
        zval_ptr_dtor(&compiled_pattern);
        return;
    }

    if (slim_memnstr_str(&compiled_pattern, SL("{"))) {
        SLIM_CONCAT_SVS(return_value, "#^", &compiled_pattern, "$#u");
        zval_ptr_dtor(&compiled_pattern);
        return;
    }

    RETURN_CTOR(&compiled_pattern);
}

PHP_METHOD(Slim_Router_Route, getPaths)
{
    RETURN_MEMBER(getThis(), "_paths");
}

PHP_METHOD(Slim_Router_Route, middleware)
{
    zval *items, middleware = {}, _middleware = {}, merge_middleware = {};

    slim_fetch_params(0, 1, 0, &items);

    slim_read_property(&middleware, getThis(), SL("_middleware"), PH_COPY);
    if (Z_TYPE(middleware) == IS_NULL) {
        array_init(&middleware);
    }

    slim_fast_array_merge(&merge_middleware, &middleware, items);
    SLIM_CALL_FUNCTION(&_middleware, "array_unique", &merge_middleware);

    slim_update_property(getThis(), SL("_middleware"), &_middleware);
}

PHP_METHOD(Slim_Router_Route, getMiddleware)
{
    RETURN_MEMBER(getThis(), "_middleware");
}
