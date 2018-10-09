#ifndef SLIM_INTERNED_STRINGS_H
#define SLIM_INTERNED_STRINGS_H

extern zend_string *slim_interned_static;
extern zend_string *slim_interned_self;
extern zend_string *slim_interned_parent;
extern zend_string *slim_interned_router;
extern zend_string *slim_interned_response;

void slim_init_interned_strings();
void slim_release_interned_strings();

#endif
