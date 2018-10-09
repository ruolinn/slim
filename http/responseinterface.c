#include "http/responseinterface.h"
#include "kernel/main.h"

zend_class_entry *slim_http_responseinterface_ce;

static const zend_function_entry slim_http_responseinterface_method_entry[] = {
    PHP_ABSTRACT_ME(Slim_Http_ResponseInterface, setContent, NULL)
    PHP_ABSTRACT_ME(Slim_Http_ResponseInterface, send, NULL)
    PHP_FE_END
};

SLIM_INIT_CLASS(Slim_Http_ResponseInterface)
{
    SLIM_REGISTER_INTERFACE(Slim\\Http, ResponseInterface, http_responseinterface, slim_http_responseinterface_method_entry);

    return SUCCESS;
}
