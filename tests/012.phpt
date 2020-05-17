--TEST--
Instantiation of DIException
--SKIPIF--
<?php
if (!extension_loaded('di')) {
	echo 'skip';
}
?>
--FILE--
<?php
echo get_class(new DIException)."\n";
var_dump((new DIException) instanceof Exception);
?>
--EXPECT--
DIException
bool(true)