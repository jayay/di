<?php

class DIContainer implements DIContainerInterface
{
    public function get(string $className) {}
    public function withInstances(array $instances) : DIContainerInterface {}
    public function withClassMap(array $mapping) : DIContainerInterface {}
}

class DIException extends Exception {}