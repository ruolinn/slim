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
