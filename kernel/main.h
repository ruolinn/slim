#ifndef SLIM_KERNEL_MAIN_H
#define SLIM_KERNEL_MAIN_H

#include "php_slim.h"

#include <Zend/zend_types.h>
#include <Zend/zend_exceptions.h>
#include <Zend/zend_interfaces.h>

#include <ext/spl/spl_exceptions.h>
#include <ext/spl/spl_iterators.h>

#define PH_DEBUG		0

#define PH_DECLARED		2
#define PH_DYNAMIC		4
#define PH_BOTH			6

#define PH_NOISY		256
#define PH_SILENT		512
#define PH_READONLY		1024

#define PH_SEPARATE		2048
#define PH_COPY			4096
#define PH_CTOR			8192

#define SL(str)   (str), (sizeof(str)-1)
#define SS(str)   (str), (sizeof(str))
#define IS(str)   (slim_interned_##str)
#define ISV(str)  (slim_interned_##str)->val
#define ISL(str)  (slim_interned_##str)->val, (sizeof(#str)-1)
#define ISS(str)  (slim_interned_##str)->val, (sizeof(#str))
#define SSL(str)   zend_string_init(SL(str), 0)
#define SSS(str)   zend_string_init(SS(str), 0)

#define SLIM_ENSURE_IS_STRING(pzv)    convert_to_string_ex(pzv)
#define SLIM_ENSURE_IS_LONG(pzv)      convert_to_long_ex(pzv)
#define SLIM_ENSURE_IS_DOUBLE(pzv)    convert_to_double_ex(pzv)
#define SLIM_ENSURE_IS_BOOL(pzv)      convert_to_boolean_ex(pzv)
#define SLIM_ENSURE_IS_ARRAY(pzv)     convert_to_array_ex(pzv)
#define SLIM_ENSURE_IS_OBJECT(pzv)    convert_to_object_ex(pzv)
#define SLIM_ENSURE_IS_NULL(pzv)      convert_to_null_ex(pzv)

int slim_is_callable(zval *var);

zend_class_entry *slim_register_internal_interface_ex(zend_class_entry *orig_ce, zend_class_entry *parent_ce);

int slim_read_global_str(zval *return_value, const char *global, unsigned int global_length);
zval* slim_get_global_str(const char *global, unsigned int global_length);

int slim_fetch_parameters(int num_args, int required_args, int optional_args, ...);

#define slim_fetch_params(memory_grow, required_params, optional_params, ...) \
    zval slim_memory_entry = {}; \
    if (slim_fetch_parameters(ZEND_NUM_ARGS(), required_params, optional_params, __VA_ARGS__) == FAILURE) { \
        RETURN_NULL(); \
    } else if (memory_grow) { \
        array_init(&slim_memory_entry); \
    } else { \
        ZVAL_NULL(&slim_memory_entry); \
    }

#define SLIM_VERIFY_INTERFACE_EX(instance, interface_ce, exception_ce) \
    if (Z_TYPE_P(instance) != IS_OBJECT) {                              \
        zend_throw_exception_ex(exception_ce, 0, "Unexpected value type: expected object implementing %s, %s given", interface_ce->name->val, zend_zval_type_name(instance)); \
        return;                                                         \
    } else if (!instanceof_function_ex(Z_OBJCE_P(instance), interface_ce, 1)) { \
        zend_throw_exception_ex(exception_ce, 0, "Unexpected value type: expected object implementing %s, object of type %s given", interface_ce->name->val, Z_OBJCE_P(instance)->name->val); \
        return;                                                         \
    }

#define RETURN_NCTOR(var) {                     \
        RETVAL_ZVAL(var, 0, 0);                 \
    }                                           \
        return;

#define RETURN_EMPTY_ARRAY() array_init(return_value); return;

#define SLIM_REGISTER_INTERFACE(ns, classname, name, methods)        \
    {                                                                   \
        zend_class_entry ce;                                            \
        INIT_NS_CLASS_ENTRY(ce, #ns, #classname, methods);              \
        slim_ ##name## _ce = zend_register_internal_interface(&ce);  \
    }

#define SLIM_REGISTER_INTERFACE_EX(ns, classname, lcname, parent_ce, methods) \
    {                                                                   \
        zend_class_entry ce;                                            \
        INIT_NS_CLASS_ENTRY(ce, #ns, #classname, methods);              \
        slim_ ##lcname## _ce = slim_register_internal_interface_ex(&ce, parent_ce); \
        if (!slim_ ##lcname## _ce) {                                 \
            fprintf(stderr, "Can't register interface %s with parent %s\n", ZEND_NS_NAME(#ns, #classname), (parent_ce ? parent_ce->name->val : "(null)")); \
            return FAILURE;                                             \
        }                                                               \
    }

#define SLIM_REGISTER_CLASS(ns, class_name, name, methods, flags)  \
    {                                                                 \
        zend_class_entry ce;                                          \
        INIT_NS_CLASS_ENTRY(ce, #ns, #class_name, methods);           \
        slim_ ##name## _ce = zend_register_internal_class(&ce);    \
        slim_ ##name## _ce->ce_flags |= flags;                     \
    }

#define SLIM_REGISTER_CLASS_EX(ns, class_name, lcname, parent_ce, methods, flags) \
    {                                                                   \
        zend_class_entry ce;                                            \
        INIT_NS_CLASS_ENTRY(ce, #ns, #class_name, methods);             \
        slim_ ##lcname## _ce = zend_register_internal_class_ex(&ce, parent_ce); \
        if (!slim_ ##lcname## _ce) {                                 \
            fprintf(stderr, "Slim Error: Class to extend '%s' was not found when registering class '%s'\n", (parent_ce ? parent_ce->name->val : "(null)"), ZEND_NS_NAME(#ns, #class_name)); \
            return FAILURE;                                             \
        }                                                               \
        slim_ ##lcname## _ce->ce_flags |= flags;                     \
    }

#define RETURN_ON_FAILURE(what)                 \
    if (FAILURE == what) {                      \
        return;                                 \
    }

#define RETURN_THIS() {                         \
        RETVAL_ZVAL(getThis(), 1, 0);           \
    }                                           \
        return;

#define RETURN_MEMBER(object, member_name)                              \
    slim_read_property(return_value, object, SL(member_name), PH_COPY); \
    return;

#define RETURN_CTOR(var) {                      \
        RETVAL_ZVAL(var, 1, 0);                 \
    }                                           \
    return;

#define SLIM_VERIFY_CLASS_EX(instance, class_ce, exception_ce)       \
    if (Z_TYPE_P(instance) != IS_OBJECT || !instanceof_function_ex(Z_OBJCE_P(instance), class_ce, 0)) { \
        if (Z_TYPE_P(instance) != IS_OBJECT) {                          \
            zend_throw_exception_ex(exception_ce, 0, "Unexpected value type: expected object of type %s, %s given", class_ce->name->val, zend_zval_type_name(instance)); \
        } else {                                                        \
            zend_throw_exception_ex(exception_ce, 0, "Unexpected value type: expected object of type %s, object of type %s given", class_ce->name->val, Z_OBJCE_P(instance)->name->val); \
        }                                                               \
        return;                                                         \
    }

#endif
