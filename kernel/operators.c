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
