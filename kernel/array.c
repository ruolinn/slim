#include "kernel/array.h"

int slim_array_append(zval *arr, zval *value, int flags) {

    if (Z_TYPE_P(arr) != IS_ARRAY) {
        zend_error(E_WARNING, "Cannot use a scalar value as an array");
        return FAILURE;
    }

    if ((flags & PH_SEPARATE) == PH_SEPARATE) {
        SEPARATE_ZVAL_IF_NOT_REF(arr);
    }

    if ((flags & PH_COPY) == PH_COPY) {
        Z_TRY_ADDREF_P(value);
    }

    return add_next_index_zval(arr, value);
}

int ZEND_FASTCALL slim_array_isset_fetch(zval *fetched, const zval *arr, const zval *index, int flags)
{
    return slim_array_fetch(fetched, arr, index, flags) == SUCCESS ? 1 : 0;
}

int ZEND_FASTCALL slim_array_isset_fetch_str(zval *fetched, const zval *arr, const char *index, uint index_length, int flags)
{
    zval z_index = {};
    int status;
    ZVAL_STRINGL(&z_index, index, index_length);

    status = slim_array_isset_fetch(fetched, arr, &z_index, flags);
    zval_ptr_dtor(&z_index);
    return status;
}


int slim_array_fetch(zval *return_value, const zval *arr, const zval *index, int flags){

	zval *zv;
	HashTable *ht;
	int result = SUCCESS, found = 0;
	ulong uidx = 0;
	char *sidx = NULL;

	if (Z_TYPE_P(arr) == IS_ARRAY) {
		ht = Z_ARRVAL_P(arr);
		switch (Z_TYPE_P(index)) {
			case IS_NULL:
				found = (zv = zend_hash_str_find(ht, SL(""))) != NULL;
				sidx   = "";
				break;

			case IS_DOUBLE:
				uidx   = (ulong)Z_DVAL_P(index);
				found  = (zv = zend_hash_index_find(ht, uidx)) != NULL;
				break;

			case IS_LONG:
			case IS_RESOURCE:
				uidx   = Z_LVAL_P(index);
				found  = (zv = zend_hash_index_find(ht, uidx)) != NULL;
				break;

			case IS_FALSE:
				uidx = 0;
				found  = (zv = zend_hash_index_find(ht, uidx)) != NULL;
				break;

			case IS_TRUE:
				uidx = 1;
				found  = (zv = zend_hash_index_find(ht, uidx)) != NULL;
				break;

			case IS_STRING:
				sidx   = Z_STRLEN_P(index) ? Z_STRVAL_P(index) : "";
				found  = (zv = zend_symtable_str_find(ht, Z_STRVAL_P(index), Z_STRLEN_P(index))) != NULL;
				break;

			default:
				if ((flags & PH_NOISY) == PH_NOISY) {
					zend_error(E_WARNING, "Illegal offset type");
				}
				result = FAILURE;
				break;
		}

		if (result != FAILURE && found == 1) {
			/*
			if (EXPECTED(Z_TYPE_P(zv) == IS_REFERENCE)) {
				zv = Z_REFVAL_P(zv);
			}
			if (Z_TYPE_P(zv) == IS_INDIRECT) {
				zv = Z_INDIRECT_P(zv);
			}
			*/
			if ((flags & PH_SEPARATE) == PH_SEPARATE) {
				SEPARATE_ZVAL_IF_NOT_REF(zv);
				ZVAL_COPY_VALUE(return_value, zv);
			} else if ((flags & PH_CTOR) == PH_CTOR) {
				ZVAL_DUP(return_value, zv);
			} else if ((flags & PH_READONLY) == PH_READONLY) {
				ZVAL_COPY_VALUE(return_value, zv);
			} else {
				ZVAL_COPY(return_value, zv);
			}
			return SUCCESS;
		}

		if ((flags & PH_NOISY) == PH_NOISY) {
			if (sidx == NULL) {
				zend_error(E_NOTICE, "Undefined index: %ld", uidx);
			} else {
				zend_error(E_NOTICE, "Undefined index: %s", sidx);
			}
		}
	}

	ZVAL_NULL(return_value);
	return FAILURE;
}


int slim_array_fetch_long(zval *return_value, const zval *arr, ulong index, int flags)
{
    zval *zv;

    if (likely(Z_TYPE_P(arr) == IS_ARRAY)) {
        if ((zv = zend_hash_index_find(Z_ARRVAL_P(arr), index)) != NULL) {
            if ((flags & PH_SEPARATE) == PH_SEPARATE) {
                SEPARATE_ZVAL_IF_NOT_REF(zv);
                ZVAL_COPY_VALUE(return_value, zv);
            } else if ((flags & PH_READONLY) == PH_READONLY) {
                ZVAL_COPY_VALUE(return_value, zv);
            } else {
                ZVAL_COPY(return_value, zv);
            }
            return SUCCESS;
        }

        if (flags == PH_NOISY) {
            zend_error(E_NOTICE, "Undefined index: %lu", index);
        }
    } else {
        if (flags == PH_NOISY) {
            zend_error(E_NOTICE, "Cannot use a scalar value as an array");
        }
    }

    ZVAL_NULL(return_value);

    return FAILURE;
}
