<?php

class DIContainerNative implements DIContainer
{
    public function get(string $className) {}
    public function withInstances(array $instances) : DIContainer {}
    public function withClassMap(array $mapping) : DIContainer {}
}

interface DIContainer {
    public function get(string $className);
    public function withInstances(array $instances) : DIContainer;
    public function withClassMap(array $mapping) : DIContainer;
}

class DIException extends Exception {}