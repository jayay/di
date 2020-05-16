/* di extension for PHP (c) 2020 Jakob Jungmann */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "php.h"
#include "ext/standard/info.h"
#include "zend_interfaces.h"
#include "php_di.h"

#include "zend.h"
#include "zend_API.h"
#include "zend_constants.h"


/* For compatibility with older PHP versions */
#ifndef ZEND_PARSE_PARAMETERS_NONE
#define ZEND_PARSE_PARAMETERS_NONE() \
    ZEND_PARSE_PARAMETERS_START(0, 0) \
    ZEND_PARSE_PARAMETERS_END()
#endif

static zend_class_entry *find_class_entry_by_mapping_name(zend_string *class_name, zval *this_ptr);
static zend_string *find_class_name_by_mapping_name(zend_string *class_name, zval *this_ptr);
static int resolve_build_dependencies(zend_string* class, uint32_t nesting_limit, zval *this_ptr, zval* retval);
static int build_instance(zend_class_entry *ce, zval *this_ptr, zval *new_obj);

zend_class_entry *di_ce_interface, *di_ce_container;

static zend_object_handlers di_object_handlers_di_container;

ZEND_BEGIN_ARG_INFO_EX(arginfo_di_container_method_construct, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_di_method_get, 0, 0, 1)
    ZEND_ARG_TYPE_INFO(0, class_name, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_di_method_withInstances, 0, 0, 1)
    ZEND_ARG_ARRAY_INFO(0, instances, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_di_method_withClassMap, 0, 0, 1)
    ZEND_ARG_ARRAY_INFO(0, classmap, 0)
ZEND_END_ARG_INFO()

static const zend_function_entry di_container_interface[] = {
    PHP_ABSTRACT_ME(DIContainerInterface, get, arginfo_di_method_get)
    PHP_ABSTRACT_ME(DIContainerInterface, withInstances, arginfo_di_method_withInstances)
    PHP_ABSTRACT_ME(DIContainerInterface, withClassMap, arginfo_di_method_withClassMap)
    PHP_FE_END
};

static HashTable *get_default_entries_classmap()
{
    HashTable *ht;
    ALLOC_HASHTABLE(ht);
    zend_hash_init(ht, 1, NULL, ZVAL_PTR_DTOR, 0);
    zval nameContainerZval;
    zend_string *nameContainerInterface;
    ZVAL_NEW_STR(&nameContainerZval, zend_string_init("DIContainer", strlen("DIContainer"), 0));
    nameContainerInterface = zend_string_init("DIContainerInterface", strlen("DIContainerInterface"), 0);
    zend_hash_add(ht, nameContainerInterface, &nameContainerZval);
    zend_string_release(nameContainerInterface);
    return ht;
}

static HashTable *get_default_entries_instances(zval *this_ptr)
{
    zval nameContainerZval;
    HashTable *ht;
    ALLOC_HASHTABLE(ht);
    zend_hash_init(ht, 1, NULL, ZVAL_PTR_DTOR, 0);
    Z_STR(nameContainerZval) = zend_string_init("DIContainer", strlen("DIContainer"), 0);
    zend_hash_add(ht, Z_STR(nameContainerZval), this_ptr);
    return ht;
}

PHP_METHOD(DIContainer, __construct)
{
    php_di_obj *php_di_obj;
    php_di_obj = Z_PHPDI_P(getThis());

    php_di_obj->classmap = get_default_entries_classmap();
    php_di_obj->instances = get_default_entries_instances(getThis());
}

PHP_METHOD(DIContainer, get)
{
    zend_string* cf;
    int status;

    ZEND_PARSE_PARAMETERS_START(1,1)
            Z_PARAM_STR(cf)
    ZEND_PARSE_PARAMETERS_END();


    status = resolve_build_dependencies(cf,
                                        100, getThis(), return_value);

    if (status != SUCCESS) {
        RETURN_LONG(status);
    }

    if (return_value == NULL) {
        RETURN_LONG(-11111);
    }
}

