#include "events/listener.h"
#include "kernel/main.h"
#include "kernel/object.h"

zend_class_entry *slim_events_listener_ce;

PHP_METHOD(Slim_Events_Listener, __construct);
PHP_METHOD(Slim_Events_Listener, setListener);
PHP_METHOD(Slim_Events_Listener, getListener);
PHP_METHOD(Slim_Events_Listener, setPriority);
PHP_METHOD(Slim_Events_Listener, getPriority);
PHP_METHOD(Slim_Events_Listener, setEvent);
PHP_METHOD(Slim_Events_Listener, getEvent);

static const zend_function_entry slim_events_listener_method_entry[] = {
    PHP_ME(Slim_Events_Listener, __construct, NULL, ZEND_ACC_PUBLIC|ZEND_ACC_CTOR)
    PHP_ME(Slim_Events_Listener, setListener, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(Slim_Events_Listener, getListener, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(Slim_Events_Listener, setPriority, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(Slim_Events_Listener, getPriority, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(Slim_Events_Listener, setEvent, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(Slim_Events_Listener, getEvent, NULL, ZEND_ACC_PUBLIC)
    PHP_FE_END
};

SLIM_INIT_CLASS(Slim_Events_Listener)
{
    SLIM_REGISTER_CLASS(Slim\\Events, Listener, events_listener, slim_events_listener_method_entry, 0);

    zend_declare_property_null(slim_events_listener_ce, SL("_listener"), ZEND_ACC_PROTECTED);
    zend_declare_property_long(slim_events_listener_ce, SL("_priority"), 100, ZEND_ACC_PROTECTED);
    zend_declare_property_null(slim_events_listener_ce, SL("_event"), ZEND_ACC_PROTECTED);

    return SUCCESS;
}

PHP_METHOD(Slim_Events_Listener, __construct)
{
    zval *listener = NULL, *priority = NULL, *event = NULL;

    slim_fetch_params(0, 0, 3, &listener, &priority, &event);

    if (listener) {
        slim_update_property(getThis(), SL("_listener"), listener);
    }

    if (priority) {
        slim_update_property(getThis(), SL("_priority"), priority);
    }

    if (event) {
        slim_update_property(getThis(), SL("_event"), event);
    }
}

PHP_METHOD(Slim_Events_Listener, setListener)
{
    zval *listener;

    slim_fetch_params(0, 1, 0, &listener);

    slim_update_property(getThis(), SL("_listener"), listener);
}

PHP_METHOD(Slim_Events_Listener, getListener)
{
    RETURN_MEMBER(getThis(), "_listener");
}

PHP_METHOD(Slim_Events_Listener, setPriority)
{
    zval *priority;

    slim_fetch_params(0, 1, 0, &priority);

    slim_update_property(getThis(), SL("_priority"), priority);
}

PHP_METHOD(Slim_Events_Listener, getPriority)
{
    RETURN_MEMBER(getThis(), "_priority");
}

PHP_METHOD(Slim_Events_Listener, setEvent)
{
    zval *event;

    slim_fetch_params(0, 1, 0, &event);

    slim_update_property(getThis(), SL("_event"), event);
}

PHP_METHOD(Slim_Events_Listener, getEvent)
{
    RETURN_MEMBER(getThis(), "_event");
}

