--TEST--
Refcount of DIContainerNative output
--SKIPIF--
<?php
if (!extension_loaded('di')) {
	echo 'skip';
}
?>
--FILE--
<?php

class A {}

$di = new DIContainerNative;
$a = $di->get(A::class);
debug_zval_dump($a);

debug_zval_dump($di->get(A::class));

unset($a);

debug_zval_dump($di->get(A::class));

$b = $di->get(A::class);

unset($di);
debug_zval_dump($b);

$o = new A;
debug_zval_dump($o);
?>
--EXPECT--
object(A)#2 (0) refcount(3){
}
object(A)#2 (0) refcount(3){
}
object(A)#2 (0) refcount(2){
}
object(A)#2 (0) refcount(3){
}
object(A)#1 (0) refcount(2){
}