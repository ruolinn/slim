#ifndef SLIM_HTTP_REQUEST_H
#define SLIM_HTTP_REQUEST_H

#include "php_slim.h"

extern zend_class_entry *slim_http_request_ce;

static const char* slim_http_request_getmethod_helper();

SLIM_INIT_CLASS(Slim_Http_Request);

#endif
