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

try {
    $di->get(NonExistant::class);
    echo "DIException expected";
} catch (DIException $ex) {
    var_dump($ex->getCode());
    var_dump($ex->getMessage());
}
var_dump($di->get(A::class));

?>
--EXPECT--
int(0)
string(32) "Class NonExistant does not exist"
object(A)#3 (0) {
}