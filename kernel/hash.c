#include "kernel/hash.h"

zval* slim_hash_get(HashTable *ht, const zval *key, int type)
{
	zval *ret = NULL, value = {};

	switch (Z_TYPE_P(key)) {
		case IS_RESOURCE:
			zend_error(E_STRICT, "Resource ID#%ld used as offset, casting to integer (%ld)", Z_LVAL_P(key), Z_LVAL_P(key));
			/* no break */
		case IS_LONG:
		case IS_DOUBLE:
		case IS_TRUE:
		case IS_FALSE: {
			ulong index = 0;
			if ((Z_TYPE_P(key) == IS_TRUE)) {
				index = 1;
			} else if ((Z_TYPE_P(key) == IS_FALSE)) {
				index = 0;
			} else {
				index = (Z_TYPE_P(key) == IS_DOUBLE) ? ((long int)Z_DVAL_P(key)) : Z_LVAL_P(key);
			}

			if ((ret = zend_hash_index_find(ht, index)) == NULL) {
				switch (type) {
					case BP_VAR_R:
						zend_error(E_NOTICE, "Undefined offset: %ld", index);
						/* no break */
					case BP_VAR_UNSET:
					case BP_VAR_IS: {
						TSRMLS_FETCH();
						ret = &EG(uninitialized_zval);
						break;
					}

					case BP_VAR_RW:
						zend_error(E_NOTICE, "Undefined offset: %ld", index);
						/* no break */
					case BP_VAR_W: {
						ZVAL_NULL(&value);
						zend_hash_index_update(ht, index, &value);
						break;
					}
				}
			}

			return ret;
		}

		case IS_STRING:
			if ((ret = zend_symtable_find(ht, Z_STR_P(key)))  == NULL) {
				switch (type) {
					case BP_VAR_R:
						zend_error(E_NOTICE, "Undefined offset: %s", Z_STRVAL_P(key));
						/* no break */
					case BP_VAR_UNSET:
					case BP_VAR_IS: {
						TSRMLS_FETCH();
						ret = &EG(uninitialized_zval);
						break;
					}

					case BP_VAR_RW:
						zend_error(E_NOTICE, "Undefined offset: %s", Z_STRVAL_P(key));
						/* no break */
					case BP_VAR_W: {
						ZVAL_NULL(&value);
						zend_symtable_update(ht, Z_STR_P(key), &value);
						break;
					}
				}
			}

			return ret;

		default: {
			TSRMLS_FETCH();
			zend_error(E_WARNING, "Illegal offset type");
			return (type == BP_VAR_W || type == BP_VAR_RW) ? &EG(error_zval) : &EG(uninitialized_zval);
		}
	}
}