static int resolve_build_dependencies(
    zend_string* class,
    uint32_t nesting_limit,
    zval *this_ptr,
    zval* retval)
{
    zend_class_entry *ce, *sub_entry;
    zend_type type;
    php_di_obj *php_di_obj;
    zval *find_res_tmp, *find_res, tmp, retval_o;
    uint32_t req_num_args, i;
    int build_result, sub_result;

    if (nesting_limit == 0) {
        return -3;
    }

    if ((ce = find_class_entry_by_mapping_name(class, this_ptr)) == NULL) {
        return -20;
        //zend_throw_exception_ex(reflection_exception_ptr, 0,
        //		"Class %s does not exist", ZSTR_VAL(name));
        //zend_string_release(name); // do again after throw
        //RETURN_THROWS();
    }

    if (ce->constructor) {
        if (ce->constructor->common.fn_flags & ZEND_ACC_ABSTRACT) {
            // throw exception (method is abstract)
            return 2;
        }

        if (!(ce->constructor->common.fn_flags & ZEND_ACC_PUBLIC)) {
            // throw exception (constructor is private)
            return 3;
        }

    }

    php_di_obj = Z_PHPDI_P(this_ptr);

    if ((find_res_tmp = zend_hash_find(php_di_obj->instances, ce->name)) != NULL) {
        if (Z_TYPE_P(find_res_tmp) == IS_OBJECT) {
            ZVAL_COPY(retval, find_res_tmp);
            return SUCCESS;
        }
    }

    if (ce->constructor) {
        req_num_args = ce->constructor->internal_function.required_num_args;
        for (i = 0; i < req_num_args; i++) {
            type = ce->constructor->internal_function.arg_info[i].type;
#if PHP_MAJOR_VERSION < 8
            if (!ZEND_TYPE_IS_CLASS(type)) {
                return -1;
            }
#endif

            if ((sub_entry = find_class_entry_by_mapping_name(ZEND_TYPE_NAME(type), this_ptr)) == NULL) {
                return -2;
            }

            find_res = zend_hash_find(php_di_obj->instances, sub_entry->name);
            if (find_res == NULL) {
                zend_hash_add_empty_element(php_di_obj->instances, sub_entry->name);

                if ((sub_result = resolve_build_dependencies(
                        sub_entry->name, nesting_limit - 1, this_ptr, &tmp)) < 0) {
                    return sub_result;
                }
            }
        }
    }

    if (UNEXPECTED(object_init_ex(&retval_o, ce) != SUCCESS)) {
        return -4;
    }

    build_result = SUCCESS;

    if (ce->constructor) {
        if (!(ce->constructor->internal_function.fn_flags & ZEND_ACC_PUBLIC)) {
            zval_ptr_dtor(&retval_o);
            return -9;
        }
        build_result = build_instance(ce, this_ptr, &retval_o);
    }

    if (build_result == SUCCESS) {
        zend_hash_update(php_di_obj->instances, ce->name, &retval_o);
        ZVAL_COPY(retval, &retval_o);
    }

    return build_result;
}


