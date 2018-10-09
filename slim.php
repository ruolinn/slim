<?php
class HomeController
{
    public function index()
    {
        //return 'Hello, String';
        return ['message' => 'Hello, Json'];
    }
}

$br = (php_sapi_name() == "cli")? "":"<br>";

if(!extension_loaded('slim')) {
	dl('slim.' . PHP_SHLIB_SUFFIX);
}

//$response = new Slim\Http\Response;
//$response->setJsonContent(['key' => 'value']);

//print_r($response);exit;

$app = new Slim\App;//print_r(get_parent_class($app));exit;

$router = $app->getShared('router');

$router->add(['GET'], '/home', 'HomeController::index');

/*
$router->add(['GET'], '/home', function() {
    echo 'hello, slim';
});
*/

$router->add(['POST'], '/users', function() {
    echo "post user";
});

//$_SERVER['REQUEST_URI'] = '/users?name=wangxiaoguang';
$_SERVER['REQUEST_URI'] = '/home?name=wangxiaoguang';

$app->handle();


//$router = $app->getShared('router');

//print_r($router);
