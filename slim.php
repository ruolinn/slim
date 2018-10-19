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

//$e = new Slim\Events\Exception('ha');
//throw $e;

class Handler
{
    public function after($event, $source, $data)
    {

    }
}

$loader = new Slim\Loader();
$loader->registerNamespaces([
    'App' => __DIR__.'/src',
]);
$loader->register();


//$request = new Slim\Http\Request();
//var_dump($request->getPost());exit;
//var_dump($request->getQuery('name'));exit;
//var_dump($request->getServer('HTTP_HOST'));exit;
//var_dump($request->getEnv('php'));exit;
//var_dump($request->has('name'));exit;
//var_dump($request->hasPost('name'));exit;
//var_dump($request->isPut());exit;
//var_dump($request->getRawBody());exit;
//var_dump($request->getPut('age', 15));exit;
//var_dump($request->hasPut('age'));exit;
//var_dump($request->hasQuery('age'));exit;
//var_dump($request->hasServer('HTTP_HOST'));exit;
//var_dump($request->hasHeader('CONNECTION'));exit;
//var_dump($request->getHeader('HTTP_CONNECTION'));exit;
//var_dump($request->getScheme());exit;
//var_dump($request->isSecure());exit;
//var_dump($request->getServerAddress());exit;
//var_dump($request->getServerName());exit;
//var_dump($request->getHttpHost());exit;
//var_dump($request->getClientAddress());exit;
//var_dump($request->getMethod());exit;
//var_dump($request->getURI());exit;
//var_dump($request->getQueryString());exit;
//var_dump($request->getUserAgent());exit;
//var_dump($request->isMethod(['GET', 'POST']));
//var_dump($request->getHeaders());exit;
//var_dump($request->getHttpReferer());exit;
//print_r($request->getAcceptableContent());exit;
//print_r($request->getBestAccept());exit;
//var_dump($request->getClientCharsets());exit;
//var_dump($request->getBestCharset());exit;
//var_dump($request->getLanguages());exit;
//var_dump($request->getBestLanguage());exit;

//$response = new Slim\Http\Response;
//$response->setStatusCode(200, 'yes');
//$response->setHeader('Accept', 'img');
//$response->setHeaders(['Content-Type' => 'text/html']);
//$response->setCookies(['key' => 'value']);
//var_dump($response->getCookies());exit;
//$response->setRawHeader('HTTP/1.1 404 Not Found');
//$response->resetHeaders();
//$response->setExpires(new DateTime());
//$response->setContentType("application/json", "utf-8");
//$response->setNotModified();
//$response->setEtag('ene');
//var_dump($response->getContent());exit;
//$response->setContent('{"key":"vakue"}');
//var_dump($response->getJsonContent());exit;
//$response->setHeader("Content-Type","application/bson");
//$response->sendHeaders();
//print_r($response);exit;
/*
$router = new Slim\Router;
$router->add('GET', '/home', function(){});
$router->add('GET', '/profile', function(){});
$router->add('POST', '/posts', function(){});
print_r($router);exit;
*/

class HttpMiddleware
{
    public function handle()
    {
        echo 'api';
    }
}

class ResponseMiddleware
{
    public function handle()
    {
        echo  'web';
        return false;
    }
}

$_SERVER['REQUEST_METHOD'] = 'GET';
$app = new Slim\App;

//$app->middleware([HttpMiddleware::class]);
//$app->middleware(ResponseMiddleware::class);



$app->routeMiddleware([
    'api' => HttpMiddleware::class,
    'web' => ResponseMiddleware::class,
]);

//$app->routeMiddleware('api', HttpMiddleware::class);

//print_r($app);exit;


//print_r($app);exit;
//print_r($app);exit;

//$events = $app->getShared('events');

//print_r($events);exit;

$router = $app->getShared('router');

$router->add(['GET', 'POST'], "/home/{year:[0-9]+}/{title:[a-z\-]+}", 'HomeController::index')->middleware(['api']);

/*
$router->add(['GET'], '/home', function() {
    echo 'hello, slim';
});
*/

$router->add(['POST'], '/users', function() {
    echo "post user";
});

//$_SERVER['REQUEST_URI'] = '/users?name=wangxiaoguang';
$_SERVER['REQUEST_URI'] = '/home/2019/title?name=wangxiaoguang';

//$router->dispatch('GET', '/home/2019/title');

//$route = $router->getMatchedRoute();
//print_R($route);exit;
//print_r($route->getPaths());exit;
//print_r($router->getRoutes());exit;
//print_r($route->prepare("App\Http\HomeController"));exit;
//var_dump($route->isMethod("GET"));exit;
//var_dump($route->isMethod("POST"));exit;

//print_r($router);exit;

$app->run();
