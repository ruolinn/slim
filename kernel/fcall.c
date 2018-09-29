#include "kernel/fcall.h"

#include <Zend/zend_API.h>
#include <Zend/zend_exceptions.h>
#include <Zend/zend_execute.h>

#include "kernel/main.h"
#include "kernel/exception.h"
#include "kernel/string.h"

int slim_call_method_with_params(zval *retval, zval *object, zend_class_entry *ce, slim_call_type type, const char *method_name, uint method_len, uint param_count, zval *params[])
{
	zval func_name = {}, ret = {}, *retval_ptr = (retval != NULL) ? retval : &ret, obj = {};
	zval *arguments;
	int i, status;

	if (type != slim_fcall_function) {
		if (type != slim_fcall_ce && type != slim_fcall_self && type != slim_fcall_static) {
			if (object == NULL || Z_TYPE_P(object) != IS_OBJECT) {
				slim_throw_exception_format(spl_ce_RuntimeException, "Trying to call method %s on a non-object", method_name);
				return FAILURE;
			}
		}

		if (object == NULL || Z_TYPE_P(object) != IS_OBJECT) {
			if (zend_get_this_object(EG(current_execute_data))){
				ZVAL_OBJ(&obj, zend_get_this_object(EG(current_execute_data)));
				object = &obj;
			}
		}

		switch (type) {
			case slim_fcall_ce:
				assert(ce != NULL);
				array_init_size(&func_name, 2);
				add_next_index_string(&func_name, ce->name->val);
				add_next_index_stringl(&func_name, method_name, method_len);
				break;
			case slim_fcall_parent:
				assert(ce != NULL);
				array_init_size(&func_name, 2);
				add_next_index_string(&func_name, ISV(parent));
				add_next_index_stringl(&func_name, method_name, method_len);
				break;
			case slim_fcall_self:
				array_init_size(&func_name, 2);
				add_next_index_string(&func_name, ISV(self));
				add_next_index_stringl(&func_name, method_name, method_len);
				break;
			case slim_fcall_static:
				if (slim_memnstr_str_str(method_name, method_len, SL("::"))) {
					slim_fast_explode_str_str(&func_name, SL("::"), method_name, method_len);
				} else {
					array_init_size(&func_name, 2);
					add_next_index_string(&func_name, ISV(static));
					add_next_index_stringl(&func_name, method_name, method_len);
				}
				break;
			case slim_fcall_method:
				array_init_size(&func_name, 2);
				Z_TRY_ADDREF_P(object);
				add_next_index_zval(&func_name, object);
				add_next_index_stringl(&func_name, method_name, method_len);
				break;
			default:
				slim_throw_exception_format(spl_ce_RuntimeException, "Error call type %d for cmethod %s", type, method_name);
				return FAILURE;
		}

		if (!ce && object && Z_TYPE_P(object) == IS_OBJECT) {
			ce = Z_OBJCE_P(object);
		}
	} else {
		ZVAL_STRINGL(&func_name, method_name, method_len);
	}

	arguments = param_count ? safe_emalloc(sizeof(zval), param_count, 0) : NULL;

	i = 0;
	while(i < param_count) {
		if (params[i]) {
			ZVAL_COPY_VALUE(&arguments[i], params[i]);
		} else {
			ZVAL_NULL(&arguments[i]);
		}
		i++;
	}

	if (
#if PHP_VERSION_ID >= 70100
	(status = _call_user_function_ex(object, &func_name, retval_ptr, param_count, arguments, 1)) == FAILURE || EG(exception)
#else
	(status = call_user_function_ex(ce ? &(ce)->function_table : EG(function_table), object, &func_name, retval_ptr, param_count, arguments, 1, NULL)) == FAILURE || EG(exception)
#endif
	) {
		status = FAILURE;
		ZVAL_NULL(retval_ptr);
		if (!EG(exception)) {
			switch (type) {
				case slim_fcall_function:
					zend_error(E_ERROR, "Call to undefined function %s()", method_name);
					break;
				case slim_fcall_parent:
					zend_error(E_ERROR, "Call to undefined method parent::%s()", method_name);
					break;
				case slim_fcall_self:
					zend_error(E_ERROR, "Call to undefined method self::%s()", method_name);
					break;
				case slim_fcall_static:
					zend_error(E_ERROR, "Call to undefined function static::%s()", method_name);
					break;
				case slim_fcall_ce:
					zend_error(E_ERROR, "Call to undefined method %s::%s()", ce->name->val, method_name);
					break;
				case slim_fcall_method:
					zend_error(E_ERROR, "Call to undefined method %s::%s()", Z_OBJCE_P(object)->name->val, method_name);
					break;
				default:
					zend_error(E_ERROR, "Call to undefined method ?::%s()", method_name);
			}
		}
	}
	zval_ptr_dtor(&func_name);
	efree(arguments);
	if (retval == NULL) {
		if (!Z_ISUNDEF(ret)) {
			zval_ptr_dtor(&ret);
		}
	}

	return status;
}
