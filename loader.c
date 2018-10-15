#include "loader.h"
#include "kernel/main.h"
#include "kernel/array.h"
#include "kernel/object.h"
#include "kernel/file.h"
#include "kernel/operators.h"
#include "kernel/fcall.h"
#include "kernel/string.h"
#include "kernel/concat.h"
#include "kernel/require.h"

#include "interned-strings.h"

zend_class_entry *slim_loader_ce;

PHP_METHOD(Slim_Loader, __construct);
PHP_METHOD(Slim_Loader, registerNamespaces);
PHP_METHOD(Slim_Loader, getNamespaces);
PHP_METHOD(Slim_Loader, register);
PHP_METHOD(Slim_Loader, autoLoad);
PHP_METHOD(Slim_Loader, findFile);

static const zend_function_entry slim_loader_method_entry[] = {
    PHP_ME(Slim_Loader, __construct, NULL, ZEND_ACC_PUBLIC|ZEND_ACC_CTOR)
    PHP_ME(Slim_Loader, registerNamespaces, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(Slim_Loader, getNamespaces, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(Slim_Loader, register, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(Slim_Loader, autoLoad, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(Slim_Loader, findFile, NULL, ZEND_ACC_PUBLIC)
    PHP_FE_END
};

SLIM_INIT_CLASS(Slim_Loader)
{
    SLIM_REGISTER_CLASS(Slim, Loader, loader, slim_loader_method_entry, 0);

    zend_declare_property_null(slim_loader_ce, SL("_default"), ZEND_ACC_PROTECTED|ZEND_ACC_STATIC);
    zend_declare_property_null(slim_loader_ce, SL("_foundPath"), ZEND_ACC_PROTECTED);
    zend_declare_property_null(slim_loader_ce, SL("_checkedPath"), ZEND_ACC_PROTECTED);
    zend_declare_property_null(slim_loader_ce, SL("_prefixes"), ZEND_ACC_PROTECTED);
    zend_declare_property_null(slim_loader_ce, SL("_classes"), ZEND_ACC_PROTECTED);
    zend_declare_property_null(slim_loader_ce, SL("_extensions"), ZEND_ACC_PROTECTED);
    zend_declare_property_null(slim_loader_ce, SL("_namespaces"), ZEND_ACC_PROTECTED);
    zend_declare_property_null(slim_loader_ce, SL("_directories"), ZEND_ACC_PROTECTED);
    zend_declare_property_bool(slim_loader_ce, SL("_registered"), 0, ZEND_ACC_PROTECTED);

    return SUCCESS;
}

PHP_METHOD(Slim_Loader, __construct)
{
    zval extensions = {}, default_loader = {};

    array_init_size(&extensions, 1);
    slim_array_append_string(&extensions, IS(php), PH_COPY);

    slim_update_property(getThis(), SL("_extensions"), &extensions);
    zval_ptr_dtor(&extensions);

    slim_read_static_property_ce(&default_loader, slim_loader_ce, SL("_default"), PH_READONLY);
    if (Z_TYPE(default_loader) == IS_NULL) {
        slim_update_static_property_ce(slim_loader_ce, SL("_default"), getThis());
    }
}

PHP_METHOD(Slim_Loader, registerNamespaces)
{
    zval *namespaces, *merge = NULL, current_namespaces = {}, merged_namespaces = {};

    slim_fetch_params(0, 1, 1, &namespaces, &merge);

    if (merge && zend_is_true(merge)) {
        slim_read_property(&current_namespaces, getThis(), SL("_namespaces"), PH_NOISY|PH_READONLY);
        if (Z_TYPE(current_namespaces) == IS_ARRAY) {
            slim_fast_array_merge(&merged_namespaces, &current_namespaces, namespaces);
        } else {
            ZVAL_COPY_VALUE(&merged_namespaces, namespaces);
        }

        slim_update_property(getThis(), SL("_namespaces"), &merged_namespaces);
    } else {
        slim_update_property(getThis(), SL("_namespaces"), namespaces);
    }

    RETURN_THIS();
}

PHP_METHOD(Slim_Loader, getNamespaces)
{
    RETURN_MEMBER(getThis(), "_namespaces");
}

PHP_METHOD(Slim_Loader, register)
{
    zval registered = {}, autoloader = {};

    slim_read_property(&registered, getThis(), SL("_registered"), PH_NOISY|PH_READONLY);
    if (SLIM_IS_FALSE(&registered)) {
        array_init_size(&autoloader, 2);
        slim_array_append(&autoloader, getThis(), PH_COPY);
        slim_array_append_string(&autoloader, IS(autoLoad), PH_COPY);
        SLIM_CALL_FUNCTION(NULL, "spl_autoload_register", &autoloader);
        zval_ptr_dtor(&autoloader);

        slim_update_property(getThis(), SL("_registered"), &SLIM_G(z_true));
    }

    RETURN_THIS();
}

PHP_METHOD(Slim_Loader, autoLoad)
{
    zval *class_name, found = {}, file_path = {}, ds = {}, namespace_separator = {};
    zval extensions = {}, *directory, directories = {}, pseudo_separator;
    zend_string *str_key;
    ulong idx;
    char slash[2] = {DEFAULT_SLASH, 0};

    slim_fetch_params(0, 1, 0, &class_name);

    ZVAL_FALSE(&found);

    ZVAL_STRING(&ds,slash);
    ZVAL_STRING(&namespace_separator, "\\");
    ZVAL_STRING(&pseudo_separator, "_");
    slim_read_property(&extensions, getThis(), SL("_extensions"), PH_NOISY|PH_READONLY);

    if (!zend_is_true(&found)) {
        zval namespaces = {};

        slim_read_property(&namespaces, getThis(), SL("_namespaces"), PH_READONLY);
        if (Z_TYPE(namespaces) == IS_ARRAY) {
            // foreach start
            ZEND_HASH_FOREACH_KEY_VAL(Z_ARRVAL(namespaces), idx, str_key, directory) {
                zval ns_prefix = {}, ns_prefixed = {}, file_name = {};
                if (str_key) {
                    ZVAL_STR(&ns_prefix, str_key);
                } else {
                    ZVAL_LONG(&ns_prefix, idx);
                }
                /**
                 * The class name must start with the current namespace
                 */
                SLIM_CONCAT_VV(&ns_prefixed, &ns_prefix, &namespace_separator);
                if (!slim_start_with(class_name, &ns_prefixed, NULL)) {
                    continue;
                }

                /**
                 * Get the possible file path
                 */
                slim_possible_autoload_filepath(&file_name, &ns_prefix, class_name, &ds, NULL);
                if (zend_is_true(&file_name)) {
                    SLIM_CALL_METHOD(&found, getThis(), "findfile", &file_name, directory, &extensions, &ds);

                    if (zend_is_true(&found)) {
                        break;
                    }
                }

                if (slim_memnstr_str(class_name, SL("_"))) {
                    slim_possible_autoload_filepath(&file_name, &ns_prefix, class_name, &ds, &pseudo_separator);
                    if (zend_is_true(&file_name)) {
                        SLIM_CALL_METHOD(&found, getThis(), "findfile", &file_name, directory, &extensions, &ds);

                        if (zend_is_true(&found)) {
                            break;
                        }
                    }
                }

            } ZEND_HASH_FOREACH_END();
            // foreach end
        }
    }

    if (zend_is_true(&found)) {
        RETURN_TRUE;
    }

    RETURN_FALSE;
}

PHP_METHOD(Slim_Loader, findFile)
{
    zval *class_name, *directory, *extensions, *ds = NULL, ds_slash = {}, directories = {}, *dir, *extension;
    char slash[2] = {DEFAULT_SLASH, 2};

    slim_fetch_params(0, 3, 1, &class_name, &directory, &extensions, &ds);

    if (Z_TYPE_P(directory) != IS_ARRAY) {
        array_init(&directories);
        slim_array_append(&directories, directory, PH_COPY);
    } else {
        ZVAL_COPY_VALUE(&directories, directory);
    }


    if (!ds) {
        ZVAL_STRING(&ds_slash, slash);
    } else {
        ZVAL_COPY_VALUE(&ds_slash, ds);
    }

    ZEND_HASH_FOREACH_VAL(Z_ARRVAL(directories), dir) {
        zval fixed_dir = {};
        if (Z_TYPE_P(dir) != IS_STRING) {
            convert_to_string_ex(dir);
        }
        /**
         * Add a trailing directory separator if the user forgot to do that
         */
        slim_fix_path(&fixed_dir, dir, &ds_slash);
        ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(extensions), extension) {
            zval file_path = {};
            SLIM_CONCAT_VVSV(&file_path, &fixed_dir, class_name, ".", extension);
            /**
             * This is probably a good path, let's check if the file exist
             */
            if (slim_file_exists(&file_path) == SUCCESS) {
                /**
                 * Simulate a require
                 */
                assert(Z_TYPE(file_path) == IS_STRING);
                RETURN_ON_FAILURE(slim_require(Z_STRVAL(file_path)));

                /**
                 * Return true mean success
                 */
                RETVAL_TRUE;
                break;
            }
        } ZEND_HASH_FOREACH_END();

        if (zend_is_true(return_value)) {
            break;
        }
    } ZEND_HASH_FOREACH_END();

    return;
}
