#include "container/service.h"
#include "kernel/main.h"
#include "kernel/object.h"

zend_class_entry *slim_container_service_ce;

PHP_METHOD(Slim_Container_Service, __construct);
PHP_METHOD(Slim_Container_Service, resolve);

static const zend_function_entry slim_container_service_method_entry[] = {
    PHP_ME(Slim_Container_Service, __construct, NULL, ZEND_ACC_PUBLIC|ZEND_ACC_CTOR)
    PHP_ME(Slim_Container_Service, resolve, NULL, ZEND_ACC_PUBLIC)
    PHP_FE_END
};

SLIM_INIT_CLASS(Slim_Container_Service)
{
    SLIM_REGISTER_CLASS(Slim\\Container, Service, container_service, slim_container_service_method_entry, 0);

    zend_declare_property_null(slim_container_service_ce, SL("_name"), ZEND_ACC_PROTECTED);
    zend_declare_property_null(slim_container_service_ce, SL("_definition"), ZEND_ACC_PROTECTED);
    zend_declare_property_bool(slim_container_service_ce, SL("_shared"), 0, ZEND_ACC_PROTECTED);
    zend_declare_property_bool(slim_container_service_ce, SL("_resolved"), 0, ZEND_ACC_PROTECTED);
    zend_declare_property_null(slim_container_service_ce, SL("_sharedInstance"), ZEND_ACC_PROTECTED);

    return SUCCESS;
}

PHP_METHOD(Slim_Container_Service, __construct)
{
    zval *name, *definition, *shared = NULL;

    slim_fetch_params(0, 2, 1, &name, &definition, &shared);

    slim_update_property(getThis(), SL("_name"), name);
    slim_update_property(getThis(), SL("_definition"), definition);

    if (shared) {
        SLIM_ENSURE_IS_BOOL(shared);
        slim_update_property(getThis(), SL("_shared"), shared);
    }
}

PHP_METHOD(Slim_Container_Service, resolve)
{
    zval *parameters = NULL, *container = NULL, name = {}, shared = {}, shared_instance = {}, definition = {}, builder = {};
    int found = 0, ishared = 0;

    slim_fetch_params(0, 0, 2, &parameters, &container);

    if (!parameters) {
        parameters = &SLIM_G(z_null);
    }

    if (!container) {
        container = &SLIM_G(z_null);
    }

    slim_read_property(&shared, getThis(), SL("_shared"), PH_READONLY);
    slim_read_property(&shared_instance, getThis(), SL("_sharedInstance"), PH_READONLY);

    ishared = zend_is_true(&shared);
    if (ishared && Z_TYPE(shared_instance) != IS_NULL) {
        RETURN_CTOR(&shared_instance);
    }

    slim_read_property(&definition, getThis(), SL("_definition"), PH_READONLY);

    if (Z_TYPE(definition) == IS_STRING) {
        if (slim_class_exists(&definition, 1) != NULL) {
            found = 1;
            if (Z_TYPE_P(parameters) == IS_ARRAY) {
                RETURN_ON_FAILURE(slim_create_instance_params(return_value, &definition, parameters));
            } else {
                RETURN_ON_FAILURE(slim_create_instance(return_value, &definition));
            }
        }
    }
}
