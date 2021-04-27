<?php
require_once('config.php');
if ($_GET['key']) {
    if ($_GET['key'] === $config['secret_key']) {
        $name = "cheat.dll";
        $fp = fopen($name, 'r');
        header("Content-Type: application/octet-stream");
        header("Content-Length: " . filesize($name));
        header('Content-Disposition: attachment; filename="cheat.dll"');
        fpassthru($fp);
    }
    else {
        echo "incorrect server key";
    }
}
else {
    if (!file_exists("loader.exe")) {
        echo "Loader not found";
    }
    else {
        $name = "loader.exe";
        $fp = fopen($name, 'r');
        header("Content-Type: application/octet-stream");
        header("Content-Length: " . filesize($name));
        header('Content-Disposition: attachment; filename="loader.exe"');
        fpassthru($fp);
    }
}