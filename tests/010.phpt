--TEST--
Injection of DIContainer
--SKIPIF--
<?php
if (!extension_loaded('di')) {
	echo 'skip';
}
?>
--FILE--
<?php

class C {
    private $_diContainer;

    public function __construct(DIContainer $di) {
        $this->_diContainer = $di;
    }
}

class D {
    private $_diContainer;

    public function __construct(DIContainerInterface $di) {
        $this->_diContainer = $di;
    }
}

$di = new DIContainer;
$di2 = $di->withClassMap([I::class => C::class]);

var_dump($di->get(DIContainer::class));
var_dump($di->get(DIContainerInterface::class));
var_dump($di2->get(C::class));
var_dump($di2->get(D::class));

?>
--EXPECT--
object(DIContainer)#1 (0) {
}
object(DIContainer)#1 (0) {
}
object(C)#3 (1) {
  ["_diContainer":"C":private]=>
  object(DIContainer)#1 (0) {
  }
}
object(D)#4 (1) {
  ["_diContainer":"D":private]=>
  object(DIContainer)#1 (0) {
  }
}