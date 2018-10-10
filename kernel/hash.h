#ifndef SLIM_KERNEL_HASH_H
#define SLIM_KERNEL_HASH_H

#include "php_slim.h"
#include <Zend/zend.h>

zval* slim_hash_get(HashTable *ht, const zval *key, int type);

#endif
