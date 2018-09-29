#include "kernel/memory.h"

void slim_initialize_memory(zend_slim_globals *slim_globals_ptr)
{
    ZVAL_NULL(&slim_globals_ptr->z_null);
    ZVAL_FALSE(&slim_globals_ptr->z_false);
    ZVAL_TRUE(&slim_globals_ptr->z_true);
    ZVAL_LONG(&slim_globals_ptr->z_zero, 0);
    ZVAL_LONG(&slim_globals_ptr->z_one, 1);
    ZVAL_LONG(&slim_globals_ptr->z_two, 2);

    slim_globals_ptr->initialized = 1;
}
