#include "php_slim.h"
#include "interned-strings.h"
#include "kernel/main.h"
#include "kernel/string.h"

zend_string *slim_interned_static = NULL;
zend_string *slim_interned_self = NULL;
zend_string *slim_interned_parent = NULL;

void slim_init_interned_strings()
{
    slim_interned_static = SSL("static");
    slim_interned_self = SSL("self");
    slim_interned_parent("parent");
}


void slim_release_interned_strings()
{
    zend_string_release(slim_interned_static);
    zend_string_release(slim_interned_self);
    zend_string_release(slim_interned_parent);
}
