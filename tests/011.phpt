--TEST--
Userland implementation of DIContainer
--SKIPIF--
<?php
if (!extension_loaded('di')) {
	echo 'skip';
}
?>
--FILE--
<?php

interface Injectable {}

class UserLandDIContainer extends DIContainer implements Injectable {
    public function get(string $classname) : Injectable {
        echo "get called with $classname\n";
        return parent::get($classname);
    }
}

class C implements Injectable {
    private $_diContainer;

    public function __construct(DIContainerInterface $di) {
        $this->_diContainer = $di;
    }

    public function action() {
        echo (string)$this->_diContainer->get(D::class);
    }
}

class D implements Injectable {
    public function __toString(): string {
        return "Hello World\n";
    }
}

$di = (new DIContainer)->withClassMap([DIContainer::class => UserLandDIContainer::class]);

var_dump($di->get(DIContainer::class));
var_dump($di->get(DIContainerInterface::class));
var_dump($di->get(C::class));
$di->get(C::class)->action();
?>
--EXPECT--
object(UserLandDIContainer)#1 (0) {
}
object(UserLandDIContainer)#1 (0) {
}
object(C)#3 (1) {
  ["_diContainer":"C":private]=>
  object(UserLandDIContainer)#1 (0) {
  }
}
get called with D
Hello World