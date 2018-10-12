#include "php_slim.h"
#include "kernel/framework/router.h"

#include <Zend/zend_smart_str.h>
#include <ext/standard/php_string.h>

void slim_extract_named_params(zval *return_value, zval *str, zval *matches)
{
	zval tmp = {};
	int i, k;
	uint j, bracket_count = 0, parentheses_count = 0, ch;
	uint intermediate = 0, length, number_matches = 0, found_pattern;
	int variable_length, regexp_length = 0, not_valid = 0;
	char *cursor, *cursor_var, *marker = NULL;
	char *item, *variable = NULL, *regexp = NULL;
	smart_str route_str = {0};

	if (Z_TYPE_P(str) != IS_STRING || Z_STRLEN_P(str) <= 0 || Z_TYPE_P(matches) != IS_ARRAY) {
		ZVAL_FALSE(return_value);
		return;
	}

	cursor = Z_STRVAL_P(str);
	for (i = 0; i < Z_STRLEN_P(str); i++) {

		ch = *cursor;
		if (ch == '\0') {
			break;
		}

		if (parentheses_count == 0) {
			if (ch == '{') {
				if (bracket_count == 0) {
					marker = cursor;
					intermediate = 0;
					not_valid = 0;
				}
				bracket_count++;
			} else {
				if (ch == '}') {
					bracket_count--;
					if (intermediate > 0) {
						if (bracket_count == 0) {

							number_matches++;

							variable = NULL;
							length = cursor - marker - 1;
							item = estrndup(marker + 1, length);
							cursor_var = item;
							marker = item;
							for (j = 0; j < length; j++) {
								ch = *cursor_var;
								if (ch == '\0') {
									break;
								}
								if (j == 0 && !((ch >= 'a' && ch <='z') || (ch >= 'A' && ch <='Z'))){
									not_valid = 1;
									break;
								}
								if ((ch >= 'a' && ch <='z') || (ch >= 'A' && ch <='Z') || (ch >= '0' && ch <='9') || ch == '-' || ch == '_' || ch ==  ':') {
									if (ch == ':') {
										regexp_length = length - j - 1;
										variable_length = cursor_var - marker;
										variable = estrndup(marker, variable_length);
										regexp = estrndup(cursor_var + 1, regexp_length);
										break;
									}
								} else {
									not_valid = 1;
									break;
								}
								cursor_var++;
							}

							if (!not_valid) {
								ZVAL_LONG(&tmp, number_matches);

								if (variable) {
									if (regexp_length > 0) {
										ASSUME(regexp != NULL);

										/**
										 * Check if we need to add parentheses to the expression
										 */
										found_pattern = 0;
										for (k = 0; k < regexp_length; k++) {
											if (regexp[k] == '\0') {
												break;
											}
											if (!found_pattern) {
												if (regexp[k] == '(') {
													found_pattern = 1;
												}
											} else {
												if (regexp[k] == ')') {
													found_pattern = 2;
													break;
												}
											}
										}

										if (found_pattern != 2) {
											smart_str_appendc(&route_str, '(');
											smart_str_appendl(&route_str, regexp, regexp_length);
											smart_str_appendc(&route_str, ')');
										} else {
											smart_str_appendl(&route_str, regexp, regexp_length);
										}
										zend_hash_str_update(Z_ARRVAL_P(matches), variable, variable_length, &tmp);
									}
									efree(regexp);
									efree(variable);
								} else {
									smart_str_appendl(&route_str, "([^/]*)", strlen("([^/]*)"));
									zend_hash_str_update(Z_ARRVAL_P(matches), item, length, &tmp);
								}
							} else {
								smart_str_appendc(&route_str, '{');
								smart_str_appendl(&route_str, item, length);
								smart_str_appendc(&route_str, '}');
							}

							efree(item);

							cursor++;
							continue;
						}
					}
				}
			}

		}

		if (bracket_count == 0) {
			if (ch == '(') {
				parentheses_count++;
			} else {
				if (ch == ')') {
					parentheses_count--;
					if (parentheses_count == 0) {
						number_matches++;
					}
				}
			}
		}

		if (bracket_count > 0) {
			intermediate++;
		} else {
			smart_str_appendc(&route_str, ch);
		}

		cursor++;
	}
	smart_str_0(&route_str);

	if (route_str.s) {
		RETURN_STR(route_str.s);
	} else {
		smart_str_free(&route_str);
		RETURN_EMPTY_STRING();
	}

}
