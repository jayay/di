--TEST--
DIContainerInterface implementation
--SKIPIF--
<?php
if (!extension_loaded('di')) {
	echo 'skip';
}
?>
--FILE--
<?php
class A implements \DIContainerInterface
{
    public function get(string $className) {}
    public function withInstances(array $instances) : DIContainerInterface {}
}

Reflection::export(new ReflectionClass(\DIContainerInterface::class));
$a = new A;
var_dump($a instanceof \DIContainerInterface);
var_dump(get_class($a));
?>
--EXPECT--
Interface [ <internal:di> interface DIContainerInterface ] {

  - Constants [0] {
  }

  - Static properties [0] {
  }

  - Static methods [0] {
  }

  - Properties [0] {
  }

  - Methods [2] {
    Method [ <internal:di> abstract public method get ] {

      - Parameters [1] {
        Parameter #0 [ <required> string $class_name ]
      }
    }

    Method [ <internal:di> abstract public method withInstances ] {

      - Parameters [1] {
        Parameter #0 [ <required> array $instances ]
      }
    }
  }
}

bool(true)
string(1) "A"