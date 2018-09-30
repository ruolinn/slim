#include "container/exception.h"
#include "container/../exception.h"
#include "kernel/main.h"

zend_class_entry *slim_container_exception_ce;

SLIM_INIT_CLASS(Slim_Container_Exception)
{
    SLIM_REGISTER_CLASS_EX(Slim\\Container, Exception, container_exception, slim_exception_ce, NULL, 0);

    return SUCCESS;
}
