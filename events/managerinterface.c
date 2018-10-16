#include "events/managerinterface.h"
#include "kernel/main.h"

zend_class_entry *slim_events_managerinterface_ce;

static const zend_function_entry slim_events_managerinterface_method_entry[] = {
    PHP_ABSTRACT_ME(Slim_Events_ManagerInterface, attach, NULL)
    PHP_ABSTRACT_ME(Slim_Events_ManagerInterface, detach, NULL)
    PHP_ABSTRACT_ME(Slim_Events_ManagerInterface, detachAll, NULL)
    PHP_ABSTRACT_ME(Slim_Events_ManagerInterface, fire, NULL)
    PHP_ABSTRACT_ME(Slim_Events_ManagerInterface, getListeners, NULL)
    PHP_FE_END
};

SLIM_INIT_CLASS(Slim_Events_ManagerInterface)
{
    SLIM_REGISTER_INTERFACE(Slim\\Events, ManagerInterface, events_managerinterface, slim_events_managerinterface_method_entry);

    return SUCCESS;
}
