#ifndef SLIM_KERNEL_DEBUG_H
#define SLIM_KERNEL_DEBUG_H

#include "php_slim.h"

#include <stdio.h>
#include <ext/standard/php_var.h>

#define SLIM_DUMP(v) slim_dump(v)

int slim_dump(zval *uservar);

#endif
