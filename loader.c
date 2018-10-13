#include "loader.h"
#include "kernel/main.h"
#include "kernel/array.h"
#include "kernel/object.h"

#include "interned-strings.h"

zend_class_entry *slim_loader_ce;

PHP_METHOD(Slim_Loader, __construct);
PHP_METHOD(Slim_Loader, registerNamespaces);
PHP_METHOD(Slim_Loader, getNamespaces);
PHP_METHOD(Slim_Loader, register);

static const zend_function_entry slim_loader_method_entry[] = {
    PHP_ME(Slim_Loader, __construct, NULL, ZEND_ACC_PUBLIC|ZEND_ACC_CTOR)
    PHP_ME(Slim_Loader, registerNamespaces, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(Slim_Loader, getNamespaces, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(Slim_Loader, register, NULL, ZEND_ACC_PUBLIC)
    PHP_FE_END
};

SLIM_INIT_CLASS(Slim_Loader)
{
    SLIM_REGISTER_CLASS(Slim, Loader, loader, slim_loader_method_entry, 0);

    zend_declare_property_null(slim_loader_ce, SL("_default"), ZEND_ACC_PROTECTED|ZEND_ACC_STATIC);
    zend_declare_property_null(slim_loader_ce, SL("_foundPath"), ZEND_ACC_PROTECTED);
    zend_declare_property_null(slim_loader_ce, SL("_checkedPath"), ZEND_ACC_PROTECTED);
    zend_declare_property_null(slim_loader_ce, SL("_prefixes"), ZEND_ACC_PROTECTED);
    zend_declare_property_null(slim_loader_ce, SL("_classes"), ZEND_ACC_PROTECTED);
    zend_declare_property_null(slim_loader_ce, SL("_extensions"), ZEND_ACC_PROTECTED);
    zend_declare_property_null(slim_loader_ce, SL("_namespaces"), ZEND_ACC_PROTECTED);
    zend_declare_property_null(slim_loader_ce, SL("_directories"), ZEND_ACC_PROTECTED);
    zend_declare_property_bool(slim_loader_ce, SL("_registered"), 0, ZEND_ACC_PROTECTED);

    return SUCCESS;
}

PHP_METHOD(Slim_Loader, __construct)
{
    zval extensions = {}, default_loader = {};

    array_init_size(&extensions, 1);
    slim_array_append_string(&extensions, IS(php), PH_COPY);

    slim_update_property(getThis(), SL("_extensions"), &extensions);
    zval_ptr_dtor(&extensions);

    slim_read_static_property_ce(&default_loader, slim_loader_ce, SL("_default"), PH_READONLY);
    if (Z_TYPE(default_loader) == IS_NULL) {
        slim_update_static_property_ce(slim_loader_ce, SL("_default"), getThis());
    }
}

PHP_METHOD(Slim_Loader, registerNamespaces)
{
    zval *namespaces, *merge = NULL, current_namespaces = {}, merged_namespaces = {};

    slim_fetch_params(0, 1, 1, &namespaces, &merge);

    if (merge && zend_is_true(merge)) {
        slim_read_property(&current_namespaces, getThis(), SL("_namespaces"), PH_NOISY|PH_READONLY);
        if (Z_TYPE(current_namespaces) == IS_ARRAY) {
            slim_fast_array_merge(&merged_namespaces, &current_namespaces, namespaces);
        } else {
            ZVAL_COPY_VALUE(&merged_namespaces, namespaces);
        }

        slim_update_property(getThis(), SL("_namespaces"), &merged_namespaces);
    } else {
        slim_update_property(getThis(), SL("_namespaces"), namespaces);
    }

    RETURN_THIS();
}

PHP_METHOD(Slim_Loader, getNamespaces)
{
    RETURN_MEMBER(getThis(), "_namespaces");
}

