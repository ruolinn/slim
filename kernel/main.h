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
            fprintf(stderr, "Phalcon Error: Class to extend '%s' was not found when registering class '%s'\n", (parent_ce ? parent_ce->name->val : "(null)"), ZEND_NS_NAME(#ns, #class_name)); \
            return FAILURE;                                             \
        }                                                               \
        slim_ ##lcname## _ce->ce_flags |= flags;                     \
    }

#define RETURN_ON_FAILURE(what)                 \
    if (FAILURE == what) {                      \
        return;                                 \
    }

#endif
