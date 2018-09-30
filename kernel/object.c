#include "kernel/object.h"
#include "kernel/array.h"
#include "kernel/fcall.h"
#include "kernel/exception.h"
#include "kernel/../exception.h"


int slim_update_property(zval *object, const char *property_name, uint32_t property_length, zval *value){

	zend_class_entry *ce, *old_scope;
	zval property;

#if PHP_VERSION_ID >= 70100
	old_scope = EG(fake_scope);
#else
	old_scope = EG(scope);
#endif

	if (Z_TYPE_P(object) != IS_OBJECT) {
		php_error_docref(NULL, E_WARNING, "Attempt to assign property of non-object");
		return FAILURE;
	}

	ce = Z_OBJCE_P(object);
	if (ce->parent) {
		ce = slim_lookup_class_ce(ce, property_name, property_length);
	}

#if PHP_VERSION_ID >= 70100
	EG(fake_scope) = ce;
#else
	EG(scope) = ce;
#endif

	if (!Z_OBJ_HT_P(object)->write_property) {
		const char *class_name;

		class_name = Z_OBJ_P(object) ? ZSTR_VAL(Z_OBJCE_P(object)->name) : "";
		zend_error(E_CORE_ERROR, "Property %s of class %s cannot be updated", property_name, class_name);
	}

	ZVAL_STRINGL(&property, property_name, property_length);

	/* write_property will add 1 to refcount, so no Z_TRY_ADDREF_P(value); is necessary */
	// Z_OBJ_HT_P(object)->write_property(object, &property, value, NULL);
	Z_OBJ_HANDLER_P(object, write_property)(object, &property, value, NULL);

#if PHP_VERSION_ID >= 70100
	EG(fake_scope) = old_scope;
#else
	EG(scope) = old_scope;
#endif
	zval_ptr_dtor(&property);
	return SUCCESS;
}

int slim_update_property_empty_array(zval *object, const char *property_name, uint32_t property_length) {

    zval v = {};
    int status;

    array_init(&v);
    status = slim_update_property(object, property_name, property_length, &v);
    zval_ptr_dtor(&v);
    return status;
}

int slim_update_property_array_append(zval *object, const char *property, uint32_t property_length, zval *value)
{
	zval tmp = {};
	int separated = 0;

	if (!object) {
		php_error_docref(NULL, E_WARNING, "Attempt to assign property of non-object (1)");
		return FAILURE;
	}

	if (Z_TYPE_P(object) != IS_OBJECT) {
		php_error_docref(NULL, E_WARNING, "Attempt to assign property of non-object (2)");
		return FAILURE;
	}

	slim_read_property(&tmp, object, property, property_length, PH_NOISY | PH_READONLY);

	/** Convert the value to array if not is an array */
	if (Z_TYPE(tmp) != IS_ARRAY) {
		array_init(&tmp);
		separated = 1;
		Z_DELREF(tmp);
	}

	slim_array_append(&tmp, value, PH_COPY);

	if (separated) {
      slim_update_property(object, property, property_length, &tmp);
	}

	return SUCCESS;
}


int slim_read_property(zval *result, zval *object, const char *property_name, uint32_t property_length, int flags)
{
	zval property, rv;
	zend_class_entry *ce, *old_scope;
	zval *res;

	if (Z_TYPE_P(object) != IS_OBJECT) {

		if ((flags & PH_NOISY) == PH_NOISY) {
			php_error_docref(NULL, E_NOTICE, "Trying to get property \"%s\" of non-object", property_name);
		}

		ZVAL_NULL(result);
		return FAILURE;
	}

	ce = Z_OBJCE_P(object);
	if (ce->parent) {
		ce = slim_lookup_class_ce(ce, property_name, property_length);
	}

#if PHP_VERSION_ID >= 70100
	old_scope = EG(fake_scope);
	EG(fake_scope) = ce;
#else
	old_scope = EG(scope);
	EG(scope) = ce;
#endif
	if (!Z_OBJ_HT_P(object)->read_property) {
		const char *class_name;

		class_name = Z_OBJ_P(object) ? ZSTR_VAL(Z_OBJCE_P(object)->name) : "";
		zend_error(E_CORE_ERROR, "Property %s of class %s cannot be read", property_name, class_name);
	}

	ZVAL_STRINGL(&property, property_name, property_length);

	res = Z_OBJ_HT_P(object)->read_property(object, &property, flags ? BP_VAR_IS : BP_VAR_R, NULL, &rv);
	if ((flags & PH_SEPARATE) == PH_SEPARATE) {
		ZVAL_DUP(result, res);
	} else if ((flags & PH_READONLY) == PH_READONLY) {
		ZVAL_COPY_VALUE(result, res);
	} else {
		ZVAL_COPY(result, res);
	}

#if PHP_VERSION_ID >= 70100
	EG(fake_scope) = old_scope;
#else
	EG(scope) = old_scope;
#endif
	zval_ptr_dtor(&property);
	return SUCCESS;
}

