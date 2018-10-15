#include "kernel/file.h"
#include "kernel/concat.h"

#include <ctype.h>

#include <main/php_streams.h>
#include <Zend/zend_exceptions.h>
#include <Zend/zend_interfaces.h>
#include <Zend/zend_smart_str.h>
#include <ext/standard/file.h>
#include <ext/standard/php_filestat.h>
#include <ext/standard/php_string.h>
void slim_possible_autoload_filepath(zval *return_value, zval *prefix, zval *class_name, zval *virtual_separator, zval *separator) {

	int i, length;
	unsigned char ch;
	smart_str virtual_str = {0};

	if (Z_TYPE_P(prefix) != IS_STRING || Z_TYPE_P(class_name) != IS_STRING || Z_TYPE_P(virtual_separator) != IS_STRING) {
		RETURN_FALSE;
	}

	length = Z_STRLEN_P(prefix);
	if (!length) {
		RETURN_FALSE;
	}

	if (length > Z_STRLEN_P(class_name)) {
		RETURN_FALSE;
	}

	if (separator) {
		if (Z_STRVAL_P(prefix)[Z_STRLEN_P(prefix) - 1] == Z_STRVAL_P(separator)[0]) {
			length--;
		}
	}

	for (i = length + 1; i < Z_STRLEN_P(class_name); i++) {

		ch = Z_STRVAL_P(class_name)[i];

		/**
		 * Anticipated end of string
		 */
		if (ch == '\0') {
			break;
		}

		/**
		 * Replace namespace separator by directory separator
		 */
		if (ch == '\\') {
			smart_str_appendl(&virtual_str, Z_STRVAL_P(virtual_separator), Z_STRLEN_P(virtual_separator));
			continue;
		}

		/**
		 * Replace separator
		 */
		if (separator) {
			if (ch == Z_STRVAL_P(separator)[0]) {
				smart_str_appendl(&virtual_str, Z_STRVAL_P(virtual_separator), Z_STRLEN_P(virtual_separator));
				continue;
			}
		}

		/**
		 * Basic alphanumeric characters
		 */
		if ((ch == '_') || (ch >= '0' && ch <= '9') || (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z')) {
			smart_str_appendc(&virtual_str, ch);
			continue;
		}

		/**
		 * Multibyte characters?
		 */
		if (ch > 127) {
			smart_str_appendc(&virtual_str, ch);
			continue;
		}

	}

	smart_str_0(&virtual_str);

	if (virtual_str.s) {
		RETURN_STR(virtual_str.s);
	} else {
		smart_str_free(&virtual_str);
		RETURN_FALSE;
	}
}

void slim_fix_path(zval *return_value, zval *path, zval *directory_separator)
{
    if (Z_TYPE_P(path) != IS_STRING || Z_TYPE_P(directory_separator) != IS_STRING) {
        return;
    }

    if (Z_STRLEN_P(path) > 0 && Z_STRLEN_P(directory_separator) > 0) {
        if (Z_STRVAL_P(path)[Z_STRLEN_P(path) - 1] != Z_STRVAL_P(directory_separator)[0]) {
            SLIM_CONCAT_VV(return_value, path, directory_separator);
            return;
        }
    }

    ZVAL_DUP(return_value, path);
    return;
}

int slim_file_exists(zval *filename)
{
    zval exists_flag = {};

    if (Z_TYPE_P(filename) != IS_STRING) {
        return FAILURE;
    }

    php_stat(Z_STRVAL_P(filename), (php_stat_len) Z_STRLEN_P(filename), FS_EXISTS, &exists_flag);

    if (Z_TYPE(exists_flag) == IS_TRUE) {
        return SUCCESS;
    }

    return FAILURE;
}
