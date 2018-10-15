#ifndef SLIM_KERNEL_REQUIRE_H
#define SLIM_KERNEL_REQUIRE_H

#include "php_slim.h"

int slim_require_ret(zval *return_value_ptr, const char *require_path) SLIM_ATTR_NONNULL1(2);

SLIM_ATTR_NONNULL static inline int slim_require(const char *require_path)
{
    return slim_require_ret(NULL, require_path);
}

#endif
