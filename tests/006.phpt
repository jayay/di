--TEST--
DIContainer declaration and instantiation
--SKIPIF--
<?php
if (!extension_loaded('di')) {
	echo 'skip';
}
?>
--FILE--
<?php
Reflection::export(new ReflectionClass(\DIContainer::class));

$di = new \DIContainer;

var_dump($di);
var_dump($di instanceof \DIContainer);
var_dump($di instanceof \DIContainerInterface);

$di2 = new \DIContainer;
var_dump($di2);
?>
--EXPECT--
Class [ <internal:di> class DIContainer implements DIContainerInterface ] {

  - Constants [0] {
  }

  - Static properties [0] {
  }

  - Static methods [0] {
  }

  - Properties [1] {
    Property [ <default> private $instances ]
  }

  - Methods [3] {
    Method [ <internal:di, ctor> public method __construct ] {

      - Parameters [0] {
      }
    }

    Method [ <internal:di, prototype DIContainerInterface> public method get ] {

      - Parameters [1] {
        Parameter #0 [ <required> string $class_name ]
      }
    }

    Method [ <internal:di, prototype DIContainerInterface> public method withInstances ] {

      - Parameters [1] {
        Parameter #0 [ <required> array $instances ]
      }
    }
  }
}

object(DIContainer)#1 (0) {
}
bool(true)
bool(true)
object(DIContainer)#2 (0) {
}