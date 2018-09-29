#include "router.h"
#include "router/route.h"
#include "kernel/main.h"
#include "kernel/object.h"
#include "kernel/fcall.h"

//#include <main/SAPI.h>

zend_class_entry *slim_router_ce;

PHP_METHOD(Slim_Router, __construct);
PHP_METHOD(Slim_Router, add);
PHP_METHOD(Slim_Router, handle);

static const zend_function_entry slim_router_method_entry[] = {
    PHP_ME(Slim_Router, __construct, NULL, ZEND_ACC_PUBLIC|ZEND_ACC_CTOR)
    PHP_ME(Slim_Router, add, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(Slim_Router, handle, NULL, ZEND_ACC_PUBLIC)
    PHP_FE_END
};

SLIM_INIT_CLASS(Slim_Router)
{
    SLIM_REGISTER_CLASS(Slim, Router, router, slim_router_method_entry, 0);

    zend_declare_property_null(slim_router_ce, SL("_params"), ZEND_ACC_PROTECTED);
    zend_declare_property_null(slim_router_ce, SL("_routes"), ZEND_ACC_PROTECTED);

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

