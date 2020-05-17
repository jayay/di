--TEST--
DIContainerNative nested class expansion
--SKIPIF--
<?php
if (!extension_loaded('di')) {
	echo 'skip';
}
?>
--FILE--
<?php

class A {}
class B {}
class C {
    private $a;
    private $b;
    public function __construct(A $a, B $b) {
        $this->a = $a;
        $this->b = $b;
    }
}

class D {
    private $a;
    private $c;
    public function __construct(A $a, C $c) {
        $this->a = $a;
        $this->c = $c;
    }
}

$di = new \DIContainerNative;
var_dump($di->get(C::class));
var_dump($di->get(D::class));

$di2 = new \DIContainerNative;
var_dump($di->get(C::class) === $di->get(C::class));
var_dump($di->get(C::class) === $di2->get(C::class));
?>
--EXPECT--
object(C)#4 (2) {
  ["a":"C":private]=>
  object(A)#2 (0) {
  }
  ["b":"C":private]=>
  object(B)#3 (0) {
  }
}
object(D)#5 (2) {
  ["a":"D":private]=>
  object(A)#2 (0) {
  }
  ["c":"D":private]=>
  object(C)#4 (2) {
    ["a":"C":private]=>
    object(A)#2 (0) {
    }
    ["b":"C":private]=>
    object(B)#3 (0) {
    }
  }
}
bool(true)
bool(false)