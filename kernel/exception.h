#ifndef SLIM_KERNEL_EXCEPTION_H
#define SLIM_KERNEL_EXCEPTION_H

#include "php_slim.h"

void slim_throw_exception_format(zend_class_entry *ce, const char *format, ...);
void slim_throw_exception_string(zend_class_entry *ce, const char *message) SLIM_ATTR_NONNULL;

#endif
