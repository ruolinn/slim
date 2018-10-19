
#include "app.h"
#include "container.h"
#include "router.h"
#include "router/exception.h"
#include "http/response.h"
#include "http/responseinterface.h"

#include <Zend/zend_closures.h>

#include "kernel/main.h"
#include "kernel/fcall.h"
#include "kernel/string.h"
#include "kernel/operators.h"
#include "kernel/array.h"
#include "kernel/object.h"
#include "kernel/concat.h"
#include "kernel/exception.h"
#include "interned-strings.h"

#include "kernel/debug.h"

zend_class_entry *slim_app_ce;

PHP_METHOD(Slim_App, __construct);
PHP_METHOD(Slim_App, bootstrapContainer);
PHP_METHOD(Slim_App, middleware);
PHP_METHOD(Slim_App, fireMiddleware);
PHP_METHOD(Slim_App, fireRouteMiddleware);
PHP_METHOD(Slim_App, routeMiddleware);
PHP_METHOD(Slim_App, run);

static const zend_function_entry slim_app_method_entry[] = {
    PHP_ME(Slim_App, __construct, NULL, ZEND_ACC_PUBLIC|ZEND_ACC_CTOR)
    PHP_ME(Slim_App, bootstrapContainer, NULL, ZEND_ACC_PROTECTED)
    PHP_ME(Slim_App, middleware, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(Slim_App, fireMiddleware, NULL, ZEND_ACC_PROTECTED)
    PHP_ME(Slim_App, routeMiddleware, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(Slim_App, fireRouteMiddleware, NULL, ZEND_ACC_PROTECTED)
    PHP_ME(Slim_App, run, NULL, ZEND_ACC_PUBLIC)
    PHP_FE_END
};

SLIM_INIT_CLASS(Slim_App)
{
    SLIM_REGISTER_CLASS_EX(Slim, App, app, slim_container_ce, slim_app_method_entry, 0);

    zend_declare_property_null(slim_app_ce, SL("_middleware"), ZEND_ACC_PROTECTED);
    zend_declare_property_null(slim_app_ce, SL("_routeMiddleware"), ZEND_ACC_PROTECTED);

    return SUCCESS;
}

PHP_METHOD(Slim_App, __construct)
{
    SLIM_CALL_SELF(NULL, "bootstrapcontainer");
}

PHP_METHOD(Slim_App, bootstrapContainer)
{
    zval service = {}, definition = {};

    ZVAL_STR(&service, IS(router));
    ZVAL_STRING(&definition, "Slim\\Router");
    SLIM_CALL_SELF(NULL, "set", &service, &definition, &SLIM_G(z_true));
    zval_ptr_dtor(&definition);

    ZVAL_STR(&service, IS(request));
    ZVAL_STRING(&definition, "Slim\\Http\\Request");
    SLIM_CALL_SELF(NULL, "set", &service, &definition, &SLIM_G(z_true));
    zval_ptr_dtor(&definition);

    ZVAL_STR(&service, IS(response));
    ZVAL_STRING(&definition, "Slim\\Http\\Response");
    SLIM_CALL_SELF(NULL, "set", &service, &definition, &SLIM_G(z_true));
    zval_ptr_dtor(&definition);

    ZVAL_STR(&service, IS(events));
    ZVAL_STRING(&definition, "Slim\\Events\\Manager");
    SLIM_CALL_SELF(NULL, "set", &service, &definition, &SLIM_G(z_true));
    zval_ptr_dtor(&definition);
}

PHP_METHOD(Slim_App, middleware)
{
    zval *listeners, middleware = {}, merge_listeners = {}, _listeners, _middleware;

    slim_fetch_params(0, 1, 0, &listeners);

    if (Z_TYPE_P(listeners) != IS_ARRAY) {
        array_init(&_listeners);
        slim_array_append(&_listeners, listeners, PH_COPY);
    } else {
        ZVAL_COPY(&_listeners, listeners);
    }

    slim_read_property(&middleware, getThis(), SL("_middleware"), PH_COPY);
    if (Z_TYPE(middleware) != IS_ARRAY) {
        array_init(&middleware);
    }

    slim_fast_array_merge(&merge_listeners, &middleware, &_listeners);

    SLIM_CALL_FUNCTION(&_middleware, "array_unique", &merge_listeners);

    slim_update_property(getThis(), SL("_middleware"), &_middleware);
}

PHP_METHOD(Slim_App, fireMiddleware)
{
    zval *app, listeners = {}, *listener, status = {};

    slim_fetch_params(0, 1, 0, &app);

    slim_read_property(&listeners, getThis(), SL("_middleware"), PH_COPY);

    if (unlikely(Z_TYPE(listeners) != IS_ARRAY)) {
        return;
    }

    ZEND_HASH_FOREACH_VAL(Z_ARRVAL(listeners), listener) {
        zval handler = {};
        if (Z_TYPE_P(listener) == IS_STRING) {
            SLIM_CALL_SELF(&handler, "get", listener);

            if (slim_method_exists_ex(&handler, SL("handle")) == SUCCESS) {
                SLIM_CALL_METHOD(&status, &handler, "handle");

                if (SLIM_IS_FALSE(&status)) {
                    break;
                }

                zval_ptr_dtor(&handler);
            }
        }


    } ZEND_HASH_FOREACH_END();

    RETURN_NCTOR(&status);
}

PHP_METHOD(Slim_App, routeMiddleware)
{
    zval *listeners, route_middleware, merge_route_middleware;

    slim_fetch_params(0, 1, 0, &listeners);

    slim_read_property(&route_middleware, getThis(), SL("_routeMiddleware"), PH_COPY);
    if (Z_TYPE(route_middleware) != IS_ARRAY) {
        array_init(&route_middleware);
    }

    slim_fast_array_merge(&merge_route_middleware, &route_middleware, listeners);

    slim_update_property(getThis(), SL("_routeMiddleware"), &merge_route_middleware);
}

PHP_METHOD(Slim_App, fireRouteMiddleware)
{
    zval *names, *name, route_listeners, listener = {}, status = {};

    slim_fetch_params(0, 1, 0, &names);

    slim_read_property(&route_listeners, getThis(), SL("_routeMiddleware"), PH_COPY);

    ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(names), name) {
        zval listener = {}, handler = {};
        if (slim_array_isset_fetch(&listener, &route_listeners, name, PH_READONLY)) {
            if (Z_TYPE(listener) == IS_STRING) {
                SLIM_CALL_SELF(&handler, "get", &listener);

                if (slim_method_exists_ex(&handler, SL("handle")) == SUCCESS) {
                    SLIM_CALL_METHOD(&status, &handler, "handle");

                    if (SLIM_IS_FALSE(&status)) {
                        break;
                    }
                }

                zval_ptr_dtor(&handler);
            }
        }

    } ZEND_HASH_FOREACH_END();

    RETURN_NCTOR(&status);
}

PHP_METHOD(Slim_App, run)
{
    zval http_method = {}, uri = {}, service = {}, router = {}, request = {}, callable = {}, route = {}, parts = {};
    zval route_paths, response = {}, possible_response = {}, returned_response = {}, returned_response_sent = {};
    zval controller_name = {}, action_name = {}, has_service = {}, exception_message = {}, call_object = {}, handler = {};
    zval status = {}, middleware;

    SLIM_CALL_METHOD(&status, getThis(), "firemiddleware", getThis());

    if (SLIM_IS_FALSE(&status)) {
        RETURN_FALSE;
    }
    zval_ptr_dtor(&status);

    ZVAL_STR(&service, IS(request));
    SLIM_CALL_SELF(&request, "getshared", &service);
    SLIM_CALL_METHOD(&uri, &request, "getpathinfo");
    SLIM_CALL_METHOD(&http_method, &request, "getmethod");

    ZVAL_STR(&service, IS(router));
    SLIM_CALL_SELF(&router, "getshared", &service);

    SLIM_CALL_METHOD(NULL, &router, "dispatch", &http_method, &uri);

    SLIM_CALL_METHOD(&route, &router, "getmatchedroute");
    SLIM_CALL_METHOD(&callable, &route, "getcallable");
    SLIM_CALL_METHOD(&middleware, &route, "getmiddleware");

    SLIM_CALL_METHOD(&status, getThis(), "fireroutemiddleware", &middleware);

    if (Z_TYPE(callable) == IS_OBJECT) {
        if (instanceof_function(Z_OBJCE(callable), zend_ce_closure)) {
            SLIM_CALL_USER_FUNC_ARRAY(&possible_response, &callable, NULL);
        }
    } else if (Z_TYPE(callable) == IS_STRING) {
        slim_fast_explode_str(&parts, SL("::"), &callable);
        // @TODO 判断下小于2返回异常
        slim_array_fetch_long(&controller_name, &parts, 0, PH_NOISY|PH_READONLY);
        slim_array_fetch_long(&action_name, &parts, 1, PH_NOISY|PH_READONLY);

        SLIM_CALL_SELF(&has_service, "has", &controller_name);
        if (!zend_is_true(&has_service)) {
            ZVAL_BOOL(&has_service, (slim_class_exists(&controller_name, 1) != NULL) ? 1 : 0);
        }

        if (!zend_is_true(&has_service)) {
            SLIM_CONCAT_VS(&exception_message, &controller_name, " handler class cannot be loaded");
            SLIM_THROW_EXCEPTION_ZVAL(slim_router_route_exception_ce, &exception_message);
        }

        SLIM_CALL_SELF(&handler, "getshared", &controller_name);

        array_init_size(&call_object, 2);
        slim_array_append(&call_object, &handler, PH_COPY);
        slim_array_append(&call_object, &action_name, PH_COPY);

        SLIM_CALL_USER_FUNC_ARRAY(&possible_response, &call_object, NULL);
        //SLIM_CALL_USER_FUNC_ARRAY(&possible_response, &parts, NULL);
    }

    if (Z_TYPE(possible_response) == IS_OBJECT && instanceof_function_ex(Z_OBJCE(possible_response), slim_http_responseinterface_ce, 1)) {
        ZVAL_COPY_VALUE(&response, &possible_response);
        ZVAL_TRUE(&returned_response);
    } else {
        ZVAL_STR(&service, IS(response));
        SLIM_CALL_METHOD(&response, getThis(), "getshared", &service);

        ZVAL_FALSE(&returned_response);
    }

    if (SLIM_IS_FALSE(&returned_response)) {
        if (Z_TYPE(possible_response) == IS_STRING) {
            SLIM_CALL_METHOD(NULL, &response, "setcontent", &possible_response);
        } else if (Z_TYPE(possible_response) == IS_ARRAY) {
            SLIM_CALL_METHOD(NULL, &response, "setjsoncontent", &possible_response);
        }
    }

    SLIM_CALL_METHOD(&returned_response_sent, &response, "issent");

    if (SLIM_IS_FALSE(&returned_response_sent)) {
        SLIM_CALL_METHOD(NULL, &response, "send");
    }
}
