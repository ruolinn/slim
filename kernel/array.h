#ifndef SLIM_KERNEL_ARRAY_H
#define SLIM_KERNEL_ARRAY_H

#include "php_slim.h"
#include "kernel/memory.h"

int ZEND_FASTCALL slim_array_isset(const zval *arr, const zval *index);

int slim_array_append(zval *arr, zval *value, int flags);

int ZEND_FASTCALL slim_array_isset_fetch_long(zval *fetched, const zval *arr, ulong index, int flags);

int slim_array_fetch_str(zval *return_value, const zval *arr, const char *index, uint index_length, int silent);

int slim_array_fetch(zval *return_value, const zval *arr, const zval *index, int silent);
int slim_array_fetch_long(zval *return_value, const zval *arr, ulong index, int silent);

int slim_array_update_str(zval *arr, const char *index, uint index_length, zval *value, int flags);
int slim_array_update_long(zval *arr, ulong index, zval *value, int flags);

static inline int slim_array_update_str_str(zval *arr, const char *index, uint index_length, char *value, uint value_length, int flags)
{
    zval zvalue;
    int ret;

    ZVAL_STRINGL(&zvalue, value, value_length);
    ret = slim_array_update_str(arr, index, index_length, &zvalue, flags);
    return ret;
}

int ZEND_FASTCALL slim_array_isset_fetch(zval *fetched, const zval *arr, const zval *index, int flags);
int ZEND_FASTCALL slim_array_isset_fetch_str(zval *fetched, const zval *arr, const char *index, uint index_length, int flags);

int ZEND_FASTCALL slim_array_unset(zval *arr, const zval *index, int flags);

void slim_fast_array_merge(zval *return_value, zval *array1, zval *array2);

int slim_array_update(zval *arr, const zval *index, zval *value, int flags);
int slim_array_update_hash(HashTable *ht, const zval *index, zval *value, int flags);

#endif
