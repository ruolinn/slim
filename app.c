
#include "app.h"
#include "container.h"
#include "router.h"
#include "http/response.h"
#include "http/responseinterface.h"

#include <Zend/zend_closures.h>

#include "kernel/main.h"
#include "kernel/fcall.h"
#include "kernel/string.h"
#include "kernel/operators.h"
#include "interned-strings.h"



zend_class_entry *slim_app_ce;

PHP_METHOD(Slim_App, __construct);
PHP_METHOD(Slim_App, bootstrapContainer);
PHP_METHOD(Slim_App, run);

static const zend_function_entry slim_app_method_entry[] = {
    PHP_ME(Slim_App, __construct, NULL, ZEND_ACC_PUBLIC|ZEND_ACC_CTOR)
    PHP_ME(Slim_App, bootstrapContainer, NULL, ZEND_ACC_PROTECTED)
    PHP_ME(Slim_App, run, NULL, ZEND_ACC_PUBLIC)
    PHP_FE_END
};

SLIM_INIT_CLASS(Slim_App)
{
    SLIM_REGISTER_CLASS_EX(Slim, App, app, slim_container_ce, slim_app_method_entry, 0);

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
}

PHP_METHOD(Slim_App, run)
{
    zval uri = {}, service = {}, router = {}, request = {}, callable = {}, route = {}, parts = {};
    zval route_paths, response = {}, possible_response = {}, returned_response = {}, returned_response_sent = {};


	ZVAL_STR(&service, IS(request));
	SLIM_CALL_SELF(&request, "getshared", &service);
	SLIM_CALL_METHOD(&uri, &request, "getpathinfo");

    ZVAL_STR(&service, IS(router));
    SLIM_CALL_SELF(&router, "getshared", &service);

    SLIM_CALL_METHOD(NULL, &router, "handle", &uri);

	SLIM_CALL_METHOD(&route, &router, "getmatchedroute");
	SLIM_CALL_METHOD(&callable, &route, "getCallable");

	if (Z_TYPE(callable) == IS_OBJECT) {
		if (instanceof_function(Z_OBJCE(callable), zend_ce_closure)) {
			SLIM_CALL_USER_FUNC_ARRAY(&possible_response, &callable, NULL);
		}
	} else if (Z_TYPE(callable) == IS_STRING) {
		slim_fast_explode_str(&parts, SL("::"), &callable);
		SLIM_CALL_USER_FUNC_ARRAY(&possible_response, &parts, NULL);
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
