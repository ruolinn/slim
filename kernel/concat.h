#ifndef SLIM_KERNEL_CONCAT_H
#define SLIM_KERNEL_CONCAT_H

#include "php_slim.h"

#define SLIM_CONCAT_SV(result, op1, op2)                 \
    slim_concat_sv(result, op1, sizeof(op1)-1, op2, 0);
#define SLIM_CONCAT_VS(result, op1, op2)                 \
    slim_concat_vs(result, op1, op2, sizeof(op2)-1, 0);
#define SLIM_CONCAT_VSV(result, op1, op2, op3)                 \
    slim_concat_vsv(result, op1, op2, sizeof(op2)-1, op3, 0);
#define SLIM_CONCAT_SVS(result, op1, op2, op3)                       \
    slim_concat_svs(result, op1, sizeof(op1)-1, op2, op3, sizeof(op3)-1, 0);
#define SLIM_CONCAT_SVSV(result, op1, op2, op3, op4)                 \
    slim_concat_svsv(result, op1, sizeof(op1)-1, op2, op3, sizeof(op3)-1, op4, 0);

void slim_concat_vs(zval *result, zval *op1, const char *op2, uint32_t op2_len, int self_var);
void slim_concat_sv(zval *result, const char *op1, uint32_t op1_len, zval *op2, int self_var);
void slim_concat_vsv(zval *result, zval *op1, const char *op2, uint32_t op2_len, zval *op3, int self_var);
void slim_concat_svs(zval *result, const char *op1, uint32_t op1_len, zval *op2, const char *op3, uint32_t op3_len, int self_var);
void slim_concat_svsv(zval *result, const char *op1, uint32_t op1_len, zval *op2, const char *op3, uint32_t op3_len, zval *op4, int self_var);

#endif
