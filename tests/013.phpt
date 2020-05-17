--TEST--
Exception cases
--SKIPIF--
<?php
if (!extension_loaded('di')) {
	echo 'skip';
}
?>
--INI--
error_reporting=32639
--FILE--
<?php

class R {}

class A {
    private $b;
    private function __construct(B $b) {
        $this->b = $b;
    }
}

class B {
    public function __construct(Nonexistant $n) {}
}


abstract class C {
    private $r;

    abstract public function __construct(R $r);
}

interface I {
    public function __construct(R $r);
}

abstract class D implements I {}

class E {
    public function __construct($a) {}
}

class F {
    public function __construct(string $a, int $b, float $c, array $d, resource $e, stdClass $f) {}
}

class G {
    public function __construct(int $b, float $c, array $d, resource $e, stdClass $f) {}
}

class H {
    public function __construct(float $c, array $d, resource $e, stdClass $f) {}
}

class J {
    public function __construct(array $d, resource $e, stdClass $f) {}
}

class K {
    public function __construct(resource $e, stdClass $f) {}
}

class L {
    public function __construct(R ...$a) {}
}

$classesToBeBuilt = [
    A::class,
    B::class,
    C::class,
    I::class,
    D::class,
    '',
    E::class,
    F::class,
    G::class,
    J::class,
    K::class,
    L::class,
];

foreach ($classesToBeBuilt as $class) {
    $di = new DIContainer();
    try {
        var_dump($di->get($class));
        echo "DIException expected\n";
    } catch (DIException $e) {
        var_dump($class.": ".$e->getMessage());
    }
}

?>
--EXPECTF--
string(33) "A: Constructor of A is not public"
string(39) "B: Failed autoloading class Nonexistant"
string(40) "C: Constructor of C must not be abstract"
string(%d) "I: %s"
string(43) "D: Construction of object of class D failed"
string(23) ": Class  does not exist"
string(52) "E: Argument 1 of class E is not a class or interface"
string(52) "F: Argument 1 of class F is not a class or interface"
string(52) "G: Argument 1 of class G is not a class or interface"
string(52) "J: Argument 1 of class J is not a class or interface"
string(36) "K: Failed autoloading class resource"
string(50) "L: Variadic constructors are not allowed (class L)"