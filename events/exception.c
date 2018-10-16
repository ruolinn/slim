#include "events/exception.h"
#include "events/../exception.h"
#include "kernel/main.h"

zend_class_entry *slim_events_exception_ce;

SLIM_INIT_CLASS(Slim_Events_Exception)
{
    SLIM_REGISTER_CLASS_EX(Slim\\Events, Exception, events_exception, slim_exception_ce, NULL, 0);

    return SUCCESS;
}