static int build_instance(zend_class_entry *ce, zval *this_ptr, zval *new_obj)
{
    zval retval;
    zval *params;
    int ret, i, num_args;
    zend_fcall_info fci;
    zend_fcall_info_cache fcc;
    zend_string* dependency_str;
    php_di_obj *php_di_obj;

    num_args = ce->constructor->internal_function.required_num_args;

    params = emalloc(num_args * sizeof(zval));
    memset(params, 0, num_args * sizeof(zval));

    php_di_obj = Z_PHPDI_P(this_ptr);

    for (i = 0; i < num_args; i++) {
        dependency_str = find_class_name_by_mapping_name(
                ZEND_TYPE_NAME(ce->constructor->internal_function.arg_info[i].type),
                this_ptr);
        zval *zval_result;

        if ((zval_result = zend_hash_find(php_di_obj->instances, dependency_str)) == NULL) {
            return -12;
        }
        ZVAL_COPY(&(params[i]), zval_result);
        Z_TRY_ADDREF(params[i]);
    }

    fci.size = sizeof(fci);
    ZVAL_UNDEF(&fci.function_name);
    fci.object = Z_OBJ_P(new_obj);
    fci.retval = &retval;
    fci.param_count = num_args;
    fci.params = params;
    fci.no_separation = 1;

    fcc.function_handler = ce->constructor;
    fcc.called_scope = Z_OBJCE_P(new_obj);
    fcc.object = Z_OBJ_P(new_obj);

    ret = zend_call_function(&fci, &fcc);
    zval_ptr_dtor(&retval);
    for (i = 0; i < num_args; i++) {
        zval_ptr_dtor(&params[i]);
    }

    if (EG(exception)) {
        zend_object_store_ctor_failed(Z_OBJ_P(new_obj));
    }

    efree(params);

    if (ret == FAILURE) {
        zval_ptr_dtor(&retval);
        php_error_docref(NULL, E_WARNING, "Invocation of %s's constructor failed", ZSTR_VAL(ce->name));
        zval_ptr_dtor(new_obj);
        return -1;
    }

    return SUCCESS;
}

/* Advanced Interface */
PHPAPI zval *php_di_instantiate(zend_class_entry *ce, zval *object) /* {{{ */
{
    object_init_ex(object, ce);
    return object;
} /* }}} */

static zend_string *find_class_name_by_mapping_name(zend_string *class_name, zval *this_ptr)
{
    php_di_obj *php_di_obj;
    zval *hash_result;

    php_di_obj = Z_PHPDI_P(this_ptr);

    while (1) {
        hash_result = zend_hash_find(php_di_obj->classmap, class_name);
        if (hash_result == NULL) {
            break;
        }
        class_name = Z_STR_P(hash_result);
    }
    return class_name;
}

static zend_class_entry* find_class_entry_by_mapping_name(zend_string *class_name, zval *this_ptr)
{
    zend_class_entry *result;
    class_name = find_class_name_by_mapping_name(class_name, this_ptr);
    result = zend_lookup_class(class_name);
    return result;
}


PHP_METHOD(DIContainer, withInstances)
{

}

PHP_METHOD(DIContainer, withClassMap)
{
    php_di_obj *new_obj;
    HashTable *classmap, *default_classmap, *default_instances;

    php_di_instantiate(di_ce_container, return_value);
    new_obj = Z_PHPDI_P(return_value);

    ZEND_PARSE_PARAMETERS_START(1,1)
            Z_PARAM_ARRAY_HT((classmap))
    ZEND_PARSE_PARAMETERS_END();


    default_classmap = get_default_entries_classmap();
    default_instances = get_default_entries_instances(getThis());

    zend_hash_merge(default_classmap, classmap, zval_add_ref, 1);
    new_obj->classmap = default_classmap;
    new_obj->instances = default_instances;
}

static const zend_function_entry di_container_impl[] = {
    PHP_ME(DIContainer,			__construct,		arginfo_di_container_method_construct, ZEND_ACC_PUBLIC)
    PHP_ME(DIContainer, get,	arginfo_di_method_get, ZEND_ACC_PUBLIC)
    PHP_ME(DIContainer, withInstances,	arginfo_di_method_withInstances, ZEND_ACC_PUBLIC)
    PHP_ME(DIContainer, withClassMap,	arginfo_di_method_withClassMap, ZEND_ACC_PUBLIC)
    PHP_FE_END
};


/* {{{ void di_test1()
 */
PHP_FUNCTION(di_test1)
{
    ZEND_PARSE_PARAMETERS_NONE();

    php_printf("The extension %s is loaded and working!\r\n", "di");
}
/* }}} */

/* {{{ string di_test2( [ string $var ] )
 */
PHP_FUNCTION(di_test2)
{
    char *var = "World";
    size_t var_len = sizeof("World") - 1;
    zend_string *retval;

    ZEND_PARSE_PARAMETERS_START(0, 1)
            Z_PARAM_OPTIONAL
            Z_PARAM_STRING(var, var_len)
    ZEND_PARSE_PARAMETERS_END();

    retval = strpprintf(0, "Hello %s", var);

    RETURN_STR(retval);
}
/* }}}*/

