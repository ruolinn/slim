<?php
$br = (php_sapi_name() == "cli")? "":"<br>";

if(!extension_loaded('slim')) {
	dl('slim.' . PHP_SHLIB_SUFFIX);
}

class User
{

}


$container = new Slim\Container;
$container->set('router', Slim\Router::class);


$router = $container->get('router');
$not = $container->get(User::class);

print_r($not);exit;

//$app = new Slim\App;
//print_r($app->handle());exit;

$router->add(['GET'], '/home', function() {
    echo 'hello, slim';
});

$router->add(['POST'], '/users', function() {
    echo "post user";
});

//$_SERVER['REQUEST_URI'] = '/users?name=wangxiaoguang';
$_SERVER['REQUEST_URI'] = '/home?name=wangxiaoguang';

$router->handle();

print_r($router);