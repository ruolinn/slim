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

    return SUCCESS;
}

PHP_METHOD(Slim_App, __construct)
{
    SLIM_CALL_SELF(NULL, "bootstrapcontainer");
}

PHP_METHOD(Slim_App, bootstrapContainer)
{
    zval service = {}, definition = {};

    ZVAL_STR(&service, IS(router));
    ZVAL_STRING(&definition, "Slim\\Router");
    SLIM_CALL_SELF(NULL, "set", &service, &definition, &SLIM_G(z_true));
    zval_ptr_dtor(&definition);
}

PHP_METHOD(Slim_App, handle)
{
    zval *uri = NULL, service = {}, router = {};

    slim_fetch_params(0, 0, 1, &uri);

    if (!uri) {
        uri = &SLIM_G(z_null);
    }

    ZVAL_STR(&service, IS(router));
    SLIM_CALL_SELF(&router, "getshared", &service);

    SLIM_CALL_METHOD(NULL, &router, "handle", uri);
}
