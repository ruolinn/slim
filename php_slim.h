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

#define likely(x)       EXPECTED(x)
#define unlikely(x)     UNEXPECTED(x)

#if defined(__GNUC__) && (defined(__clang__) || ((__GNUC__ * 100 + __GNUC_MINOR__) >= 405))
#	define UNREACHABLE() __builtin_unreachable()
#	define ASSUME(x)     if (x) {} else __builtin_unreachable()
#else
#	define UNREACHABLE() assert(0)
#	define ASSUME(x)     assert(!!(x));
#endif

#if defined(__GNUC__) || defined(__clang__)
#	define SLIM_ATTR_NONNULL            __attribute__((nonnull))
#	define SLIM_ATTR_NONNULL1(x)        __attribute__((nonnull (x)))
#	define SLIM_ATTR_NONNULL2(x, y)     __attribute__((nonnull (x, y)))
#	define SLIM_ATTR_NONNULL3(x, y, z)  __attribute__((nonnull (x, y, z)))
#	define SLIM_ATTR_PURE               __attribute__((pure))
#	define SLIM_ATTR_CONST              __attribute__((const))
#	define SLIM_ATTR_WARN_UNUSED_RESULT __attribute__((warn_unused_result))
#else
#	define SLIM_ATTR_NONNULL
#	define SLIM_ATTR_NONNULL1(x)
#	define SLIM_ATTR_NONNULL2(x, y)
#	define SLIM_ATTR_NONNULL3(x, y, z)
#	define SLIM_ATTR_PURE
#	define SLIM_ATTR_CONST
#	define SLIM_ATTR_WARN_UNUSED_RESULT
#endif

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

ZEND_BEGIN_MODULE_GLOBALS(slim)
    int initialized;

    zval z_null;
    zval z_true;
    zval z_false;
    zval z_zero;
    zval z_one;
    zval z_two;
ZEND_END_MODULE_GLOBALS(slim)

ZEND_EXTERN_MODULE_GLOBALS(slim)


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

# if defined(HAVE_JSON) && !defined(SLIM_USE_PHP_JSON)
#    define SLIM_USE_PHP_JSON 1
# endif

#endif	/* PHP_SLIM_H */


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
