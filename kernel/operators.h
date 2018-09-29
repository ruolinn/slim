#ifndef SLIM_KERNEL_OPERATORS_H
#define SLIM_KERNEL_OPERATORS_H

#include "php_slim.h"
#include "kernel/main.h"

#define SLIM_IS_EMPTY_STRING(var)		(Z_TYPE_P(var) <= IS_NULL || (Z_TYPE_P(var) == IS_STRING && !Z_STRLEN_P(var)))
#define SLIM_IS_NOT_EMPTY_STRING(var)	(Z_TYPE_P(var) == IS_STRING && Z_STRLEN_P(var))
#define SLIM_IS_EMPTY_ARR(var)			(Z_TYPE_P(var) == IS_ARRAY && !zend_hash_num_elements(Z_ARRVAL_P(var)))
#define SLIM_IS_NOT_EMPTY_ARR(var)		(Z_TYPE_P(var) == IS_ARRAY && zend_hash_num_elements(Z_ARRVAL_P(var)))
#define SLIM_IS_EMPTY(var)				(SLIM_IS_EMPTY_STRING(var) || SLIM_IS_EMPTY_ARR(var))
#define SLIM_IS_NOT_EMPTY(var)			(!SLIM_IS_EMPTY(var))

#endif
