#ifndef SLIM_KERNEL_OBJECT_H
#define SLIM_KERNEL_OBJECT_H

#include "php_slim.h"
#include "kernel/main.h"

void slim_get_class_ns(zval *result, const zval *object, int lower);
void slim_get_ns_class(zval *result, const zval *object, int lower);

zend_class_entry *slim_class_exists(const zval *class_name, int autoload);
zend_class_entry *slim_class_exists_ex(const zval *class_name, int autoload);
zend_class_entry *slim_class_str_exists(const char *class_name, uint32_t class_len, int autoload);

static inline zend_class_entry *slim_lookup_class_ce(zend_class_entry *ce, const char *property_name, uint32_t property_length) {

    zend_class_entry *original_ce = ce;

    while (ce) {
        if (zend_hash_str_exists(&ce->properties_info, property_name, property_length)) {
            return ce;
        }
        ce = ce->parent;
    }
    return original_ce;
}

int slim_read_property(zval *result, zval *object, const char *property_name, uint32_t property_length, int flags);

int slim_update_property(zval *obj, const char *property_name, uint32_t property_length, zval *value);
int slim_update_property_null(zval *obj, const char *property_name, uint32_t property_length);
int slim_update_property_empty_array(zval *object, const char *property, uint32_t property_length);
int slim_update_property_array_append(zval *object, const char *property, uint32_t property_length, zval *value);
int slim_update_property_bool(zval *obj, const char *property_name, uint32_t property_length, int value);
int slim_update_property_array(zval *object, const char *property, uint32_t property_length, const zval *index, zval *value);

int slim_unset_property_array(zval *object, const char *property, uint32_t property_length, const zval *index);

int slim_create_instance_params_ce(zval *return_value, zend_class_entry *ce, zval *params);
int slim_create_instance(zval *return_value, const zval *class_name);
int slim_create_instance_params(zval *return_value, const zval *class_name, zval *params);

int slim_property_isset_fetch(zval *fetched, zval *object, const char *property_name, size_t property_length, int flags);
int slim_property_array_isset_fetch(zval *fetched, zval *object, const char *property_name, size_t property_length, const zval *index, int flags);

int slim_property_exists(zval *object, const char *property_name, uint32_t property_length, int flags);
#define slim_isset_property(object, property_name, property_length) slim_property_exists(object, property_name, property_length, PH_BOTH)

int slim_clone(zval *destination, zval *obj);

int slim_method_exists(const zval *object, const zval *method_name);
int slim_method_exists_ex(const zval *object, const char *method_name, uint32_t method_len);

int slim_read_static_property(zval *return_value, const char *class_name, uint32_t class_length, const char *property_name, uint32_t property_length, int flags);
int slim_read_static_property_ce(zval *return_value, zend_class_entry *ce, const char *property, uint32_t len, int flags);

int slim_update_static_property_ce(zend_class_entry *ce, const char *name, uint32_t len, zval *value);

int slim_isset_property_array(zval *object, const char *property, uint32_t property_length, const zval *index);

int slim_instance_of_ev(const zval *object, const zend_class_entry *ce);

#endif
