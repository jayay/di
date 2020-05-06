--TEST--
DIContainerInterface registration
--SKIPIF--
<?php
if (!extension_loaded('di')) {
	echo 'skip';
}
?>
--FILE--
<?php
var_dump(interface_exists(\DIContainerInterface::class));
?>
--EXPECT--
bool(true)
