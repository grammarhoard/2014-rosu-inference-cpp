<?php
/**
 * CORE
 * @author alpha
 */
class Amvc
{
    public static $config = array();
    public static $cache = array();
    public static $modules = array();
    private $default = array();
    private $routing = array();
    private $url;
    private $model;
    private $controller;
    private $action;
    private $queryString = array();

    /**
     * Amvc constructor:
     * - loadConfig();
     * - loadCache();
     * - setReporting();
     * - removeMagicQuotes();
     * - unregisterGlobals();
     * - setDefault();
     * - setRouting();
     */
    public function __construct()
    {
        $this->loadConfig();
        $this->loadCache();
        $this->setReporting();

        $this->removeMagicQuotes();
        $this->unregisterGlobals();

        $this->setDefault();
        $this->setRouting();
    }

    /**
     * Set error reporting by environment
     */
    private function setReporting()
    {
        error_reporting(E_ALL);

        if (self::$config->DEVELOPMENT_ENVIRONMENT == 1) {
            ini_set('display_errors', 'On');
        } else {
            ini_set('display_errors', 'Off');
            ini_set('log_errors', 'On');
            ini_set('error_log', LOGS_PATH . 'error.log');
        }
    }

    /**
     * Remove magic quotes
     */
    private function removeMagicQuotes()
    {
        if (!get_magic_quotes_gpc()) {
            return;
        }
        $_GET    = stripSlashesDeep($_GET   );
        $_POST   = stripSlashesDeep($_POST  );
        $_COOKIE = stripSlashesDeep($_COOKIE);
    }

    /**
     * Unregister globals
     */
    private function unregisterGlobals()
    {
        if (!ini_get('register_globals')) {
            return;
        }
        $registerGlobals = array(
            '_SESSION', '_POST', '_GET', '_COOKIE', '_REQUEST', '_SERVER', '_ENV', '_FILES'
        );
        foreach ($registerGlobals as $registerGlobal) {
            foreach ($GLOBALS[$registerGlobal] as $key => $var) {
                if ($var === $GLOBALS[$key]) {
                    unset($GLOBALS[$key]);
                }
            }
        }
    }

    /**
     * Load Configuration Constants
     */
    private function loadConfig()
    {
        self::$config = new Config();
    }

    /**
     * Load Cache functionality
     */
    private function loadCache()
    {
        self::$cache = new Cache();
    }

    /**
     * Specifies default controller and action
     */
    public function setDefault()
    {
        $this->default = self::$config->amvc['default'];
    }

    /**
     * Can specify custom redirects using regular expressions
     * ie: http://localhost/framework/admin/categories/view =>
     *     http://localhost/framework/admin/categories_view where
     *     admin = controller and categories_view = action
     * can have urls like admin/categories/delete/15 =>
     *     call categories_delete (15) in the controllers/admincontroller.php file
     */
    public function setRouting()
    {
        $this->routing = self::$config->amvc['routing'];
    }

    /**
     * GZip Output
     */
    /*
    public function gzipOutput()
    {
        $ua = $_SERVER['HTTP_USER_AGENT'];

        if (0 !== strpos($ua, 'Mozilla/4.0 (compatible; MSIE ') || false !== strpos($ua, 'Opera')) {
            return false;
        }

        $version = (float) substr ($ua, 30); 
        return ($version < 6) || ($version == 6  && false === strpos($ua, 'SV1'));
    }
    */

    /**
     * Register a module and include class files
     * @param string $moduleName
     * @param string $modelName
     */
    public function registerModule($moduleName, $modelName)
    {
        $controller = $moduleName . 'controller.php';
        $model      = $modelName . '.php';

        $controllerPath = APP_PATH . $moduleName . DS . $controller;
        $modelPath = APP_PATH . $moduleName . DS . $model;

        // Check if class files exists
        if (!file_exists($controllerPath)) {
            showError('Controller file "' . $controllerPath . '" does not exist!', E_USER_ERROR);
        }
        if (!file_exists($modelPath)) {
            showError('Model file "' . $modelPath . '" does not exist!', E_USER_ERROR);
        }

        require_once($controllerPath);
        require_once($modelPath);

        self::$modules[$moduleName] = $modelName;
    }

    /**
     * Main Processing 
     * - takes the received URL from index.php and separates it into:
     *     $controller, $action and the remaining as $queryString
     * - model is the singular version of $controller
     * - creates a new object of the class $controller . 'Controller'
     * - calls the method $action of the class
     */
    public function callHook()
    {
        if (!isset($_GET['url'])) {
            $this->controller = $this->default['controller'];
            $this->action = $this->default['action'];
        } else {
            $this->url = $this->routeURL($_GET['url']);
            $urlArray = array();
            $urlArray = explode("/", $this->url);

            $this->controller = $urlArray[0];
            array_shift($urlArray);
            if (isset($urlArray[0])) {
                $this->action = $urlArray[0];
                array_shift($urlArray);
            } else {
                $this->action = 'index'; // Default Action
            }
            $this->queryString = $urlArray;
        }

        // Check if controller exists
        if (!isset(self::$modules[$this->controller])) {
            showError('Controller "' . $this->controller . '" does not exist!', E_USER_ERROR);
        }

        $this->model = self::$modules[$this->controller];
        $controllerName = ucfirst($this->controller) . 'Controller';
        $dispatch = new $controllerName($this->model, $this->action);

        // Check if method exists
        if (!method_exists($controllerName, $this->action)) {
            showError('Method "' . $controllerName . '->' . $this->action . '" does not exist!', E_USER_ERROR);
        }

        call_user_func_array(array($dispatch, "beforeAction"), $this->queryString);
        call_user_func_array(array($dispatch, $this->action), $this->queryString);
        call_user_func_array(array($dispatch, "afterAction"), $this->queryString);
    }

    /**
     * Route url
     * checks for routes defined in config
     * @param string $url
     */
    private function routeURL($url)
    {
        // TODO Sanitize url
        foreach ($this->routing as $pattern => $result) {
            if (preg_match($pattern, $url)) {
                return preg_replace($pattern, $result, $url);
            }
        }
        return $url;
    }

    /**
     * Secondary Call Function
     */
    public static function performAction($controller, $action, $queryString = null, $render = 0)
    {
        // Check if controller exists
        if (!isset(Amvc::$modules[$controller])) {
            showError('Controller "' . $controller . '" does not exist!', E_USER_ERROR);
        }

        $controllerName = ucfirst($controller) . 'Controller';
        $dispatch = new $controllerName(Amvc::$modules[$controller], $action);
        $dispatch->render = $render;
        return call_user_func_array(array($dispatch, $action), $queryString);
    }

    //TODO Redirect function
    /*
    private function redirectAction($controller, $action, $queryString = null, $render = 0)
    {
        // will perform only the redirected action and display that corresponding view only
        // will be helpful during user authentication
        // if the user is not logged in, then we can call a function like ->
        // redirectAction ('user', 'login', array ($returnUrl));

        $controllerName = ucfirst ($controller) . 'Controller';
        $dispatch = new $controllerName ($controller, $action);
        $dispatch->render = $render;
        return call_user_func_array (array ($dispatch, $action), $queryString);
    }
    */
}