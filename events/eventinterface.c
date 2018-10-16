#include "events/eventinterface.h"
#include "kernel/main.h"

zend_class_entry *slim_events_eventinterface_ce;

static const zend_function_entry slim_events_eventinterface_method_entry[] = {
    PHP_ABSTRACT_ME(Slim_Events_EventInterface, setType, NULL)
    PHP_ABSTRACT_ME(Slim_Events_EventInterface, getType, NULL)
    PHP_ABSTRACT_ME(Slim_Events_EventInterface, setSource, NULL)
    PHP_ABSTRACT_ME(Slim_Events_EventInterface, getSource, NULL)
    PHP_ABSTRACT_ME(Slim_Events_EventInterface, setData, NULL)
    PHP_ABSTRACT_ME(Slim_Events_EventInterface, getData, NULL)
    PHP_ABSTRACT_ME(Slim_Events_EventInterface, setCancelable, NULL)
    PHP_ABSTRACT_ME(Slim_Events_EventInterface, isCancelable, NULL)
    PHP_ABSTRACT_ME(Slim_Events_EventInterface, stop, NULL)
    PHP_ABSTRACT_ME(Slim_Events_EventInterface, isStopped, NULL)
    PHP_FE_END
};

SLIM_INIT_CLASS(Slim_Events_EventInterface)
{
    SLIM_REGISTER_INTERFACE(Slim\\Events, EventInterface, events_eventinterface, slim_events_eventinterface_method_entry);

    return SUCCESS;
}
