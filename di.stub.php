<?php

class DIContainerNative implements DIContainerInterface
{
    public function get(string $className) {}
    public function withInstances(array $instances) : DIContainerInterface {}
    public function withClassMap(array $mapping) : DIContainerInterface {}
}

interface DIContainerInterface {
    public function get(string $className);
    public function withInstances(array $instances) : DIContainerInterface;
    public function withClassMap(array $mapping) : DIContainerInterface;
}

class DIException extends Exception {}