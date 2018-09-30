#include "exception.h"
#include "kernel/main.h"

#include <Zend/zend_exceptions.h>

zend_class_entry *slim_exception_ce;

SLIM_INIT_CLASS(Slim_Exception)
{
    SLIM_REGISTER_CLASS_EX(Slim, Exception, exception, zend_ce_exception, NULL, 0);

    return SUCCESS;
}
