--TEST--
Recursive build together with method withClassMap
--SKIPIF--
<?php
if (!extension_loaded('di')) {
	echo 'skip';
}
?>
--FILE--
<?php

class A
{
    private $di;

    public function __construct(\DIContainer $di) {
        $this->di = $di;
    }

    public function do() {
        echo __METHOD__."\n";
        $this->di->get(B::class)->do();
    }
}

class B
{
    public function do() {
        echo __METHOD__."\n";
    }
}
$di1 = new \DIContainerNative;
$di2 = $di1->withClassMap([DiContainer::class => DiContainerNative::class]);
$di2->get(A::class)->do();

var_dump($di1);
var_dump($di2);
var_dump($di1->get(DIContainer::class));
var_dump($di1->get(DIContainerNative::class));
var_dump($di2->get(DIContainer::class));
var_dump($di2->get(DIContainerNative::class));
(new \DIContainerNative)->withClassMap([DiContainer::class => DiContainerNative::class])->get(B::class)->do();

?>
--EXPECT--
A::do
B::do
object(DIContainerNative)#1 (0) {
}
object(DIContainerNative)#2 (0) {
}
object(DIContainerNative)#1 (0) {
}
object(DIContainerNative)#1 (0) {
}
object(DIContainerNative)#2 (0) {
}
object(DIContainerNative)#2 (0) {
}
B::do