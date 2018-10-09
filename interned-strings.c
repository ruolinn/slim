#include "php_slim.h"
#include "interned-strings.h"
#include "kernel/main.h"
#include "kernel/string.h"

zend_string *slim_interned_static = NULL;
zend_string *slim_interned_self = NULL;
zend_string *slim_interned_parent = NULL;
zend_string *slim_interned_router = NULL;
zend_string *slim_interned_response = NULL;

void slim_init_interned_strings()
{
    slim_interned_static = SSL("static");
    slim_interned_self = SSL("self");
    slim_interned_parent = SSL("parent");
    slim_interned_router = SSL("router");
    slim_interned_response = SSL("response");
}


void slim_release_interned_strings()
{
    zend_string_release(slim_interned_static);
    zend_string_release(slim_interned_self);
    zend_string_release(slim_interned_parent);
    zend_string_release(slim_interned_router);
    zend_string_release(slim_interned_response);
}
