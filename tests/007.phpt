--TEST--
DIContainer class expansion
--SKIPIF--
<?php
if (!extension_loaded('di')) {
	echo 'skip';
}
?>
--FILE--
<?php

class A {}

$di = new \DIContainer;
var_dump($di->get(NonExistant::class));
var_dump($di->get(A::class));

?>
--EXPECT--
NULL
object(A)#2 (0) {
}