#include "http/response/exception.h"
#include "http/../exception.h"
#include "kernel/main.h"

zend_class_entry *slim_http_response_exception_ce;

SLIM_INIT_CLASS(Slim_Http_Response_Exception)
{
    SLIM_REGISTER_CLASS_EX(Slim\\Http\\Response, Exception, http_response_exception, slim_exception_ce, NULL, 0);

    return SUCCESS;
}
