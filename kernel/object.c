#include "kernel/object.h"

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