int slim_update_property_bool(zval *object, const char *property_name, uint32_t property_length, int value) {
    zval v = {};
    int ret;
    ZVAL_BOOL(&v, value);
    ret = slim_update_property(object, property_name, property_length, &v);
    zval_ptr_dtor(&v);
    return ret;
}

int slim_update_property_array(zval *object, const char *property, uint32_t property_length, const zval *index, zval *value)
{
	zval tmp;
	int separated = 0;

	if (Z_TYPE_P(object) == IS_OBJECT) {
      slim_read_property(&tmp, object, property, property_length, PH_NOISY | PH_READONLY);

		/** Convert the value to array if not is an array */
		if (Z_TYPE(tmp) != IS_ARRAY) {
			array_init(&tmp);
			separated = 1;
			Z_DELREF(tmp);
		}
		Z_TRY_ADDREF_P(value);

		if (Z_TYPE_P(index) == IS_STRING) {
			zend_symtable_str_update(Z_ARRVAL(tmp), Z_STRVAL_P(index), Z_STRLEN_P(index), value);
		} else if (Z_TYPE_P(index) == IS_LONG) {
			zend_hash_index_update(Z_ARRVAL(tmp), Z_LVAL_P(index), value);
		} else if (Z_TYPE_P(index) == IS_NULL) {
			zend_hash_next_index_insert(Z_ARRVAL(tmp), value);
		}

		if (separated) {
			slim_update_property(object, property, property_length, &tmp);
		}
	}

	return SUCCESS;
}

int slim_unset_property_array(zval *object, const char *property, uint32_t property_length, const zval *index) {

	zval tmp = {};
	int separated = 0;

	if (Z_TYPE_P(object) == IS_OBJECT) {
      slim_read_property(&tmp, object, property, property_length, PH_NOISY | PH_READONLY);

      /** Convert the value to array if not is an array */
      if (Z_TYPE(tmp) != IS_ARRAY) {
          array_init(&tmp);
          separated = 1;
          Z_DELREF(tmp);
      }

      slim_array_unset(&tmp, index, 0);

      if (separated) {
          slim_update_property(object, property, property_length, &tmp);
      }
	}

	return SUCCESS;
}


zend_class_entry *slim_class_exists(const zval *class_name, int autoload) {

    zend_class_entry *ce;

    if (Z_TYPE_P(class_name) == IS_STRING) {
        if ((ce = zend_lookup_class_ex(Z_STR_P(class_name), NULL, autoload)) != NULL) {
            return (ce->ce_flags & (ZEND_ACC_INTERFACE | (ZEND_ACC_TRAIT - ZEND_ACC_EXPLICIT_ABSTRACT_CLASS))) == 0 ? ce : NULL;
        }
    }

    return NULL;
}


zend_class_entry *slim_class_exists_ex(const zval *class_name, int autoload) {

    if (Z_TYPE_P(class_name) == IS_STRING) {
        return slim_class_exists(class_name, autoload);
    }

    php_error_docref(NULL, E_WARNING, "class name must be a string");
    return NULL;
}


