#ifndef SLIM_KERNEL_EXCEPTION_H
#define SLIM_KERNEL_EXCEPTION_H

#include "php_slim.h"

void slim_throw_exception_format(zend_class_entry *ce, const char *format, ...);
void slim_throw_exception_string(zend_class_entry *ce, const char *message) SLIM_ATTR_NONNULL;
void slim_throw_exception_zval(zend_class_entry *ce, zval *message) SLIM_ATTR_NONNULL;

#define SLIM_THROW_EXCEPTION_STR(class_entry, message)  slim_throw_exception_string(class_entry, message)
#define SLIM_THROW_EXCEPTION_ZVAL(class_entry, message) slim_throw_exception_zval(class_entry, message)
#define SLIM_THROW_EXCEPTION_FORMAT(class_entry, format, ...) slim_throw_exception_format(class_entry, format, __VA_ARGS__);

#endif
