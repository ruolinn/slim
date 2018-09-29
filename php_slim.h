/*
  +----------------------------------------------------------------------+
  | PHP Version 7                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2018 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author:                                                              |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef PHP_SLIM_H
#define PHP_SLIM_H

#include <main/php.h>

extern zend_module_entry slim_module_entry;
#define phpext_slim_ptr &slim_module_entry

#define PHP_SLIM_VERSION "0.1.0" /* Replace with version number for your extension */

#ifdef PHP_WIN32
#	define PHP_SLIM_API __declspec(dllexport)
#elif defined(__GNUC__) && __GNUC__ >= 4
#	define PHP_SLIM_API __attribute__ ((visibility("default")))
#else
#	define PHP_SLIM_API
#endif


#ifdef ZTS
#include "TSRM.h"
#endif

/*
  	Declare any global variables you may need between the BEGIN
	and END macros here:

ZEND_BEGIN_MODULE_GLOBALS(slim)
	zend_long  global_value;
	char *global_string;
ZEND_END_MODULE_GLOBALS(slim)
*/

/* Always refer to the globals in your function as SLIM_G(variable).
   You are encouraged to rename these macros something shorter, see
   examples in any other php module directory.
*/
#define SLIM_G(v) ZEND_MODULE_GLOBALS_ACCESSOR(slim, v)

#if defined(ZTS) && defined(COMPILE_DL_SLIM)
ZEND_TSRMLS_CACHE_EXTERN()
#endif

#define SLIM_INIT_CLASS(name) \
	int slim_ ##name## _init(int module_number)

#define SLIM_INIT(name) \
	if (slim_ ##name## _init(module_number) == FAILURE) { \
		return FAILURE; \
	}

#define TRACE(fmt, ...) do {                                            \
        trace(__FILE__, __LINE__, __FUNCTION__, fmt, ##__VA_ARGS__);    \
    } while(0)

static inline void trace(const char *file, int line, const char* function, const char *fmt, ...) {
    fprintf(stderr, "%s(%s:%d) - ", function, file, line);
    va_list args;
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    fprintf(stderr, "\n");
    va_end(args);
}

#endif	/* PHP_SLIM_H */


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