int slim_create_instance_params(zval *return_value, const zval *class_name, zval *params){

    zend_class_entry *ce;

    if (unlikely(Z_TYPE_P(class_name) != IS_STRING)) {
        slim_throw_exception_string(slim_exception_ce, "Invalid class name");
        return FAILURE;
    }

    ce = zend_fetch_class(Z_STR_P(class_name), ZEND_FETCH_CLASS_DEFAULT);
    if (!ce) {
        ZVAL_NULL(return_value);
        return FAILURE;
    }

    return slim_create_instance_params_ce(return_value, ce, params);
}

int slim_create_instance_params_ce(zval *return_value, zend_class_entry *ce, zval *params)
{
    int outcome = SUCCESS;

    object_init_ex(return_value, ce);

    if (slim_has_constructor_ce(ce)) {
        int param_count = (Z_TYPE_P(params) == IS_ARRAY) ? zend_hash_num_elements(Z_ARRVAL_P(params)) : 0;
        zval *static_params[10];
        zval **params_ptr, **params_arr = NULL;

        if (param_count > 0) {
            zval *item;
            int i = 0;

            if (likely(param_count) <= 10) {
                params_ptr = static_params;
            } else {
                params_arr = emalloc(param_count * sizeof(zval*));
                params_ptr = params_arr;
            }

            ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(params), item) {
                params_ptr[i++] = item;
            } ZEND_HASH_FOREACH_END();
        } else {
            params_ptr = NULL;
        }

        outcome = slim_call_method(NULL, return_value, "__construct", param_count, params_ptr);

        if (unlikely(params_arr != NULL)) {
            efree(params_arr);
        }
    }

    return outcome;
}

int slim_create_instance(zval *return_value, const zval *class_name){

    zend_class_entry *ce;

    if (unlikely(Z_TYPE_P(class_name) != IS_STRING)) {
        slim_throw_exception_string(slim_exception_ce, "Invalid class name");
        return FAILURE;
    }

    ce = zend_fetch_class(Z_STR_P(class_name), ZEND_FETCH_CLASS_DEFAULT);
    if (!ce) {
        return FAILURE;
    }

    return slim_create_instance_params_ce(return_value, ce, &SLIM_G(z_null));
}

int slim_property_array_isset_fetch(zval *fetched, zval *object, const char *property, size_t property_length, const zval *index, int flags)
{
    zval property_value = {};

    if (!slim_property_isset_fetch(&property_value, object, property, property_length, PH_READONLY)) {
        return 0;
    }

    if (!slim_array_isset_fetch(fetched, &property_value, index, flags)) {
        return 0;
    }
    return 1;
}

int slim_property_isset_fetch(zval *return_value, zval *object, const char *property_name, size_t property_length, int flags)
{
    zval *value;
    zend_class_entry *ce;

    if (!slim_isset_property(object, property_name, property_length)) {
        return 0;
    }

    ce = Z_OBJCE_P(object);
    if (ce->parent) {
        ce = slim_lookup_class_ce(ce, property_name, property_length);
    }

    value = zend_read_property(ce, object, property_name, property_length, 1, NULL);
    if (EXPECTED(Z_TYPE_P(value) == IS_REFERENCE)) {
        value = Z_REFVAL_P(value);
    }
    if ((flags & PH_SEPARATE) == PH_SEPARATE) {
        ZVAL_DUP(return_value, value);
    } else if ((flags & PH_READONLY) == PH_READONLY) {
        ZVAL_COPY_VALUE(return_value, value);
    } else {
        ZVAL_COPY(return_value, value);
    }

    return 1;
}


int slim_property_exists(zval *object, const char *property_name, uint32_t property_length, int flags)
{
    if (Z_TYPE_P(object) == IS_OBJECT) {
        if (likely((flags & PH_DECLARED) == PH_DECLARED)) {
            if (likely(zend_hash_str_exists(&Z_OBJCE_P(object)->properties_info, property_name, property_length))) {
                return 1;
            }
        }

        if (likely((flags & PH_DYNAMIC) == PH_DYNAMIC)) {
            return zend_hash_str_exists(Z_OBJ_HT_P(object)->get_properties(object), property_name, property_length);
        }
    }

    return 0;
}
