#include "container.h"
#include "container/service.h"
#include "container/exception.h"
#include "kernel/main.h"
#include "kernel/array.h"
#include "kernel/object.h"
#include "kernel/fcall.h"

zend_class_entry *slim_container_ce;

PHP_METHOD(Slim_Container, set);
PHP_METHOD(Slim_Container, get);
PHP_METHOD(Slim_Container, setShared);
PHP_METHOD(Slim_Container, getShared);

static const zend_function_entry slim_container_method_entry[] = {
    PHP_ME(Slim_Container, set, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(Slim_Container, get, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(Slim_Container, setShared, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(Slim_Container, getShared, NULL, ZEND_ACC_PUBLIC)
    PHP_FE_END
};

SLIM_INIT_CLASS(Slim_Container)
{
    SLIM_REGISTER_CLASS(Slim, Container, container, slim_container_method_entry, 0);

    zend_declare_property_null(slim_container_ce, SL("_services"), ZEND_ACC_PROTECTED);
    zend_declare_property_null(slim_container_ce, SL("_sharedInstances"), ZEND_ACC_PROTECTED);

    return SUCCESS;
}

PHP_METHOD(Slim_Container, set)
{
    zval *name, *definition, *shared = NULL;

    slim_fetch_params(0, 2, 1, &name, &definition, &shared);

    if (!shared) {
        shared = &SLIM_G(z_false);
    }

    object_init_ex(return_value, slim_container_service_ce);
    SLIM_CALL_METHOD(NULL, return_value, "__construct", name, definition, shared);

    slim_update_property_array(getThis(), SL("_services"), name, return_value);

    if (zend_is_true(shared)) {
        slim_unset_property_array(getThis(), SL("_sharedInstances"), name);
    }
}

PHP_METHOD(Slim_Container, get)
{
    zval *name, *parameters = NULL, service = {};
    zend_class_entry *ce;

    slim_fetch_params(0, 1, 1, &name, &parameters);

    if (!parameters) {
        parameters = &SLIM_G(z_null);
    }

    if (slim_property_array_isset_fetch(&service, getThis(), SL("_services"), name, PH_READONLY)) {
        SLIM_CALL_METHOD(return_value, &service, "resolve", parameters, getThis());
        ce = (Z_TYPE_P(return_value) == IS_OBJECT) ? Z_OBJCE_P(return_value) : NULL;
    } else {
        if ((ce = slim_class_exists_ex(name, 1)) != NULL) {
            if (FAILURE == slim_create_instance_params_ce(return_value, ce, parameters)) {
                return;
            }
        }

        zend_throw_exception_ex(slim_container_exception_ce, 0, "Service '%s' was not found in the container", Z_STRVAL_P(name));
    }
}

PHP_METHOD(Slim_Container, setShared)
{
    zval *name, *definition;

    slim_fetch_params(0, 2, 0, &name, &definition);

    SLIM_CALL_METHOD(return_value, getThis(), "set", name, definition, &SLIM_G(z_true));
}

PHP_METHOD(Slim_Container, getShared)
{
    zval *name, *parameters = NULL, *noerror = NULL;

    slim_fetch_params(0, 1, 2, &name, &parameters, &noerror);

    if (!parameters) {
        parameters = &SLIM_G(z_null);
    }

    if (!noerror) {
        noerror = &SLIM_G(z_null);
    }

    if (slim_property_array_isset_fetch(return_value, getThis(), SL("_sharedInstances"), name, PH_COPY)) {
        slim_update_property_bool(getThis(), SL("_freshInstance"), 0);
    } else {
        SLIM_CALL_SELF(return_value, "get", name, parameters, noerror);
        if (zend_is_true(return_value)) {
            slim_update_property_bool(getThis(), SL("_freshInstance"), 1);
            slim_update_property_array(getThis(), SL("_sharedInstances"), name, return_value);
        }
    }
}

