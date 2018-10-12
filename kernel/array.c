#include "kernel/array.h"

#include <ext/standard/php_array.h>

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


int ZEND_FASTCALL slim_array_unset(zval *arr, const zval *index, int flags) {

    HashTable *ht;

    if (Z_TYPE_P(arr) != IS_ARRAY) {
        return FAILURE;
    }

    if ((flags & PH_SEPARATE) == PH_SEPARATE) {
        SEPARATE_ZVAL_IF_NOT_REF(arr);
    }

    ht = Z_ARRVAL_P(arr);

    switch (Z_TYPE_P(index)) {
		case IS_NULL:
        return (zend_hash_str_del(ht, "", 1) == SUCCESS);

		case IS_DOUBLE:
        return (zend_hash_index_del(ht, (ulong)Z_DVAL_P(index)) == SUCCESS);

		case IS_TRUE:
        return (zend_hash_index_del(ht, 1) == SUCCESS);

		case IS_FALSE:
        return (zend_hash_index_del(ht, 0) == SUCCESS);

		case IS_LONG:
		case IS_RESOURCE:
        return (zend_hash_index_del(ht, Z_LVAL_P(index)) == SUCCESS);

		case IS_STRING:
        return (zend_symtable_del(ht, Z_STR_P(index)) == SUCCESS);

		default:
        zend_error(E_WARNING, "Illegal offset type");
        return 0;
    }
}

int slim_array_update_long(zval *arr, ulong index, zval *value, int flags)
{
	zval new_value = {};

	if (Z_TYPE_P(arr) != IS_ARRAY) {
		zend_error(E_WARNING, "Cannot use a scalar value as an array");
		return FAILURE;
	}

	if ((flags & PH_CTOR) == PH_CTOR) {
		ZVAL_DUP(&new_value, value);
		value = &new_value;
	} else if ((flags & PH_COPY) == PH_COPY) {
		Z_TRY_ADDREF_P(value);
	}

	if ((flags & PH_SEPARATE) == PH_SEPARATE) {
		SEPARATE_ZVAL_IF_NOT_REF(arr);
	}

	return zend_hash_index_update(Z_ARRVAL_P(arr), index, value) ? SUCCESS : FAILURE;
}

void slim_fast_array_merge(zval *return_value, zval *array1, zval *array2) {

    int init_size, num;

    if (Z_TYPE_P(array1) != IS_ARRAY) {
        zend_error(E_WARNING, "First argument is not an array");
        RETURN_NULL();
    }

    if (Z_TYPE_P(array2) != IS_ARRAY) {
        zend_error(E_WARNING, "Second argument is not an array");
        RETURN_NULL();
    }

    init_size = zend_hash_num_elements(Z_ARRVAL_P(array1));
    num = zend_hash_num_elements(Z_ARRVAL_P(array2));
    if (num > init_size) {
        init_size = num;
    }

    array_init_size(return_value, init_size);

    php_array_merge(Z_ARRVAL_P(return_value), Z_ARRVAL_P(array1));
    php_array_merge(Z_ARRVAL_P(return_value), Z_ARRVAL_P(array2));
}


int ZEND_FASTCALL slim_array_isset(const zval *arr, const zval *index)
{
    HashTable *h;

    if (Z_TYPE_P(arr) != IS_ARRAY) {
        return 0;
    }

    h = Z_ARRVAL_P(arr);
    switch (Z_TYPE_P(index)) {
		case IS_NULL:
        return zend_hash_str_exists(h, SL(""));

		case IS_DOUBLE:
        return zend_hash_index_exists(h, (ulong)Z_DVAL_P(index));

		case IS_TRUE:
		case IS_FALSE:
        return zend_hash_index_exists(h, Z_TYPE_P(index) == IS_TRUE ? 1 : 0);

		case IS_LONG:
		case IS_RESOURCE:
        return zend_hash_index_exists(h, Z_LVAL_P(index));

		case IS_STRING:
        return zend_symtable_exists(h, Z_STR_P(index));

		default:
        zend_error(E_WARNING, "Illegal offset type");
        return 0;
    }
}

