<?php
$br = (php_sapi_name() == "cli")? "":"<br>";

if(!extension_loaded('slim')) {
	dl('slim.' . PHP_SHLIB_SUFFIX);
}

$router = new Slim\Router;
//$route = new Slim\Router\Route;

$router->add(['GET'], '/home', function() {
    echo 'hello, slim';
});

$router->add(['POST'], '/users', function() {
    echo "post user";
});

print_r($router);