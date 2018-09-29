#ifndef SLIM_KERNEL_OBJECT_H
#define SLIM_KERNEL_OBJECT_H

#include "php_slim.h"
#include "kernel/main.h"

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

int slim_update_property(zval *obj, const char *property_name, uint32_t property_length, zval *value);
int slim_update_property_empty_array(zval *object, const char *property, uint32_t property_length);

#endif
