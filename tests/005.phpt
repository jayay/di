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

$a = new A;
var_dump($a instanceof \DIContainerInterface);
var_dump(get_class($a));
?>
--EXPECT--
bool(true)
string(1) "A"
