#ifndef SLIM_KERNEL_ARRAY_H
#define SLIM_KERNEL_ARRAY_H

#include "php_slim.h"
#include "kernel/memory.h"

int slim_array_append(zval *arr, zval *value, int flags);

int slim_array_fetch(zval *return_value, const zval *arr, const zval *index, int silent);
int slim_array_fetch_long(zval *return_value, const zval *arr, ulong index, int silent);

int ZEND_FASTCALL slim_array_isset_fetch(zval *fetched, const zval *arr, const zval *index, int flags);
int ZEND_FASTCALL slim_array_isset_fetch_str(zval *fetched, const zval *arr, const char *index, uint index_length, int flags);

int ZEND_FASTCALL slim_array_unset(zval *arr, const zval *index, int flags);

#endif
