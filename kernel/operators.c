#include "operators.h"

double slim_get_doubleval_ex(const zval *op) {

    switch (Z_TYPE_P(op)) {
    case IS_ARRAY:
        return zend_hash_num_elements(Z_ARRVAL_P(op)) ? (double) 1 : 0;
        break;
    case IS_CALLABLE:
    case IS_RESOURCE:
    case IS_OBJECT:
        return (double) 1;
		case IS_LONG:
        return (double) Z_LVAL_P(op);
		case IS_TRUE:
        return (double) 1;
		case IS_FALSE:
        return (double) 0;
		case IS_DOUBLE:
        return Z_DVAL_P(op);
		case IS_STRING: {
        zend_long long_value;
        double double_value;
        zend_uchar type;

        ASSUME(Z_STRVAL_P(op) != NULL);
        type = is_numeric_string(Z_STRVAL_P(op), Z_STRLEN_P(op), &long_value, &double_value, 0);
        if (type == IS_LONG) {
            return long_value;
        }
        if (type == IS_DOUBLE) {
            return double_value;
        }
        return 0;
		}
    }

    return 0;
}

void slim_cast(zval *result, zval *var, uint32_t type){

    ZVAL_ZVAL(result, var, 1, 0);

    switch (type) {
		case IS_STRING:
        convert_to_string(result);
        break;
		case IS_LONG:
        convert_to_long(result);
        break;
		case IS_DOUBLE:
        convert_to_double(result);
        break;
        /*case IS_BOOL:
          convert_to_bool(result);
          break;*/
		case IS_ARRAY:
        if (Z_TYPE_P(result) != IS_ARRAY) {
            convert_to_array(result);
        }
        break;
    }

}

zend_long slim_get_intval_ex(const zval *op) {

    switch (Z_TYPE_P(op)) {
    case IS_ARRAY:
        return zend_hash_num_elements(Z_ARRVAL_P(op)) ? 1 : 0;
        break;

    case IS_CALLABLE:
    case IS_RESOURCE:
    case IS_OBJECT:
        return 1;

		case IS_LONG:
        return Z_LVAL_P(op);
		case IS_TRUE:
        return 1;
		case IS_FALSE:
        return 0;
		case IS_DOUBLE:
        return (zend_long) Z_DVAL_P(op);
		case IS_STRING: {
        zend_long long_value;
        double double_value;
        zend_uchar type;

        ASSUME(Z_STRVAL_P(op) != NULL);
        type = is_numeric_string(Z_STRVAL_P(op), Z_STRLEN_P(op), &long_value, &double_value, 0);
        if (type == IS_LONG) {
            return long_value;
        }
        if (type == IS_DOUBLE) {
            return (long)double_value;
        }
        return 0;
		}
    }

    return 0;
}

zend_bool slim_get_boolval_ex(const zval *op) {

    int type;
    zend_long long_value = 0;
    double double_value = 0;

    switch (Z_TYPE_P(op)) {
    case IS_ARRAY:
        return zend_hash_num_elements(Z_ARRVAL_P(op)) ? (zend_bool) 1 : 0;
        break;
    case IS_CALLABLE:
    case IS_RESOURCE:
    case IS_OBJECT:
        return (zend_bool) 1;
		case IS_LONG:
        return (Z_LVAL_P(op) ? (zend_bool) 1 : 0);
		case IS_TRUE:
        return (zend_bool) 1;
		case IS_FALSE:
        return (zend_bool) 0;
		case IS_DOUBLE:
        return (Z_DVAL_P(op) ? (zend_bool) 1 : 0);
		case IS_STRING:
        if ((type = is_numeric_string(Z_STRVAL_P(op), Z_STRLEN_P(op), &long_value, &double_value, 0))) {
            if (type == IS_LONG) {
                return (long_value ? (zend_bool) 1 : 0);
            } else {
                if (type == IS_DOUBLE) {
                    return (double_value ? (zend_bool) 1 : 0);
                } else {
                    return 0;
                }
            }
        }
    }

    return 0;
}

