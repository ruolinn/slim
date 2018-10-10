#include "kernel/concat.h"
#include "kernel/string.h"

void slim_concat_sv(zval *result, const char *op1, uint32_t op1_len, zval *op2, int self_var){
    smart_str implstr = {0};

    if (self_var) {
        slim_append_printable_zval(&implstr, result);
    }
    slim_smart_str_appendl(&implstr, op1, op1_len);
    slim_append_printable_zval(&implstr, op2);
    smart_str_0(&implstr);
    if (implstr.s) {
        ZVAL_NEW_STR(result, implstr.s);
    } else {
        smart_str_free(&implstr);
        ZVAL_NULL(result);
    }
}

void slim_concat_vsv(zval *result, zval *op1, const char *op2, uint32_t op2_len, zval *op3, int self_var){
    smart_str implstr = {0};

    if (self_var) {
        slim_append_printable_zval(&implstr, result);
    }
    slim_append_printable_zval(&implstr, op1);
    slim_smart_str_appendl(&implstr, op2, op2_len);
    slim_append_printable_zval(&implstr, op3);
    smart_str_0(&implstr);
    if (implstr.s) {
        ZVAL_NEW_STR(result, implstr.s);
    } else {
        smart_str_free(&implstr);
        ZVAL_NULL(result);
    }
}
