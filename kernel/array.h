#ifndef SLIM_KERNEL_ARRAY_H
#define SLIM_KERNEL_ARRAY_H

#include "php_slim.h"
#include "kernel/memory.h"

int slim_array_append(zval *arr, zval *value, int flags);

#endif
