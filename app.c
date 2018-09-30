#include "app.h"
#include "container.h"
#include "router.h"
#include "kernel/main.h"
#include "kernel/fcall.h"
#include "interned-strings.h"

zend_class_entry *slim_app_ce;

PHP_METHOD(Slim_App, __construct);
PHP_METHOD(Slim_App, bootstrapContainer);
PHP_METHOD(Slim_App, handle);

static const zend_function_entry slim_app_method_entry[] = {
    PHP_ME(Slim_App, __construct, NULL, ZEND_ACC_PUBLIC|ZEND_ACC_CTOR)
    PHP_ME(Slim_App, bootstrapContainer, NULL, ZEND_ACC_PROTECTED)
    PHP_ME(Slim_App, handle, NULL, ZEND_ACC_PUBLIC)
    PHP_FE_END
};

SLIM_INIT_CLASS(Slim_App)
{
    SLIM_REGISTER_CLASS_EX(Slim, App, app, slim_container_ce, slim_app_method_entry, 0);

    //zend_declare_property_null(slim_app_ce, SL("container"), ZEND_ACC_PROTECTED);

    return SUCCESS;
}

PHP_METHOD(Slim_App, __construct)
{
    SLIM_CALL_SELF(NULL, "bootstrapcontainer");
}

PHP_METHOD(Slim_App, bootstrapContainer)
{
    zval servicename = {}, definition = {};

    ZVAL_STR(&servicename, IS(router));
    ZVAL_STRING(&definition, "Slim\\Router");
    SLIM_CALL_SELF(NULL, "set", &servicename, &definition, &SLIM_G(z_true));
    zval_ptr_dtor(&definition);
}

PHP_METHOD(Slim_App, handle)
{
    zval *uri = NULL, router = {};

    slim_fetch_params(0, 0, 1, &uri);

    object_init_ex(&router, slim_router_ce);
    //SLIM_CALL_METHOD(return_value, &router, "handle", uri);

    ZVAL_COPY_VALUE(return_value, &router);
}
