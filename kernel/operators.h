#ifndef SLIM_KERNEL_OPERATORS_H
#define SLIM_KERNEL_OPERATORS_H

#include "php_slim.h"
#include "kernel/main.h"

#define SLIM_IS_STRING(op1, op2) slim_compare_strict_string(op1, op2, strlen(op2))
#define SLIM_IS_LONG(op1, op2)   ((Z_TYPE_P(op1) == IS_LONG && Z_LVAL_P(op1) == op2) || slim_compare_strict_long(op1, op2))
#define SLIM_IS_DOUBLE(op1, op2) ((Z_TYPE_P(op1) == IS_DOUBLE && Z_DVAL_P(op1) == op2) || slim_compare_strict_double(op1, op2))

#define SLIM_IS_EMPTY_STRING(var)		(Z_TYPE_P(var) <= IS_NULL || (Z_TYPE_P(var) == IS_STRING && !Z_STRLEN_P(var)))
#define SLIM_IS_NOT_EMPTY_STRING(var)	(Z_TYPE_P(var) == IS_STRING && Z_STRLEN_P(var))
#define SLIM_IS_EMPTY_ARR(var)			(Z_TYPE_P(var) == IS_ARRAY && !zend_hash_num_elements(Z_ARRVAL_P(var)))
#define SLIM_IS_NOT_EMPTY_ARR(var)		(Z_TYPE_P(var) == IS_ARRAY && zend_hash_num_elements(Z_ARRVAL_P(var)))
#define SLIM_IS_EMPTY(var)				(SLIM_IS_EMPTY_STRING(var) || SLIM_IS_EMPTY_ARR(var))
#define SLIM_IS_NOT_EMPTY(var)			(!SLIM_IS_EMPTY(var))

#define SLIM_IS_EQUAL(op1, op2)      slim_is_equal(op1, op2)

#define SLIM_IS_FALSE(var)       (Z_TYPE_P(var) == IS_FALSE)
#define SLIM_IS_TRUE(var)        (Z_TYPE_P(var) == IS_TRUE)

#define SLIM_IS_NOT_FALSE(var)   (Z_TYPE_P(var) != IS_FALSE)
#define SLIM_IS_NOT_TRUE(var)    (Z_TYPE_P(var) != IS_TRUE)
#define SLIM_IS_BOOL(var)        (Z_TYPE_P(var) == IS_FALSE || Z_TYPE_P(var) == IS_TRUE)


void slim_cast(zval *result, zval *var, uint32_t type);
void slim_convert_to_object(zval *op);
zend_long slim_get_intval_ex(const zval *op);
double slim_get_doubleval_ex(const zval *op);
zend_bool slim_get_boolval_ex(const zval *op);

#define slim_get_numberval(z) (Z_TYPE_P(z) == IS_LONG ? Z_LVAL_P(z) : slim_get_doubleval(z))
#define slim_get_intval(z) (Z_TYPE_P(z) == IS_LONG ? Z_LVAL_P(z) : slim_get_intval_ex(z))
#define slim_get_doubleval(z) (Z_TYPE_P(z) == IS_DOUBLE ? Z_DVAL_P(z) : slim_get_doubleval_ex(z))
#define slim_get_boolval(z) (zend_is_true(z) ? 1 : 0);

int slim_and_function(zval *result, zval *left, zval *right);

int slim_compare_strict_string(zval *op1, const char *op2, int op2_length);
int slim_compare_strict_long(zval *op1, zend_long op2);
int slim_compare_strict_double(zval *op1, double op2);
int slim_compare_strict_bool(zval *op1, zend_bool op2);

int slim_is_equal(zval *op1, zval *op2);
int slim_is_equal_object(zval *obj1, zval *obj2);

#endif
