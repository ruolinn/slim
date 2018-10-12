#ifndef SLIM_KERNEL_FRAMEWORK_ROUTER_H
#define SLIM_KERNEL_FRAMEWORK_ROUTER_H

#include <Zend/zend.h>

void slim_extract_named_params(zval *return_value, zval *str, zval *matches);
void slim_replace_paths(zval *return_value, zval *pattern, zval *paths, zval *uri);

#endif
