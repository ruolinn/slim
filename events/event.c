#include "events/event.h"
#include "events/eventinterface.h"
#include "events/exception.h"
#include "kernel/main.h"
#include "kernel/object.h"
#include "kernel/exception.h"
#include "kernel/operators.h"

zend_class_entry *slim_events_event_ce;

PHP_METHOD(Slim_Events_Event, __construct);
PHP_METHOD(Slim_Events_Event, setName);
PHP_METHOD(Slim_Events_Event, getName);
PHP_METHOD(Slim_Events_Event, setType);
PHP_METHOD(Slim_Events_Event, getType);
PHP_METHOD(Slim_Events_Event, setSource);
PHP_METHOD(Slim_Events_Event, getSource);
PHP_METHOD(Slim_Events_Event, setData);
PHP_METHOD(Slim_Events_Event, getData);
PHP_METHOD(Slim_Events_Event, setCancelable);
PHP_METHOD(Slim_Events_Event, isCancelable);
PHP_METHOD(Slim_Events_Event, getFlag);
PHP_METHOD(Slim_Events_Event, stop);
PHP_METHOD(Slim_Events_Event, isStopped);

static const zend_function_entry slim_events_event_method_entry[] = {
    PHP_ME(Slim_Events_Event, __construct, NULL, ZEND_ACC_PUBLIC|ZEND_ACC_CTOR)
    PHP_ME(Slim_Events_Event, setName, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(Slim_Events_Event, getName, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(Slim_Events_Event, setType, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(Slim_Events_Event, getType, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(Slim_Events_Event, setSource, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(Slim_Events_Event, getSource, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(Slim_Events_Event, setData, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(Slim_Events_Event, getData, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(Slim_Events_Event, setCancelable, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(Slim_Events_Event, isCancelable, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(Slim_Events_Event, getFlag, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(Slim_Events_Event, stop, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(Slim_Events_Event, isStopped, NULL, ZEND_ACC_PUBLIC)
    PHP_MALIAS(Slim_Events_Event, getCancelable, isCancelable, NULL, ZEND_ACC_PUBLIC)
    PHP_FE_END
};

SLIM_INIT_CLASS(Slim_Events_Event)
{
    SLIM_REGISTER_CLASS(Slim\\Events, Event, events_event, slim_events_event_method_entry, 0);

    zend_declare_property_null(slim_events_event_ce, SL("_name"), ZEND_ACC_PROTECTED);
    zend_declare_property_null(slim_events_event_ce, SL("_type"), ZEND_ACC_PROTECTED);
    zend_declare_property_null(slim_events_event_ce, SL("_source"), ZEND_ACC_PROTECTED);
    zend_declare_property_null(slim_events_event_ce, SL("_data"), ZEND_ACC_PROTECTED);
    zend_declare_property_bool(slim_events_event_ce, SL("_stopped"), 0, ZEND_ACC_PROTECTED);
    zend_declare_property_bool(slim_events_event_ce, SL("_cancelable"), 1, ZEND_ACC_PROTECTED);
    zend_declare_property_bool(slim_events_event_ce, SL("_flag"), 0, ZEND_ACC_PROTECTED);

    zend_class_implements(slim_events_event_ce, 1, slim_events_eventinterface_ce);

    return SUCCESS;
}

PHP_METHOD(Slim_Events_Event, __construct)
{
    zval *type, *source, *data = NULL, *cancelable = NULL, *flag = NULL;

    slim_fetch_params(0, 2, 3, &type, &source, &data, &cancelable, &flag);

    if (!data) {
        data = &SLIM_G(z_null);
    }

    if (!cancelable) {
        cancelable = &SLIM_G(z_true);
    }

    if (!flag) {
        flag = &SLIM_G(z_false);
    }

    slim_update_property(getThis(), SL("_type"), type);
    slim_update_property(getThis(), SL("_source"), source);
    if (Z_TYPE_P(data) != IS_NULL) {
        slim_update_property(getThis(), SL("_data"), data);
    }

    if (SLIM_IS_NOT_TRUE(cancelable)) {
        slim_update_property(getThis(), SL("_cancelable"), cancelable);
    }
    if (SLIM_IS_TRUE(flag)) {
        slim_update_property(getThis(), SL("_flag"), flag);
    }
}

PHP_METHOD(Slim_Events_Event, setName)
{
    zval *event_name;

    slim_fetch_params(0, 1, 0, &event_name);

    slim_update_property(getThis(), SL("_name"), event_name);
}

PHP_METHOD(Slim_Events_Event, getName)
{
    RETURN_MEMBER(getThis(), "_name");
}

PHP_METHOD(Slim_Events_Event, setType)
{
    zval *event_type;

    slim_fetch_params(0, 1, 0, &event_type);

    slim_update_property(getThis(), SL("_type"), event_type);
}

PHP_METHOD(Slim_Events_Event, getType)
{
    RETURN_MEMBER(getThis(), "_type");
}

PHP_METHOD(Slim_Events_Event, setSource)
{
    zval *event_source;

    slim_fetch_params(0, 1, 0, &event_source);

    slim_update_property(getThis(), SL("_source"), event_source);
}

PHP_METHOD(Slim_Events_Event, getSource)
{
    RETURN_MEMBER(getThis(), "_source");
}

PHP_METHOD(Slim_Events_Event, setData)
{
    zval *data;

    slim_fetch_params(0, 1, 0, &data);

    slim_update_property(getThis(), SL("_data"), data);
}

PHP_METHOD(Slim_Events_Event, getData)
{
    RETURN_MEMBER(getThis(), "_data");
}

PHP_METHOD(Slim_Events_Event, setCancelable)
{
    zval *cancelable;

    slim_fetch_params(0, 1, 0, &cancelable);

    slim_update_property(getThis(), SL("_cancelable"), cancelable);
}

PHP_METHOD(Slim_Events_Event, isCancelable)
{
    RETURN_MEMBER(getThis(), "_cancelable");
}

PHP_METHOD(Slim_Events_Event, getFlag)
{
    RETURN_MEMBER(getThis(), "_flag");
}

PHP_METHOD(Slim_Events_Event, stop)
{
    zval cancelable = {};

    slim_read_property(&cancelable, getThis(), SL("_cancelable"), PH_NOISY|PH_READONLY);
    if (zend_is_true(&cancelable)) {
        slim_update_property_bool(getThis(), SL("_stopped"), 1);
    } else {
        SLIM_THROW_EXCEPTION_STR(slim_events_exception_ce, "Trying to cancel a non-cancelable event");
        return;
    }
}

PHP_METHOD(Slim_Events_Event, isStopped)
{
    RETURN_MEMBER(getThis(), "_stopped");
}
