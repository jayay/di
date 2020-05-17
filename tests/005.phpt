--TEST--
DIContainer implementation
--SKIPIF--
<?php
if (!extension_loaded('di')) {
	echo 'skip';
}
?>
--FILE--
<?php
class A implements \DIContainer
{
    public function get(string $className) {}
    public function withInstances(array $instances) : DIContainer {}
    public function withClassMap(array $mapping) : DIContainer {}
}

echo strval(new ReflectionClass(\DIContainer::class));
$a = new A;
var_dump($a instanceof \DIContainer);
var_dump(get_class($a));
?>
--EXPECT--
Interface [ <internal:di> interface DIContainer ] {

  - Constants [0] {
  }

  - Static properties [0] {
  }

  - Static methods [0] {
  }

  - Properties [0] {
  }

  - Methods [3] {
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

    Method [ <internal:di> abstract public method withClassMap ] {

      - Parameters [1] {
        Parameter #0 [ <required> array $classmap ]
      }
    }
  }
}
bool(true)
string(1) "A"
