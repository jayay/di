/* di extension for PHP (c) 2020 Jakob Jungmann */

#ifndef PHP_DI_H
# define PHP_DI_H

extern zend_module_entry di_module_entry;
# define phpext_di_ptr &di_module_entry

# define PHP_DI_VERSION "0.1.0"

# if defined(ZTS) && defined(COMPILE_DL_DI)
ZEND_TSRMLS_CACHE_EXTERN()
# endif

#endif	/* PHP_DI_H */
