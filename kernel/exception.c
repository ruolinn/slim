#include "kernel/exception.h"
#include "kernel/fcall.h"
#include "kernel/main.h"

void slim_throw_exception_format(zend_class_entry *ce, const char *format, ...)
{
    zval object = {}, msg = {};
    int len;
    char *buffer;
    va_list args;

    object_init_ex(&object, ce);

    va_start(args, format);
    len = vspprintf(&buffer, 0, format, args);
    va_end(args);

    ZVAL_STRINGL(&msg, buffer, len);

    SLIM_CALL_METHOD(NULL, &object, "__construct", &msg);

    zend_throw_exception_object(&object);
    zval_dtor(&msg);
}


void slim_throw_exception_string(zend_class_entry *ce, const char *message)
{
    zend_throw_exception_ex(ce, 0, "%s", message);
}

void slim_throw_exception_zval(zend_class_entry *ce, zval *message)
{
	zval object = {};
	object_init_ex(&object, ce);

	SLIM_CALL_METHOD(NULL, &object, "__construct", message);
	zend_throw_exception_object(&object);
}
