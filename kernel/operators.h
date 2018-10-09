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

#define SLIM_IS_FALSE(var)       (Z_TYPE_P(var) == IS_FALSE)
#define SLIM_IS_TRUE(var)        (Z_TYPE_P(var) == IS_TRUE)

void slim_cast(zval *result, zval *var, uint32_t type);
void slim_convert_to_object(zval *op);
zend_long slim_get_intval_ex(const zval *op);
double slim_get_doubleval_ex(const zval *op);
zend_bool slim_get_boolval_ex(const zval *op);

#define slim_get_numberval(z) (Z_TYPE_P(z) == IS_LONG ? Z_LVAL_P(z) : slim_get_doubleval(z))
#define slim_get_intval(z) (Z_TYPE_P(z) == IS_LONG ? Z_LVAL_P(z) : slim_get_intval_ex(z))
#define slim_get_doubleval(z) (Z_TYPE_P(z) == IS_DOUBLE ? Z_DVAL_P(z) : slim_get_doubleval_ex(z))
#define slim_get_boolval(z) (zend_is_true(z) ? 1 : 0);

#endif
