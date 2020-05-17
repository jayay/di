--TEST--
DIContainer registration
--SKIPIF--
<?php
if (!extension_loaded('di')) {
	echo 'skip';
}
?>
--FILE--
<?php
var_dump(interface_exists(\DIContainer::class));
?>
--EXPECT--
bool(true)
