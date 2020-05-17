--TEST--
DIContainerNative declaration and instantiation
--SKIPIF--
<?php
if (!extension_loaded('di')) {
	echo 'skip';
}
?>
--FILE--
<?php
echo strval(new ReflectionClass(\DIContainerNative::class));

$di = new \DIContainerNative;

var_dump($di);
var_dump($di instanceof \DIContainerNative);
var_dump($di instanceof \DIContainer);

$di2 = new \DIContainerNative;
var_dump($di2);
?>
--EXPECT--
Class [ <internal:di> class DIContainerNative implements DIContainer ] {

  - Constants [0] {
  }

  - Static properties [0] {
  }

  - Static methods [0] {
  }

  - Properties [0] {
  }

  - Methods [4] {
    Method [ <internal:di, ctor> public method __construct ] {

      - Parameters [0] {
      }
    }

    Method [ <internal:di, prototype DIContainer> public method get ] {

      - Parameters [1] {
        Parameter #0 [ <required> string $class_name ]
      }
    }

    Method [ <internal:di, prototype DIContainer> public method withInstances ] {

      - Parameters [1] {
        Parameter #0 [ <required> array $instances ]
      }
    }

    Method [ <internal:di, prototype DIContainer> public method withClassMap ] {

      - Parameters [1] {
        Parameter #0 [ <required> array $classmap ]
      }
    }
  }
}
object(DIContainerNative)#1 (0) {
}
bool(true)
bool(true)
object(DIContainerNative)#2 (0) {
}