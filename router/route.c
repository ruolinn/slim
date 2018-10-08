#include "router/route.h"
#include "kernel/main.h"
#include "kernel/object.h"

zend_class_entry *slim_router_route_ce;

PHP_METHOD(Slim_Router_Route, __construct);
PHP_METHOD(Slim_Router_Route, getCompiledPattern);
PHP_METHOD(Slim_Router_Route, getCallable);

static const zend_function_entry slim_router_route_method_entry[] = {
    PHP_ME(Slim_Router_Route, __construct, NULL, ZEND_ACC_PUBLIC|ZEND_ACC_CTOR)
    PHP_ME(Slim_Router_Route, getCompiledPattern, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(Slim_Router_Route, getCallable, NULL, ZEND_ACC_PUBLIC)
    PHP_FE_END
};

SLIM_INIT_CLASS(Slim_Router_Route)
{
    SLIM_REGISTER_CLASS(Slim\\Router, Route, router_route, slim_router_route_method_entry, 0);

    zend_declare_property_null(slim_router_route_ce, SL("_pattern"), ZEND_ACC_PROTECTED);
    zend_declare_property_null(slim_router_route_ce, SL("_compiledPattern"), ZEND_ACC_PROTECTED);
    zend_declare_property_null(slim_router_route_ce, SL("_methods"), ZEND_ACC_PROTECTED);
    zend_declare_property_null(slim_router_route_ce, SL("_callable"), ZEND_ACC_PROTECTED);

    return SUCCESS;
}

PHP_METHOD(Slim_Router_Route, __construct)
{
    zval *pattern, *callable = NULL, *http_methods = NULL, unique_id = {};
    zval compiled_pattern = {};

    slim_fetch_params(0, 3, 0, &http_methods, &pattern, &callable);

    ZVAL_COPY(&compiled_pattern, pattern);

    slim_update_property(getThis(), SL("_methods"), http_methods);
    slim_update_property(getThis(), SL("_pattern"), pattern);
    slim_update_property(getThis(), SL("_callable"), callable);

    slim_update_property(getThis(), SL("_compiledPattern"), &compiled_pattern);
    zval_ptr_dtor(&compiled_pattern);

}

PHP_METHOD(Slim_Router_Route, getCompiledPattern)
{
    RETURN_MEMBER(getThis(), "_compiledPattern");
}

PHP_METHOD(Slim_Router_Route, getCallable)
{
	RETURN_MEMBER(getThis(), "_callable");
}
