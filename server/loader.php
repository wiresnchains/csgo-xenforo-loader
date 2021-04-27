<?php
if ($_GET['username']) {
    require('src/XF.php');
    XF::start( $fileDir );
	$app = \XF::setupApp('XF\App');
    
    $ip = $_SERVER['REMOTE_HOST'];

	if ( isset( $_GET[ "password" ] ) )
	{
		$loginService = $app->service('XF:User\Login', $_GET["username"], $ip);
		$userValidate = $loginService->validate($_GET["password"], $error);
		if($userValidate)
		{
		    $username = $_GET['username'];
		    $hwid = $_GET['hwid'];
		    $sql = mysqli_connect("host", "name", "pass", "database");
		    $query = mysqli_query($sql, "SELECT * FROM hwids WHERE username='$username'");
		    $count = mysqli_num_rows($query);
		    $row = mysqli_fetch_assoc($query);
		    if ($count > 0) {
		        if ($row['hwid'] == $hwid) {
		            echo "success";
		        }
		        else {
		            echo "hwid:fail";
		        }
		    }
		    else {
		        $query = mysqli_query($sql, "INSERT INTO hwids (username, hwid) VALUES ('$username', '$hwid')");
		        echo "success";
		    }
		}
		else {
		    echo "fail";
		}
	}
	else {
	    echo "fail";
	}
}
?>