int slim_array_fetch_str(zval *return_value, const zval *arr, const char *index, uint index_length, int flags)
{

    zval *zv;

    if (likely(Z_TYPE_P(arr) == IS_ARRAY)) {
        if ((zv = zend_hash_str_find(Z_ARRVAL_P(arr), index, index_length)) != NULL) {
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

        if ((flags & PH_NOISY) == PH_NOISY) {
            zend_error(E_NOTICE, "Undefined index: %s", index);
        }
    } else {
        if ((flags & PH_NOISY) == PH_NOISY) {
            zend_error(E_NOTICE, "Cannot use a scalar value as an array");
        }
    }

    ZVAL_NULL(return_value);

    return FAILURE;
}

int slim_array_update(zval *arr, const zval *index, zval *value, int flags)
{
	zval new_value = {};
	HashTable *ht;

	if (Z_TYPE_P(arr) != IS_ARRAY) {
		zend_error(E_WARNING, "Cannot use a scalar value as an array (2)");
		return FAILURE;
	}

	if ((flags & PH_CTOR) == PH_CTOR) {
		ZVAL_DUP(&new_value, value);
		value = &new_value;
	}

	if ((flags & PH_SEPARATE) == PH_SEPARATE) {
		SEPARATE_ZVAL_IF_NOT_REF(arr);
	}

	if ((flags & PH_COPY) == PH_COPY) {
		Z_TRY_ADDREF_P(value);
	}

	ht = Z_ARRVAL_P(arr);

	return slim_array_update_hash(ht, index, value, flags);
}

int slim_array_update_hash(HashTable *ht, const zval *index, zval *value, int flags)
{
	int status;

	switch (Z_TYPE_P(index)) {
		case IS_NULL:
			status = zend_symtable_update(ht, ZSTR_EMPTY_ALLOC(), value) ? SUCCESS : FAILURE;
			break;

		case IS_DOUBLE:
			status = zend_hash_index_update(ht, zend_dval_to_lval(Z_DVAL_P(index)), value) ? SUCCESS : FAILURE;
			break;

		case IS_TRUE:
			status = zend_hash_index_update(ht, 1, value) ? SUCCESS : FAILURE;
			break;

		case IS_FALSE:
			status = zend_hash_index_update(ht, 0, value) ? SUCCESS : FAILURE;
			break;

		case IS_LONG:
			status = zend_hash_index_update(ht, Z_LVAL_P(index), value) ? SUCCESS : FAILURE;
			break;

		case IS_RESOURCE:
			zend_error(E_NOTICE, "Resource ID#%d used as offset, casting to integer (%d)", Z_RES_HANDLE_P(index), Z_RES_HANDLE_P(index));
			status = zend_hash_index_update(ht, Z_RES_HANDLE_P(index), value) ? SUCCESS : FAILURE;
			break;

		case IS_STRING:
			status = zend_symtable_update(ht, Z_STR_P(index), value) ? SUCCESS : FAILURE;
			break;

		default:
			zend_error(E_WARNING, "Illegal offset type");
			status = FAILURE;
			break;
	}

	return status;
}

int ZEND_FASTCALL slim_array_isset_fetch_long(zval *fetched, const zval *arr, ulong index, int flags)
{
	zval z_index = {};
	int status;
	ZVAL_LONG(&z_index, index);

	status = slim_array_isset_fetch(fetched, arr, &z_index, flags);
	return status;
}

int slim_array_update_str(zval *arr, const char *index, uint index_length, zval *value, int flags)
{
	zval new_value = {}, key = {};
	int status;

	if (Z_TYPE_P(arr) != IS_ARRAY) {
		zend_error(E_WARNING, "Cannot use a scalar value as an array (3)");
		return FAILURE;
	}

	if ((flags & PH_CTOR) == PH_CTOR) {
		ZVAL_DUP(&new_value, value);
		value = &new_value;
		Z_TRY_DELREF(new_value);
	} else if ((flags & PH_COPY) == PH_COPY) {
		Z_TRY_ADDREF_P(value);
	}

	if ((flags & PH_SEPARATE) == PH_SEPARATE) {
		SEPARATE_ZVAL_IF_NOT_REF(arr);
	}

	ZVAL_STRINGL(&key, index, index_length);
	status = slim_array_update_hash(Z_ARRVAL_P(arr), &key, value, flags);
	zval_ptr_dtor(&key);
	return status;
}

int ZEND_FASTCALL slim_array_unset_str(zval *arr, const char *index, uint index_length, int flags)
{
    if (Z_TYPE_P(arr) != IS_ARRAY) {
        return FAILURE;
    }

    if ((flags & PH_SEPARATE) == PH_SEPARATE) {
        SEPARATE_ZVAL_IF_NOT_REF(arr);
    }

    return zend_hash_str_del(Z_ARRVAL_P(arr), index, index_length);
}
