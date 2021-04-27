<?php
require_once('config.php');
if ($_GET['key']) {
    if ($_GET['key'] === $config['client_key']) {
        if ($_GET['check_version'] == 1) {
            echo $config['version'];
        }
    }
    else {
        echo "incorrect client key.";
    }
}
?>