#ifndef SLIM_INTERNED_STRINGS_H
#define SLIM_INTERNED_STRINGS_H

extern zend_string *slim_interned_DELETE;
extern zend_string *slim_interned_GET;
extern zend_string *slim_interned_HEAD;
extern zend_string *slim_interned_OPTIONS;
extern zend_string *slim_interned_PATCH;
extern zend_string *slim_interned_POST;
extern zend_string *slim_interned_PUT;
extern zend_string *slim_interned_static;
extern zend_string *slim_interned_self;
extern zend_string *slim_interned_parent;
extern zend_string *slim_interned_router;
extern zend_string *slim_interned_request;
extern zend_string *slim_interned_response;
extern zend_string *slim_interned_php;
extern zend_string *slim_interned_autoLoad;
extern zend_string *slim_interned_events;

void slim_init_interned_strings();
void slim_release_interned_strings();

#endif
