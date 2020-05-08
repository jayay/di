/* di extension for PHP (c) 2020 Jakob Jungmann */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "php.h"
#include "ext/standard/info.h"
#include "zend_interfaces.h"
#include "php_di.h"

/* For compatibility with older PHP versions */
#ifndef ZEND_PARSE_PARAMETERS_NONE
#define ZEND_PARSE_PARAMETERS_NONE() \
	ZEND_PARSE_PARAMETERS_START(0, 0) \
	ZEND_PARSE_PARAMETERS_END()
#endif


zend_class_entry *di_ce_interface, *di_ce_container;

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
	//Z_OBJ_HT_P(return_value)->

	//array_init_size();
}

// see https://github.com/php/php-src/blob/648be8600ff89e1b0e4a4ad25cebad42b53bed6d/ext/reflection/php_reflection.c#L4691
PHP_METHOD(DIContainer, get)
{
	zend_class_entry *class_subject;
	zend_string* cf;
	HashTable *myht;

	ZEND_PARSE_PARAMETERS_START(1,1)
		Z_PARAM_STR(cf)
	ZEND_PARSE_PARAMETERS_END();

	if ((class_subject = zend_lookup_class(cf)) == NULL) {
		RETURN_NULL();
		//zend_throw_exception_ex(reflection_exception_ptr, 0,
		//		"Class %s does not exist", ZSTR_VAL(name));
		//zend_string_release(name); // do again after throw
		//RETURN_THROWS();
	}

	uint32_t req_num_args = class_subject->constructor->common.required_num_args;

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


	if (UNEXPECTED(object_init_ex(return_value, class_subject) != SUCCESS)) {
		RETURN_LONG(4);
	}

	//return return_value;
	//RETURN_OBJ(return_value);
	//return_value = (&EX(This));
}

static int resolve_build_dependencies(zend_class_entry* ce, uint32_t nesting_limit, HashTable* storage)
{
	zend_string* dependency_str;
	zend_class_entry* sub_entry;
	zval value;
	uint32_t req_num_args, i;

	if (nesting_limit == 0) {
		return -3;
	}

	req_num_args = ce->constructor->common.required_num_args;
	for (i = 0; i < req_num_args; i++) {
		zend_type type = ce->constructor->common.arg_info[i].type;
		if (!ZEND_TYPE_IS_CLASS(type)) {
			return -1;
		}
		dependency_str = ce->constructor->common.arg_info[i].name;

		if ((sub_entry = zend_lookup_class(dependency_str)) == NULL) {
			return -2;
		}

		ZVAL_NULL(&value);
		zend_hash_update(storage, dependency_str, &value);
		
		//zend_update_property_ex()
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

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(di)
{
	zend_class_entry ce_container, ce_interface;
	zval instances_val;
	HashTable *myht;
	ALLOC_HASHTABLE(myht); /* TODO: destroy & free */
	zend_hash_init(myht, 1000, NULL, ZVAL_PTR_DTOR, 0);
	ZVAL_ARR(&instances_val, myht);

	INIT_CLASS_ENTRY(ce_interface, "DIContainerInterface", di_container_interface);
	INIT_CLASS_ENTRY(ce_container, "DIContainer", di_container_impl);

	di_ce_interface = zend_register_internal_interface(&ce_interface);
	di_ce_container = zend_register_internal_class(&ce_container);
	zend_class_implements(di_ce_container, 1, di_ce_interface);
	//zend_declare_property(di_ce_container, "instances", sizeof("instances")-1, &instances_val, ZEND_ACC_PRIVATE);

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
