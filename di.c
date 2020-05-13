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

static const zend_function_entry di_container_interface[] = {
	PHP_ABSTRACT_ME(DIContainerInterface, get, arginfo_di_method_get)
	PHP_ABSTRACT_ME(DIContainerInterface, withInstances, arginfo_di_method_withInstances)
	PHP_FE_END
};

PHP_METHOD(DIContainer, __construct)
{
	php_di_obj *php_di_obj;
    php_di_obj = Z_PHPDI_P(getThis());
	ALLOC_HASHTABLE(php_di_obj->instances); // TODO: destroy & free
    zend_hash_init(php_di_obj->instances, 10, NULL, ZVAL_PTR_DTOR, 0);
}

PHP_METHOD(DIContainer, get)
{
	zend_class_entry *class_subject;
	zend_string* cf;
    php_di_obj* php_di_obj;
	int status;

	ZEND_PARSE_PARAMETERS_START(1,1)
		Z_PARAM_STR(cf)
	ZEND_PARSE_PARAMETERS_END();

	if ((class_subject = zend_lookup_class(cf)) == NULL) {
		RETURN_LONG(-20);
		//zend_throw_exception_ex(reflection_exception_ptr, 0,
		//		"Class %s does not exist", ZSTR_VAL(name));
		//zend_string_release(name); // do again after throw
		//RETURN_THROWS();
	}

	if (class_subject->constructor) {
		if (class_subject->constructor->common.fn_flags & ZEND_ACC_ABSTRACT) {
			// throw exception (method is abstract)
			RETURN_LONG(2);
		}

		if (!(class_subject->constructor->common.fn_flags & ZEND_ACC_PUBLIC)) {
			// throw exception (constructor is private)
			RETURN_LONG(3);
		}

	}

    status = resolve_build_dependencies(class_subject, 100, getThis(), return_value);

	if (status != 0) {
	    RETURN_LONG(status);
	}

    php_di_obj = Z_PHPDI_P(getThis());

    if (return_value == NULL) {
        RETURN_LONG(-11111);
    }
}

static int resolve_build_dependencies(zend_class_entry* ce, uint32_t nesting_limit, zval *this_ptr, zval* retval)
{
	zend_class_entry* sub_entry;
    php_di_obj *php_di_obj;
	uint32_t req_num_args, i;
	int sub_result;

	if (nesting_limit == 0) {
		return -3;
	}

    php_di_obj = Z_PHPDI_P(this_ptr);

	if (ce->constructor) {
        req_num_args = ce->constructor->internal_function.required_num_args;
        for (i = 0; i < req_num_args; i++) {
            zval tmp;
            zend_type type = ce->constructor->internal_function.arg_info[i].type;
            if (!ZEND_TYPE_IS_CLASS(type)) {
                return -1;
            }

            if ((sub_entry = zend_lookup_class(ZEND_TYPE_NAME(type))) == NULL) {
                return -2;
            }

            zval* find_res = zend_hash_find(php_di_obj->instances, ZEND_TYPE_NAME(type));
            if (find_res == NULL) {
                zend_hash_add_empty_element(php_di_obj->instances, ZEND_TYPE_NAME(type));

                if ((sub_result = resolve_build_dependencies(sub_entry, nesting_limit - 1, this_ptr, &tmp)) < 0) {
                    return sub_result;
                }
            } else {
                if (ZVAL_IS_NULL(find_res)) {
                    // ok
                }
            }
        }
    }

	zval retval_o;
    if (UNEXPECTED(object_init_ex(&retval_o, ce) != SUCCESS)) {
        return -4;
    }

    int build_result = 0;

    if (ce->constructor) {
        if (!(ce->constructor->internal_function.fn_flags & ZEND_ACC_PUBLIC)) {
            zval_ptr_dtor(&retval_o);
            return -9;
        }
        build_result = build_instance(ce, this_ptr, &retval_o);
    }

    if (build_result != 0) {
        return build_result;
    }

    zend_hash_update(php_di_obj->instances, ce->name, &retval_o);

    ZVAL_COPY(retval, &retval_o);

	return 0;
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
        dependency_str = ZEND_TYPE_NAME(ce->constructor->internal_function.arg_info[i].type);
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

    return 0;
}

PHP_METHOD(DIContainer, withInstances)
{

}

static const zend_function_entry di_container_impl[] = {
	PHP_ME(DIContainer,			__construct,		arginfo_di_container_method_construct, ZEND_ACC_PUBLIC)
	PHP_ME(DIContainer, get,	arginfo_di_method_get, ZEND_ACC_PUBLIC)
	PHP_ME(DIContainer, withInstances,	arginfo_di_method_withInstances, ZEND_ACC_PUBLIC)
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

	return SUCCESS;
}
/* }}} */

// todo: internal store

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
