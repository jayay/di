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
$di = new DIContainerNative;
$app = $di->get(App::class);
```

To create a new dependency injection container and tell it to use the `DatabasePG` implementation for the `Database` interface, write

```php
<?php
$di = (new DIContainerNative)
  ->withClassMap([
    Database::class => DatabasePG::class,
  ]);
```

As soon as a new `DIContainerNative` gets instantiated, one default entry in the class mapping gets created:
Interface `DIContainer` points to the implementation `DIContainerNative`. The interface is defined as follows:
```php
<?php
interface DIContainer {
    public function get(string $className);
    public function withInstances(array $instances) : DIContainer;
    public function withClassMap(array $mapping) : DIContainer;
}
```
If a `DIContainer` is requested, `$this` will be returned by the container by default.
The implementation of `DIContainerNative` itself can be overwritten as well by pointing the interface to a new class.

## License

DI was published under The PHP License, version 3.01. See [LICENSE document](LICENSE.txt).
