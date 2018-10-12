#include "router/exception.h"
#include "router/../exception.h"
#include "kernel/main.h"

zend_class_entry *slim_router_route_exception_ce;

SLIM_INIT_CLASS(Slim_Router_Route_Exception)
{
    SLIM_REGISTER_CLASS_EX(Slim\\Router, Exception, router_route_exception, slim_exception_ce, NULL, 0);

    return SUCCESS;
}
