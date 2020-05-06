--TEST--
di_test1() Basic test
--SKIPIF--
<?php
if (!extension_loaded('di')) {
	echo 'skip';
}
?>
--FILE--
<?php
$ret = di_test1();

var_dump($ret);
?>
--EXPECT--
The extension di is loaded and working!
NULL
