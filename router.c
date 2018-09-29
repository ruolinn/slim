#include "router.h"
#include "kernel/main.h"
#include "kernel/object.h"
#include "kernel/fcall.h"

//#include <main/SAPI.h>

zend_class_entry *slim_router_ce;

PHP_METHOD(Slim_Router, __construct);
PHP_METHOD(Slim_Router, add);

static const zend_function_entry slim_router_method_entry[] = {
    PHP_ME(Slim_Router, __construct, NULL, ZEND_ACC_PUBLIC|ZEND_ACC_CTOR)
    PHP_ME(Slim_Router, add, NULL, ZEND_ACC_PUBLIC)
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
    zval *pattern, *callable, *methods;

    slim_fetch_params(0, 1, 2, &pattern, &callable, &methods);
}
