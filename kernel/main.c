#include "php_slim.h"

#include "kernel/main.h"
#include "kernel/exception.h"

#include <ext/spl/spl_exceptions.h>
#include <Zend/zend_interfaces.h>
#include <Zend/zend_inheritance.h>

zend_class_entry *slim_register_internal_interface_ex(zend_class_entry *orig_ce, zend_class_entry *parent_ce) {

    zend_class_entry *ce;

    ce = zend_register_internal_interface(orig_ce);
    if (parent_ce) {
        zend_do_inheritance(ce, parent_ce);
    }

    return ce;
}

int slim_fetch_parameters(int num_args, int required_args, int optional_args, ...)
{
    va_list va;
    zval **arg, *param;
    int arg_count;
    int use_args_num;

    param = ZEND_CALL_ARG(EG(current_execute_data), 1);
    arg_count = ZEND_CALL_NUM_ARGS(EG(current_execute_data));

    if (num_args < required_args) {
        slim_throw_exception_format(spl_ce_BadMethodCallException, "Wrong number of parameters, num:%d, required:%d", num_args, required_args);
        return FAILURE;
    }

    if (num_args > arg_count) {
        slim_throw_exception_format(spl_ce_BadMethodCallException, "Could not obtain parameters for parsing, num:%d, count:%d", num_args, arg_count);
        return FAILURE;
    }

    if (!num_args) {
        return SUCCESS;
    }

    va_start(va, optional_args);

    use_args_num = required_args + optional_args;
    num_args = num_args > use_args_num ? use_args_num : num_args;

    while (num_args-- > 0) {
        arg = va_arg(va, zval **);
        *arg = param;
        param++;
    }

    va_end(va);

    return SUCCESS;
}


zval* slim_get_global_str(const char *global, unsigned int global_length) {

    if (PG(auto_globals_jit)) {
        zend_is_auto_global_str((char *)global, global_length);
    }

    if (&EG(symbol_table)) {
        zval *gv;
        if ((gv = zend_hash_str_find(&EG(symbol_table), global, global_length)) != NULL) {
            if (EXPECTED(Z_TYPE_P(gv) == IS_REFERENCE)) {
                gv = Z_REFVAL_P(gv);
            }
            if (Z_TYPE_P(gv) == IS_ARRAY) {
                return gv;
            }
        }
    }

    return &SLIM_G(z_null);
}
