--TEST--
DIContainer withClassMap
--SKIPIF--
<?php
if (!extension_loaded('di')) {
	echo 'skip';
}
?>
--FILE--
<?php

interface I {};

class C implements I {}

$di = new DIContainer;
$di2 = $di->withClassMap([I::class => C::class]);

var_dump($di);
var_dump($di2);
var_dump($di === $di2);
var_dump($di2->get(I::class));

?>
--EXPECT--
object(DIContainer)#1 (0) {
}
object(DIContainer)#2 (0) {
}
bool(false)
object(C)#3 (0) {
}