#include "events/managerinterface.h"
#include "events/manager.h"
#include "events/exception.h"
#include "events/listener.h"
#include "events/event.h"
#include "events/eventinterface.h"
#include "kernel/main.h"
#include "kernel/exception.h"
#include "kernel/string.h"
#include "kernel/object.h"
#include "kernel/array.h"
#include "kernel/fcall.h"
#include "kernel/operators.h"
#include "kernel/concat.h"

#include <Zend/zend_closures.h>
#include <ext/spl/spl_heap.h>

zend_class_entry *slim_events_manager_ce;

PHP_METHOD(Slim_Events_Manager, enablePriorities);
PHP_METHOD(Slim_Events_Manager, arePrioritiesEnabled);
PHP_METHOD(Slim_Events_Manager, attach);
PHP_METHOD(Slim_Events_Manager, detach);
PHP_METHOD(Slim_Events_Manager, detachAll);
PHP_METHOD(Slim_Events_Manager, fire);
PHP_METHOD(Slim_Events_Manager, createEvent);
PHP_METHOD(Slim_Events_Manager, fireQueue);
PHP_METHOD(Slim_Events_Manager, collectResponses);
PHP_METHOD(Slim_Events_Manager, isCollecting);
PHP_METHOD(Slim_Events_Manager, getResponses);
PHP_METHOD(Slim_Events_Manager, hasListeners);
PHP_METHOD(Slim_Events_Manager, getListeners);
PHP_METHOD(Slim_Events_Manager, getEvents);
PHP_METHOD(Slim_Events_Manager, getCurrentEvent);

