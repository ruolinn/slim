#include "kernel/debug.h"

FILE *slim_log = NULL;

int slim_start_debug()
{
    if(!slim_log){
        slim_log = stderr;
    }
    return SUCCESS;
}

int slim_dump(zval *uservar){
    slim_start_debug();
    if(!uservar){
        fprintf(slim_log, "Null pointer\n");
        return SUCCESS;
    }

    switch(Z_TYPE_P(uservar)){
    case IS_NULL:
        fprintf(slim_log, "NULL \n");
        break;
    case IS_TRUE:
        fprintf(slim_log, "Boolean: %s\n", "TRUE");
    case IS_FALSE:
        fprintf(slim_log, "Boolean: %s\n", "FALSE");
        break;
    case IS_LONG:
        fprintf(slim_log, "Long: %ld at %p, refcount=%d\n", (long)Z_LVAL_P(uservar), uservar, Z_REFCOUNT_P(uservar));
        break;
    case IS_DOUBLE:
        fprintf(slim_log, "Double: %f\n", Z_DVAL_P(uservar));
        break;
    case IS_STRING:
        fprintf(slim_log, "String: %s(%zu) at %p, refcount=%d\n", Z_STRVAL_P(uservar), Z_STRLEN_P(uservar), uservar, Z_REFCOUNT_P(uservar));
        break;
    case IS_RESOURCE:
        fprintf(slim_log, "Resource\n");
        break;
    case IS_ARRAY:
        fprintf(slim_log, "Array at %p, refcount=%d\n", uservar, Z_REFCOUNT_P(uservar));
        break;
    case IS_OBJECT:
        fprintf(slim_log, "Object <%s> at %p\n", Z_OBJCE_P(uservar)->name->val, uservar);
        break;
    default:
        fprintf(slim_log, "Unknown\n");
    }
    return SUCCESS;
}
