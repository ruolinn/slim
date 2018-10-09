#ifndef SLIM_KERNEL_FCALL_H
#define SLIM_KERNEL_FCALL_H

#include "php_slim.h"
#include "kernel/main.h"

#include <Zend/zend_hash.h>
#include <Zend/zend.h>

typedef enum _slim_call_type {
    slim_fcall_parent,
    slim_fcall_self,
    slim_fcall_static,
    slim_fcall_ce,
    slim_fcall_method,
    slim_fcall_function
} slim_call_type;

int slim_has_constructor_ce(const zend_class_entry *ce);

int slim_call_method_with_params(zval *retval, zval *object, zend_class_entry *ce, slim_call_type type, const char *method_name, uint method_len, uint param_count, zval *params[]);
int slim_call_user_func_array(zval *retval, zval *handler, zval *params);

static inline int slim_call_method(zval *retval, zval *object, const char *method, uint nparams, zval **params)
{
    return slim_call_method_with_params(retval, object, Z_OBJCE_P(object), slim_fcall_method, method, strlen(method), nparams, params);
}

#define SLIM_FUNC_STRLEN(x) (__builtin_constant_p(x) ? (sizeof(x)-1) : strlen(x))

#if defined(_MSC_VER)
#define SLIM_PASS_CALL_PARAMS(x) x + 1
#define SLIM_CALL_NUM_PARAMS(x) ((sizeof(x) - sizeof(x[0]))/sizeof(x[0]))
#define SLIM_FETCH_VA_ARGS NULL,
#else
#define SLIM_PASS_CALL_PARAMS(x) x
#define SLIM_CALL_NUM_PARAMS(x) sizeof(x)/sizeof(zval *)
#define SLIM_FETCH_VA_ARGS
#endif

#define SLIM_CALL_METHOD(retval, object, method, ...) SLIM_CALL_METHOD_WITH_PARAMS(retval, object, Z_OBJCE_P(object), slim_fcall_method, method, __VA_ARGS__)
#define SLIM_CALL_SELF(retval, method, ...) SLIM_CALL_METHOD_WITH_PARAMS(retval, NULL, NULL, slim_fcall_self, method, __VA_ARGS__)

#define SLIM_CALL_METHOD_WITH_PARAMS(retval, obj, obj_ce, call_type, method, ...) \
    do {                                                                \
        zval *params_[] = {SLIM_FETCH_VA_ARGS __VA_ARGS__};          \
        RETURN_ON_FAILURE(slim_call_method_with_params(retval, obj, obj_ce, call_type, method, SLIM_FUNC_STRLEN(method), SLIM_CALL_NUM_PARAMS(params_), SLIM_PASS_CALL_PARAMS(params_))); \
    } while (0)

#define SLIM_CALL_USER_FUNC_ARRAY(retval, handler, params)			\
	do {																\
		RETURN_ON_FAILURE(slim_call_user_func_array(retval, handler, params)); \
	} while (0)


#define SLIM_CALL_METHOD_WITH_FLAG(flag, retval, obj, obj_ce, call_type, method, ...) \
    do {                                                                \
        zval *params_[] = {SLIM_FETCH_VA_ARGS __VA_ARGS__};          \
        flag = slim_call_method_with_params(retval, obj, obj_ce, call_type, method, SLIM_FUNC_STRLEN(method), SLIM_CALL_NUM_PARAMS(params_), SLIM_PASS_CALL_PARAMS(params_)); \
    } while (0)


#define SLIM_CALL_FUNCTION_FLAG(flag, retval, method, ...) SLIM_CALL_METHOD_WITH_FLAG(flag, retval, NULL, NULL, slim_fcall_function, method, __VA_ARGS__)

#endif
