--TEST--
Check if di is loaded
--SKIPIF--
<?php
if (!extension_loaded('di')) {
	echo 'skip';
}
?>
--FILE--
<?php
echo 'The extension "di" is available';
?>
--EXPECT--
The extension "di" is available
