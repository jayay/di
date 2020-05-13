/* di extension for PHP (c) 2020 Jakob Jungmann */

#ifndef PHP_DI_H
# define PHP_DI_H

#include "php.h"

extern zend_module_entry di_module_entry;
# define phpext_di_ptr &di_module_entry

# define PHP_DI_VERSION "0.1.0"

# if defined(ZTS) && defined(COMPILE_DL_DI)
ZEND_TSRMLS_CACHE_EXTERN()
# endif

PHP_METHOD(DIContainer,	__construct);
PHP_METHOD(DIContainer, get);
PHP_METHOD(DIContainer, withInstances);

typedef struct _php_di_obj php_di_obj;

struct _php_di_obj {
    HashTable *instances;
    zend_object std;
};

static inline php_di_obj *php_di_obj_from_obj(zend_object *obj) {
    return (php_di_obj*)((char*)(obj) - XtOffsetOf(php_di_obj, std));
}

#define Z_PHPDI_P(zv)  php_di_obj_from_obj(Z_OBJ_P((zv)))

static int resolve_build_dependencies(zend_class_entry* ce, uint32_t nesting_limit, zval *this_ptr, zval* retval);
static int build_instance(zend_class_entry *ce, zval *this_ptr, zval *new_obj);

#endif	/* PHP_DI_H */
