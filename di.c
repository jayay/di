/* di extension for PHP (c) 2020 Jakob Jungmann */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "php.h"
#include "ext/standard/info.h"
#include "Zend/zend_interfaces.h"
#include "php_di.h"

/* For compatibility with older PHP versions */
#ifndef ZEND_PARSE_PARAMETERS_NONE
#define ZEND_PARSE_PARAMETERS_NONE() \
	ZEND_PARSE_PARAMETERS_START(0, 0) \
	ZEND_PARSE_PARAMETERS_END()
#endif

zend_class_entry *di_ce_interface, *di_ce_container;

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

static void dicontainer_ctor(INTERNAL_FUNCTION_PARAMETERS)
{
	// TODO: init empty array
}

PHP_METHOD(DIContainer, __construct)
{

}

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
	INIT_CLASS_ENTRY(ce_interface, "DIContainerInterface", di_container_interface);

	di_ce_interface = zend_register_internal_interface(&ce_interface);

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