static const zend_function_entry slim_events_manager_method_entry[] = {
    PHP_ME(Slim_Events_Manager, enablePriorities, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(Slim_Events_Manager, arePrioritiesEnabled, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(Slim_Events_Manager, attach, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(Slim_Events_Manager, detach, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(Slim_Events_Manager, detachAll, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(Slim_Events_Manager, fire, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(Slim_Events_Manager, createEvent, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(Slim_Events_Manager, fireQueue, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(Slim_Events_Manager, collectResponses, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(Slim_Events_Manager, isCollecting, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(Slim_Events_Manager, getResponses, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(Slim_Events_Manager, hasListeners, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(Slim_Events_Manager, getListeners, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(Slim_Events_Manager, getEvents, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(Slim_Events_Manager, getCurrentEvent, NULL, ZEND_ACC_PUBLIC)
    PHP_FE_END
};

SLIM_INIT_CLASS(Slim_Events_Manager)
{
    SLIM_REGISTER_CLASS(Slim\\Events, Manager, events_manager, slim_events_manager_method_entry, 0);

    zend_declare_property_null(slim_events_manager_ce, SL("_events"), ZEND_ACC_PROTECTED);
    zend_declare_property_null(slim_events_manager_ce, SL("_currentEvent"), ZEND_ACC_PROTECTED);
    zend_declare_property_bool(slim_events_manager_ce, SL("_collect"), 0, ZEND_ACC_PROTECTED);
    zend_declare_property_bool(slim_events_manager_ce, SL("_enablePriorities"), 0, ZEND_ACC_PROTECTED);
    zend_declare_property_null(slim_events_manager_ce, SL("_responses"), ZEND_ACC_PROTECTED);

    zend_class_implements(slim_events_manager_ce, 1, slim_events_managerinterface_ce);

    return SUCCESS;
}

PHP_METHOD(Slim_Events_Manager, enablePriorities)
{
    zval *enable_priorities;

    slim_fetch_params(0, 1, 0, &enable_priorities);

    slim_update_property(getThis(), SL("_enablePriorities"), enable_priorities);
}

PHP_METHOD(Slim_Events_Manager, arePrioritiesEnabled)
{
    RETURN_MEMBER(getThis(), "_enablePriorities");
}

PHP_METHOD(Slim_Events_Manager, attach)
{
    zval *event_type, *handler, *_priority = NULL, event = {}, priority = {}, events = {}, listener = {}, enable_priorities = {}, priority_queue = {};

    slim_fetch_params(0, 2, 1, &event_type, &handler, &_priority);

    if (!_priority) {
        ZVAL_LONG(&priority, 100);
    } else {
        ZVAL_COPY_VALUE(&priority, _priority);
    }

    if (Z_TYPE_P(handler) != IS_OBJECT && !slim_is_callable(handler)) {
        SLIM_THROW_EXCEPTION_STR(slim_events_exception_ce, "Event handler must be an object or callable");
        return;
    }

    if (!slim_memnstr_str(event_type, SL(":"))) {
        ZVAL_COPY(&event, event_type);
    } else {
        zval event_parts ={}, name = {}, type = {};
        slim_fast_explode_str(&event_parts, SL(":"), event_type);
        slim_array_fetch_long(&name, &event_parts, 0, PH_READONLY);
        slim_array_fetch_long(&type, &event_parts, 1, PH_READONLY);
        if (SLIM_IS_STRING(&type, "*") || SLIM_IS_EMPTY_STRING(&type)) {
            ZVAL_COPY(&event, &name);
        } else {
            ZVAL_COPY(&event, event_type);
        }
        zval_ptr_dtor(&event_parts);
    }

    if (slim_instance_of_ev(handler, slim_events_listener_ce)) {
        ZVAL_COPY(&listener, handler);
        SLIM_CALL_METHOD(NULL, &listener, "setpriority", &priority);
        SLIM_CALL_METHOD(NULL, &listener, "setevent", &event);
    } else {
        object_init_ex(&listener, slim_events_listener_ce);
        SLIM_CALL_METHOD(NULL, &listener, "__construct", handler, &priority, &event);
    }

    slim_read_property(&events, getThis(), SL("_events"), PH_COPY);
    if (Z_TYPE(events) != IS_ARRAY) {
        array_init(&events);
    }

    if (!slim_array_isset_fetch(&priority_queue, &events, &event, PH_COPY)) {
        slim_read_property(&enable_priorities, getThis(), SL("_enablePriorities"), PH_READONLY);
        if (zend_is_true(&enable_priorities)) {

            object_init_ex(&priority_queue, spl_ce_SplPriorityQueue);
            if (slim_has_constructor(&priority_queue)) {
                SLIM_CALL_METHOD(NULL, &priority_queue, "__construct");
            }

            SLIM_CALL_METHOD(NULL, &priority_queue, "setextractflags", &SLIM_G(z_one));
        } else {
            array_init(&priority_queue);
        }
    }

    if (unlikely(Z_TYPE(priority_queue) == IS_OBJECT)) {
        SLIM_CALL_METHOD(NULL, &priority_queue, "insert", &listener, &priority);
    } else {
        slim_array_append(&priority_queue, &listener, PH_COPY);
    }
    zval_ptr_dtor(&listener);

    slim_array_update(&events, &event, &priority_queue, 0);
    slim_update_property(getThis(), SL("_events"), &events);
    zval_ptr_dtor(&events);
    zval_ptr_dtor(&event);
}

PHP_METHOD(Slim_Events_Manager, detach)
{
    zval *type, *handler, events = {}, queue = {}, priority_queue = {}, *listener;
    zend_string *str_key;
    ulong idx;

    slim_fetch_params(0, 2, 0, &type, &handler);

    if (Z_TYPE_P(handler) != IS_OBJECT && !slim_is_callable(handler)) {
        SLIM_THROW_EXCEPTION_STR(slim_events_exception_ce, "Event handler must be an object or callable");
        return;
    }

    slim_read_property(&events, getThis(), SL("_events"), PH_READONLY);
    if (Z_TYPE(events) != IS_ARRAY) {
        RETURN_FALSE;
    }

    if (!slim_array_isset_fetch(&queue, &events, type, PH_READONLY)) {
        RETURN_FALSE;
    }

    if (Z_TYPE(queue) == IS_OBJECT) {
        object_init_ex(&priority_queue, spl_ce_SplPriorityQueue);
        if (slim_has_constructor(&priority_queue)) {
            SLIM_CALL_METHOD(NULL, &priority_queue, "__construct");
        }

        SLIM_CALL_METHOD(NULL, &queue, "top");

        while (1) {
            zval r0 = {}, listener0 = {}, handler_embeded = {}, priority = {};
            SLIM_CALL_METHOD(&r0, &queue, "valid");
            if (!zend_is_true(&r0)) {
                break;
            }

            SLIM_CALL_METHOD(&listener0, &queue, "current");
            SLIM_CALL_METHOD(&handler_embeded, &listener0, "getlistener");

            if (!slim_is_equal(&handler_embeded, handler)) {
                SLIM_CALL_METHOD(&priority, &listener0, "getpriority");
                SLIM_CALL_METHOD(NULL, &priority_queue, "insert", &listener0, &priority);
                zval_ptr_dtor(&priority);
            }
            zval_ptr_dtor(&listener0);
            zval_ptr_dtor(&handler_embeded);

            SLIM_CALL_METHOD(NULL, &queue, "next");
        }
    } else {
        ZVAL_DUP(&priority_queue, &queue);
        ZEND_HASH_FOREACH_KEY_VAL(Z_ARRVAL(queue), idx, str_key, listener) {
            zval key = {}, handler_embeded = {};
            if (str_key) {
                ZVAL_STR(&key, str_key);
            } else {
                ZVAL_LONG(&key, idx);
            }

            SLIM_CALL_METHOD(&handler_embeded, listener, "getlistener");

            if (slim_is_equal_object(&handler_embeded, handler)) {
                slim_array_unset(&priority_queue, &key, 0);
            }
            zval_ptr_dtor(&handler_embeded);

        } ZEND_HASH_FOREACH_END();
    }

    slim_array_update(&events, type, &priority_queue, 0);
}

PHP_METHOD(Slim_Events_Manager, detachAll)
{
    zval *type = NULL, events = {};

    slim_fetch_params(0, 0, 1, &type);

    if (!type) {
        type = &SLIM_G(z_null);
    }

    slim_read_property(&events, getThis(), SL("_events"), PH_READONLY);
    if (Z_TYPE_P(type) != IS_NULL && slim_array_isset(&events, type)) {
        slim_array_unset(&events, type, 0);
    }
}

PHP_METHOD(Slim_Events_Manager, fire)
{
    zval *_event_type, *source, *data = NULL, *cancelable = NULL, *flag = NULL, debug_message = {};
    zval event_type = {}, events = {}, name = {}, type = {}, status = {}, collect = {}, any_type = {}, event = {}, fire_events = {};

    slim_fetch_params(0, 2, 3, &_event_type, &source, &data, &cancelable, &flag);

    if (!data) {
        data = &SLIM_G(z_null);
    }

    if (!cancelable || Z_TYPE_P(cancelable) == IS_NULL) {
        cancelable = &SLIM_G(z_true);
    }

    if (!flag || Z_TYPE_P(flag) == IS_NULL) {
        flag = &SLIM_G(z_false);
    }

    if (unlikely(Z_TYPE_P(_event_type) != IS_STRING)) {
        if (Z_TYPE_P(_event_type) != IS_OBJECT) {
            SLIM_THROW_EXCEPTION_STR(slim_events_exception_ce, "Event type must be a string");
            return;
        } else {
            SLIM_VERIFY_INTERFACE_EX(_event_type, slim_events_eventinterface_ce, slim_events_exception_ce);
            ZVAL_COPY(&event, _event_type);
            SLIM_CALL_METHOD(&name, &event, "getname");
            SLIM_CALL_METHOD(&type, &event, "gettype");
            SLIM_CONCAT_VSV(&event_type, &name, ":", &type);
        }
    } else {
        ZVAL_COPY(&event_type, _event_type);
        SLIM_CALL_METHOD(&event, getThis(), "createevent", &event_type, source, data, cancelable, flag);
        SLIM_CALL_METHOD(&name, &event, "getname");
    }

    slim_update_property(getThis(), SL("_currentEvent"), &event);

    slim_read_property(&events, getThis(), SL("_events"), PH_READONLY);
    if (Z_TYPE(events) != IS_ARRAY) {
        RETURN_NULL();
    }

    ZVAL_NULL(&status);

    slim_read_property(&collect, getThis(), SL("_collect"), PH_READONLY);
    if (zend_is_true(&collect)) {
        slim_update_property_null(getThis(), SL("_responses"));
    }

    ZVAL_STRING(&any_type, "*");
    if (slim_array_isset_fetch(&fire_events, &events, &any_type, PH_READONLY)) {
        if (Z_TYPE(fire_events) == IS_ARRAY || Z_TYPE(fire_events) == IS_OBJECT) {
            /**
             * Call the events queue
             */
            SLIM_CALL_METHOD(&status, getThis(), "firequeue", &fire_events, &event);
            if (zend_is_true(flag) && SLIM_IS_FALSE(&status)) {
                RETURN_FALSE;
            }
        }
    }

    if (slim_array_isset_fetch(&fire_events, &events, &name, PH_READONLY)) {
        if (Z_TYPE(fire_events) == IS_ARRAY || Z_TYPE(fire_events) == IS_OBJECT) {
            /**
             * Call the events queue
             */
            SLIM_CALL_METHOD(&status, getThis(), "firequeue", &fire_events, &event);
            if (zend_is_true(flag) && SLIM_IS_FALSE(&status)) {
                RETURN_FALSE;
            }
        }
    }

    if (slim_array_isset_fetch(&fire_events, &events, &event_type, PH_READONLY)) {
        if (Z_TYPE(fire_events) == IS_ARRAY || Z_TYPE(fire_events) == IS_OBJECT) {
            /**
             * Call the events queue
             */
            SLIM_CALL_METHOD(&status, getThis(), "firequeue", &fire_events, &event);
            if (zend_is_true(flag) && SLIM_IS_FALSE(&status)) {
                RETURN_FALSE;
            }
        }
    }

    RETURN_NCTOR(&status);
}

PHP_METHOD(Slim_Events_Manager, createEvent)
{
    zval *event_type, *source, *data = NULL, *cancelable = NULL, *flag = NULL, exception_message = {};
    zval event_parts = {}, name = {}, type = {};

    slim_fetch_params(0, 2, 3, &event_type, &source, &data, &cancelable, &flag);

    if (!data) {
        data = &SLIM_G(z_null);
    }

    if (!cancelable) {
        cancelable = &SLIM_G(z_true);
    }

    if (!flag) {
        flag = &SLIM_G(z_false);
    }

    if (!slim_memnstr_str(event_type, SL(":"))) {
        SLIM_CONCAT_SV(&exception_message, "Invalid event type ", event_type);
        SLIM_THROW_EXCEPTION_ZVAL(slim_events_exception_ce, &exception_message);
        zval_ptr_dtor(&exception_message);
        return;
    }

    slim_fast_explode_str(&event_parts, SL(":"), event_type);
    slim_array_fetch_long(&name, &event_parts, 0, PH_READONLY);
    slim_array_fetch_long(&type, &event_parts, 1, PH_READONLY);

    object_init_ex(return_value, slim_events_event_ce);
    SLIM_CALL_METHOD(NULL, return_value, "__construct", &type, source, data, cancelable, flag);
    SLIM_CALL_METHOD(NULL, return_value, "setname", &name);
    zval_ptr_dtor(&event_parts);
}

PHP_METHOD(Slim_Events_Manager, fireQueue)
{
    zval *queue, *event, *flag = NULL, *_prev_data = NULL, event_name = {}, source = {}, data = {}, cancelable = {}, collect = {}, iterator = {}, *listener;
    zval status = {};
    zend_class_entry *ce, *weakref_ce;

    slim_fetch_params(0, 2, 1, &queue, &event, &flag, &_prev_data);

    if (!flag) {
        flag = &SLIM_G(z_false);
    }

    if (_prev_data) {
        ZVAL_COPY(&status, _prev_data);
    } else {
        ZVAL_NULL(&status);
    }

    if (unlikely(Z_TYPE_P(queue) != IS_ARRAY)) {
        if (Z_TYPE_P(queue) == IS_OBJECT) {
            ce = Z_OBJCE_P(queue);
            if (!instanceof_function_ex(ce, slim_events_event_ce, 0) && !instanceof_function_ex(ce, spl_ce_SplPriorityQueue, 0)) {
                SLIM_THROW_EXCEPTION_FORMAT(slim_events_exception_ce, "Unexpected value type: expected object of type Slim\\Events\\Event or SplPriorityQueue, %s given", ce->name->val);
                return;
            }
        } else {
            SLIM_THROW_EXCEPTION_FORMAT(slim_events_exception_ce, "Unexpected value type: expected object of type Slim\\Events\\Event or SplPriorityQueue, %s given", zend_zval_type_name(queue));
            return;
        }
    }

    SLIM_VERIFY_CLASS_EX(event, slim_events_event_ce, slim_events_exception_ce);

    SLIM_CALL_METHOD(&event_name, event, "gettype");

    weakref_ce = slim_class_str_exists(SL("WeakRef"), 0);

    if (unlikely(Z_TYPE(event_name) != IS_STRING)) {
        SLIM_THROW_EXCEPTION_STR(slim_events_exception_ce, "The event type not valid");
        return;
    }

    SLIM_CALL_METHOD(&source, event, "getsource");

    SLIM_CALL_METHOD(&data, event, "getdata");

    SLIM_CALL_METHOD(&cancelable, event, "iscancelable");

    slim_read_property(&collect, getThis(), SL("_collect"), PH_READONLY);
    if (Z_TYPE_P(queue) == IS_OBJECT) {

        if (slim_clone(&iterator, queue) == FAILURE) {
            return;
        }

        SLIM_CALL_METHOD(NULL, &iterator, "top");

        while (1) {
            zval r0 = {}, listener0 = {}, handler_embeded = {}, handler_referenced = {}, handler = {}, arguments = {}, is_stopped = {};

            SLIM_CALL_METHOD(&r0, &iterator, "valid");
            if (!zend_is_true(&r0)) {
                break;
            }

            SLIM_CALL_METHOD(&listener0, &iterator, "current");
            SLIM_CALL_METHOD(&handler_embeded, &listener0, "getlistener");

            /**
             * Only handler objects are valid
             */
            if (Z_TYPE(handler_embeded) == IS_OBJECT) {
                /**
                 * Check if the event is a weak reference.
                 */
                if (weakref_ce && instanceof_function(Z_OBJCE(handler_embeded), weakref_ce)) {
                    /**
                     * Checks whether the object referenced still exists.
                     */
                    SLIM_CALL_METHOD(&handler_referenced, &handler_embeded, "valid");

                    if (zend_is_true(&handler_referenced)) {
                        SLIM_CALL_METHOD(&handler, &handler_embeded, "get");
                    } else {
                        /**
                         * Move the queue to the next handler
                         */
                        SLIM_CALL_METHOD(NULL, &iterator, "next");
                        zval_ptr_dtor(&handler_embeded);
                        continue;
                    }

                } else {
                    ZVAL_COPY(&handler, &handler_embeded);
                }

                /**
                 * Check if the event is a closure
                 */
                assert(Z_TYPE(handler) == IS_OBJECT);
                if (instanceof_function(Z_OBJCE(handler), zend_ce_closure)) {
                    /**
                     * Create the closure arguments
                     */
                    array_init_size(&arguments, 4);
                    slim_array_append(&arguments, event, PH_COPY);
                    slim_array_append(&arguments, &source, PH_COPY);
                    slim_array_append(&arguments, &data, PH_COPY);
                    slim_array_append(&arguments, &status, 0);

                    /**
                     * Call the function in the PHP userland
                     */
                    SLIM_CALL_USER_FUNC_ARRAY(&status, &handler, &arguments);
                    zval_ptr_dtor(&arguments);
                    if (zend_is_true(flag) && SLIM_IS_FALSE(&status)){
                        break;
                    }

                    /**
                     * Trace the response
                     */
                    if (zend_is_true(&collect)) {
                        slim_update_property_array_append(getThis(), SL("_responses"), &status);
                    }

                    if (zend_is_true(&cancelable)) {
                        /**
                         * Check if the event was stopped by the user
                         */
                        SLIM_CALL_METHOD(&is_stopped, event, "isstopped");
                        if (zend_is_true(&is_stopped)) {
                            break;
                        }
                    }
                } else {
                    /**
                     * Check if the listener has implemented an event with the same name
                     */
                    if (slim_method_exists(&handler, &event_name) == SUCCESS || slim_method_exists_ex(&handler, SL("__call")) == SUCCESS) {
                        zval prev_data = {};
                        ZVAL_COPY(&prev_data, &status);
                        zval_ptr_dtor(&status);
                        /**
                         * Call the function in the PHP userland
                         */
                        SLIM_CALL_METHOD(&status, &handler, Z_STRVAL(event_name), event, &source, &data, &prev_data);
                        zval_ptr_dtor(&prev_data);
                        if (zend_is_true(flag) && SLIM_IS_FALSE(&status)){
                            break;
                        }

                        /**
                         * Collect the response
                         */
                        if (zend_is_true(&collect)) {
                            slim_update_property_array_append(getThis(), SL("_responses"), &status);
                        }

                        if (zend_is_true(&cancelable)) {

                            /**
                             * Check if the event was stopped by the user
                             */
                            SLIM_CALL_METHOD(&is_stopped, event, "isstopped");
                            if (zend_is_true(&is_stopped)) {
                                zval_ptr_dtor(&handler_embeded);
                                zval_ptr_dtor(&listener0);
                                break;
                            }
                        }
                    }
                }
                zval_ptr_dtor(&handler);
            }
            zval_ptr_dtor(&handler_embeded);
            zval_ptr_dtor(&listener0);

            /**
             * Move the queue to the next handler
             */
            SLIM_CALL_METHOD(NULL, &iterator, "next");
        }
    } else {
        ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(queue), listener) {
            zval handler_embeded = {}, handler_referenced = {}, handler = {}, arguments = {}, is_stopped = {};

            SLIM_CALL_METHOD(&handler_embeded, listener, "getlistener");
            /**
             * Only handler objects are valid
             */
            if (Z_TYPE(handler_embeded) == IS_OBJECT) {

                /**
                 * Check if the event is a weak reference.
                 */
                if (weakref_ce && instanceof_function(Z_OBJCE(handler_embeded), weakref_ce)) {
                    /**
                     * Checks whether the object referenced still exists.
                     */
                    SLIM_CALL_METHOD(&handler_referenced, &handler_embeded, "valid");

                    if (zend_is_true(&handler_referenced)) {
                        SLIM_CALL_METHOD(&handler, &handler_embeded, "get");
                    } else {
                        continue;
                    }

                } else {
                    ZVAL_COPY(&handler, &handler_embeded);
                }

                /**
                 * Check if the event is a closure
                 */
                assert(Z_TYPE(handler) == IS_OBJECT);
                if (instanceof_function(Z_OBJCE(handler), zend_ce_closure)) {
                    /**
                     * Create the closure arguments
                     */
                    array_init_size(&arguments, 4);
                    slim_array_append(&arguments, event, PH_COPY);
                    slim_array_append(&arguments, &source, PH_COPY);
                    slim_array_append(&arguments, &data, PH_COPY);
                    slim_array_append(&arguments, &status, 0);

                    /**
                     * Call the function in the PHP userland
                     */
                    SLIM_CALL_USER_FUNC_ARRAY(&status, &handler, &arguments);
                    zval_ptr_dtor(&arguments);
                    if (zend_is_true(flag) && SLIM_IS_FALSE(&status)){
                        break;
                    }

                    /**
                     * Trace the response
                     */
                    if (zend_is_true(&collect)) {
                        slim_update_property_array_append(getThis(), SL("_responses"), &status);
                    }

                    if (zend_is_true(&cancelable)) {

                        /**
                         * Check if the event was stopped by the user
                         */
                        SLIM_CALL_METHOD(&is_stopped, event, "isstopped");
                        if (zend_is_true(&is_stopped)) {
                            break;
                        }
                    }
                } else {
                    /**
                     * Check if the listener has implemented an event with the same name
                     */
                    if (slim_method_exists(&handler, &event_name) == SUCCESS || slim_method_exists_ex(&handler, SL("__call")) == SUCCESS) {
                        zval prev_data = {};
                        ZVAL_COPY(&prev_data, &status);
                        zval_ptr_dtor(&status);
                        /**
                         * Call the function in the PHP userland
                         */
                        SLIM_CALL_METHOD(&status, &handler, Z_STRVAL(event_name), event, &source, &data, &prev_data);
                        zval_ptr_dtor(&prev_data);
                        if (zend_is_true(flag) && SLIM_IS_FALSE(&status)){
                            break;
                        }

                        /**
                         * Collect the response
                         */
                        if (zend_is_true(&collect)) {
                            slim_update_property_array_append(getThis(), SL("_responses"), &status);
                        }

                        if (zend_is_true(&cancelable)) {
                            /**
                             * Check if the event was stopped by the user
                             */
                            SLIM_CALL_METHOD(&is_stopped, event, "isstopped");
                            if (zend_is_true(&is_stopped)) {
                                break;
                            }
                        }
                    }
                }
                zval_ptr_dtor(&handler);
            }
            zval_ptr_dtor(&handler_embeded);
        } ZEND_HASH_FOREACH_END();
    }
    zval_ptr_dtor(&event_name);
    zval_ptr_dtor(&source);
    zval_ptr_dtor(&data);
    zval_ptr_dtor(&cancelable);
    RETURN_NCTOR(&status);
}

PHP_METHOD(Slim_Events_Manager, collectResponses)
{
    zval *collect;

    slim_fetch_params(0, 1, 0, &collect);

    slim_update_property(getThis(), SL("_collect"), collect);
}

PHP_METHOD(Slim_Events_Manager, isCollecting)
{
    RETURN_MEMBER(getThis(), "_collect");
}

PHP_METHOD(Slim_Events_Manager, getResponses)
{
    RETURN_MEMBER(getThis(), "_responses");
}

PHP_METHOD(Slim_Events_Manager, hasListeners)
{
    zval *type, events = {};

    slim_fetch_params(0, 1, 0, &type);

    slim_read_property(&events, getThis(), SL("_events"), PH_READONLY);
    if (slim_array_isset(&events, type)) {
        RETURN_TRUE;
    }

    RETURN_FALSE;
}

PHP_METHOD(Slim_Events_Manager, getListeners)
{
	zval *type, *full = NULL, events = {}, queue = {}, iterator = {}, *listener;

	slim_fetch_params(0, 1, 1, &type, &full);

	if (!full) {
		full = &SLIM_G(z_false);
	}

	slim_read_property(&events, getThis(), SL("_events"), PH_READONLY);
	if (Z_TYPE(events) != IS_ARRAY) {
		RETURN_EMPTY_ARRAY();
	}

	if (!slim_array_isset(&events, type)) {
		RETURN_EMPTY_ARRAY();
	}

	array_init(return_value);

	slim_array_fetch(&queue, &events, type, PH_NOISY|PH_READONLY);

	if (zend_is_true(full)) {
		RETURN_CTOR(&queue);
	}

	if (Z_TYPE(queue) == IS_OBJECT) {
		if (slim_clone(&iterator, &queue) == FAILURE) {
			return;
		}

		SLIM_CALL_METHOD(NULL, &iterator, "top");

		while (1) {
			zval r0 = {}, listener0 = {}, handler_embeded = {};

			SLIM_CALL_METHOD(&r0, &iterator, "valid");
			if (!zend_is_true(&r0)) {
				break;
			}

			SLIM_CALL_METHOD(&listener0, &iterator, "current");
			SLIM_CALL_METHOD(&handler_embeded, &listener0, "getlistener");
			zval_ptr_dtor(&listener0);

			slim_array_append(return_value, &handler_embeded, 0);

			SLIM_CALL_METHOD(NULL, &iterator, "next");
		}
		zval_ptr_dtor(&iterator);
	} else {
		ZEND_HASH_FOREACH_VAL(Z_ARRVAL(queue), listener) {
			zval handler_embeded = {};
			SLIM_CALL_METHOD(&handler_embeded, listener, "getlistener");
			slim_array_append(return_value, &handler_embeded, 0);
		} ZEND_HASH_FOREACH_END();
	}
}

PHP_METHOD(Slim_Events_Manager, getEvents)
{
    zval events = {};

    slim_read_property(&events, getThis(), SL("_events"), PH_READONLY);
    slim_array_keys(return_value, &events);
}

PHP_METHOD(Slim_Events_Manager, getCurrentEvent)
{
    RETURN_MEMBER(getThis(), "_currentEvent");
}