/* {{{ PHP_RINIT_FUNCTION
 */
PHP_RINIT_FUNCTION(di)
{
#if defined(ZTS) && defined(COMPILE_DL_DI)
    ZEND_TSRMLS_CACHE_UPDATE();
#endif

    return SUCCESS;
}
/* }}} */

static zend_object *di_object_new_di(zend_class_entry *class_type) /* {{{ */
{
    php_di_obj *intern = zend_object_alloc(sizeof(php_di_obj), class_type);

    zend_object_std_init(&intern->std, class_type);
    object_properties_init(&intern->std, class_type);
    intern->std.handlers = &di_object_handlers_di_container;

    return &intern->std;
} /* }}} */

#if PHP_MAJOR_VERSION < 8
static zend_object *di_object_clone_di(zval *this_ptr) /* {{{ */
{
    php_di_obj *old_obj = Z_PHPDI_P(this_ptr);
    php_di_obj *new_obj = php_di_obj_from_obj(di_object_new_di(old_obj->std.ce));

    zend_objects_clone_members(&new_obj->std, &old_obj->std);

    return &new_obj->std;
}
#else
static zend_object *di_object_clone_di(zend_object *this_ptr) /* {{{ */
{
    php_di_obj *old_obj = php_di_obj_from_obj(this_ptr);
    php_di_obj *new_obj = php_di_obj_from_obj(di_object_new_di(old_obj->std.ce));

    zend_objects_clone_members(&new_obj->std, &old_obj->std);

    return &new_obj->std;
} /* }}} */
#endif

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(di)
{
    zend_class_entry ce_container, ce_interface;

    INIT_CLASS_ENTRY(ce_interface, "DIContainerInterface", di_container_interface);
    INIT_CLASS_ENTRY(ce_container, "DIContainer", di_container_impl);

    di_ce_interface = zend_register_internal_interface(&ce_interface);
    di_ce_container = zend_register_internal_class(&ce_container);
    zend_class_implements(di_ce_container, 1, di_ce_interface);
    di_ce_container->create_object = di_object_new_di;
    memcpy(&di_object_handlers_di_container, &std_object_handlers, sizeof(zend_object_handlers));
    di_object_handlers_di_container.offset = XtOffsetOf(php_di_obj , std);
    di_object_handlers_di_container.clone_obj = di_object_clone_di;

    return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(di)
{
    php_info_print_table_start();
    php_info_print_table_header(2, "di support", "enabled");
    php_info_print_table_end();
}
/* }}} */

/* {{{ arginfo
 */
ZEND_BEGIN_ARG_INFO(arginfo_di_test1, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_di_test2, 0)
    ZEND_ARG_INFO(0, str)
ZEND_END_ARG_INFO()
/* }}} */

/* {{{ di_functions[]
 */
static const zend_function_entry di_functions[] = {
    PHP_FE(di_test1,		arginfo_di_test1)
    PHP_FE(di_test2,		arginfo_di_test2)
    PHP_FE_END
};
/* }}} */

/* {{{ di_module_entry
 */
zend_module_entry di_module_entry = {
    STANDARD_MODULE_HEADER,
    "di",					/* Extension name */
    di_functions,			/* zend_function_entry */
    PHP_MINIT(di),			/* PHP_MINIT - Module initialization */
    NULL,					/* PHP_MSHUTDOWN - Module shutdown */
    PHP_RINIT(di),			/* PHP_RINIT - Request initialization */
    NULL,					/* PHP_RSHUTDOWN - Request shutdown */
    PHP_MINFO(di),			/* PHP_MINFO - Module info */
    PHP_DI_VERSION,			/* Version */
    STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_DI
# ifdef ZTS
ZEND_TSRMLS_CACHE_DEFINE()
# endif
ZEND_GET_MODULE(di)
#endif
