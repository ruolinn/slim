#include "php_slim.h"
#include "interned-strings.h"
#include "kernel/main.h"
#include "kernel/string.h"

zend_string *slim_interned_DELETE						= NULL;
zend_string *slim_interned_GET							= NULL;
zend_string *slim_interned_HEAD							= NULL;
zend_string *slim_interned_OPTIONS						= NULL;
zend_string *slim_interned_PATCH							= NULL;
zend_string *slim_interned_POST							= NULL;
zend_string *slim_interned_PUT							= NULL;
zend_string *slim_interned_static = NULL;
zend_string *slim_interned_self = NULL;
zend_string *slim_interned_parent = NULL;
zend_string *slim_interned_router = NULL;
zend_string *slim_interned_request						= NULL;
zend_string *slim_interned_response = NULL;
zend_string *slim_interned_php							= NULL;

void slim_init_interned_strings()
{
    slim_interned_DELETE							= SSL("DELETE");
    slim_interned_GET							= SSL("GET");
    slim_interned_HEAD							= SSL("HEAD");
    slim_interned_OPTIONS						= SSL("OPTIONS");
    slim_interned_PATCH							= SSL("PATCH");
    slim_interned_POST							= SSL("POST");
    slim_interned_PUT							= SSL("PUT");
    slim_interned_static = SSL("static");
    slim_interned_self = SSL("self");
    slim_interned_parent = SSL("parent");
    slim_interned_router = SSL("router");
    slim_interned_request						= SSL("request");
    slim_interned_response = SSL("response");
    slim_interned_php							= SSL("php");
}


void slim_release_interned_strings()
{
    zend_string_release(slim_interned_DELETE);
    zend_string_release(slim_interned_GET);
    zend_string_release(slim_interned_HEAD);
    zend_string_release(slim_interned_OPTIONS);
    zend_string_release(slim_interned_PATCH);
    zend_string_release(slim_interned_POST);
    zend_string_release(slim_interned_PUT);
    zend_string_release(slim_interned_static);
    zend_string_release(slim_interned_self);
    zend_string_release(slim_interned_parent);
    zend_string_release(slim_interned_router);
    zend_string_release(slim_interned_request);
    zend_string_release(slim_interned_response);
    zend_string_release(slim_interned_php);
}
