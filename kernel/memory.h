#ifndef SLIM_KERNEL_MEMORY_H
#define SLIM_KERNEL_MEMORY_H

#include "php_slim.h"
#include "kernel/main.h"
#include <stdint.h>

#define SLIM_ZVAL_DUP(d, v) ZVAL_DUP(d, v);

void slim_initialize_memory(zend_slim_globals *slim_globals_ptr);

#endif
