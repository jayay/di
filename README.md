# DI 
![C/C++ CI](https://github.com/jayay/di/workflows/C/C++%20CI/badge.svg)

DI is a native dependency injection framework for PHP written as a C extension.

⚠️ IMPORTANT: This is an EXPERIMENTAL extension, do not use in production! ⚠️

## Install Guide

Make sure to have the PHP development package installed. 

On Ubuntu:
```
sudo apt install php-dev
```
Or on Fedora:
```
sudo dnf install php-devel
```
Please note this extension was tested for PHP 7.3 and above.

```
git clone https://github.com/jayay/di
cd di
phpize
./configure
make
sudo make install
```
You may also want to edit your `php.ini` and add the following line at the end of the file:
```
extension=di.so
```

## Usage

In order to build a class and its dependencies, use the `get` method:

```php
<?php
$di = new DIContainer;
$app = $di->get(App::class);
```

To create a new dependency injection container and tell it to use the `DatabasePG` implementation for the `Database` interface, write

```php
<?php
$di = (new DIContainer)
  ->withClassMap([
    Database::class => DatabasePG::class,
  ]);
```

As soon as a new `DIContainer` gets instantiated, one default entry in the class mapping gets created:
Interface `DIContainerInterface` points to the implementation `DIContainer`. The interface is defined as follows:
```php
<?php
interface DIContainerInterface {
    public function get(string $className);
    public function withInstances(array $instances) : DIContainerInterface;
    public function withClassMap(array $mapping) : DIContainerInterface;
}
```
If a `DIContainerInterface` is requested, `$this` will be returned by the container by default.
The implementation of the `DIContainer` itself can be overwritten as well by pointing the interface to a new class.

## License

DI was published under The PHP License, version 3.01. See [LICENSE document](LICENSE.txt).
