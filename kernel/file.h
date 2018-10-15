#ifndef SLIM_KERNEL_FILE_H
#define SLIM_KERNEL_FILE_H

#include "php_slim.h"

int slim_file_exists(zval *filename);
void slim_fix_path(zval *return_value, zval *path, zval *directory_separator);
void slim_possible_autoload_filepath(zval *return_value, zval *prefix, zval *class_name, zval *virtual_separator, zval *separator);

#endif