void slim_convert_to_object(zval *op) {
    convert_to_object(op);
}

int slim_compare_strict_string(zval *op1, const char *op2, int op2_length){

    switch (Z_TYPE_P(op1)) {
		case IS_STRING:
        if (!Z_STRLEN_P(op1) && !op2_length) {
            return 1;
        }
        if (Z_STRLEN_P(op1) != op2_length) {
            return 0;
        }
        return !zend_binary_strcmp(Z_STRVAL_P(op1), Z_STRLEN_P(op1), op2, op2_length);
		case IS_NULL:
        return !zend_binary_strcmp("", 0, op2, op2_length);
		case IS_TRUE:
        return !zend_binary_strcmp("1", strlen("1"), op2, op2_length);
		case IS_FALSE:
        return !zend_binary_strcmp("0", strlen("0"), op2, op2_length);
    }

    return 0;
}

int slim_compare_strict_long(zval *op1, zend_long op2){
    switch (Z_TYPE_P(op1)) {
		case IS_LONG:
        return Z_LVAL_P(op1) == op2;
		case IS_DOUBLE:
        return Z_LVAL_P(op1) == (double) op2;
		case IS_NULL:
        return 0 == op2;
		case IS_TRUE:
        return 1 == op2;
		case IS_FALSE:
        return 0 == op2;
		default: {
        zval result = {}, op2_tmp = {};
        ZVAL_LONG(&op2_tmp, op2);
        is_equal_function(&result, op1, &op2_tmp);
        return Z_TYPE(result) == IS_TRUE ? 1 : 0;
		}
    }

    return 0;
}

int slim_compare_strict_double(zval *op1, double op2) {

    switch (Z_TYPE_P(op1)) {
		case IS_LONG:
        return Z_LVAL_P(op1) == (zend_long) op2;
		case IS_DOUBLE:
        return Z_DVAL_P(op1) == op2;
		case IS_NULL:
        return 0 == op2;
		case IS_TRUE:
        return 1 == op2;
		case IS_FALSE:
        return 0 == op2;
		default:
        {
            zval result = {}, op2_tmp = {};
            ZVAL_DOUBLE(&op2_tmp, op2);
            is_equal_function(&result, op1, &op2_tmp);
            return Z_TYPE(result) == IS_TRUE ? 1 : 0;
        }
    }

    return 0;
}

int slim_compare_strict_bool(zval *op1, zend_bool op2) {

    switch (Z_TYPE_P(op1)) {
		case IS_LONG:
        return (Z_LVAL_P(op1) ? 1 : 0) == op2;
		case IS_DOUBLE:
        return (Z_DVAL_P(op1) ? 1 : 0) == op2;
		case IS_NULL:
        return 0 == op2;
		case IS_TRUE:
        return 1 == op2;
		default:
        {
            zval result = {}, op2_tmp = {};
            ZVAL_BOOL(&op2_tmp, op2);
            is_equal_function(&result, op1, &op2_tmp);
            return Z_TYPE(result) == IS_TRUE ? 1 : 0;
        }
    }

    return 0;
}

int slim_and_function(zval *result, zval *left, zval *right){
    int istrue = zend_is_true(left) && zend_is_true(right);
    ZVAL_BOOL(result, istrue);
    return SUCCESS;
}

int slim_is_equal(zval *op1, zval *op2)
{
    zval result = {};
    if (Z_TYPE_P(op1) == IS_STRING && Z_TYPE_P(op1) == Z_TYPE_P(op2)) {
        return !zend_binary_strcmp(Z_STRVAL_P(op1), Z_STRLEN_P(op1), Z_STRVAL_P(op2), Z_STRLEN_P(op2));
    }
    is_equal_function(&result, op1, op2);
    return Z_TYPE(result) == IS_TRUE ? 1 : 0;
}
