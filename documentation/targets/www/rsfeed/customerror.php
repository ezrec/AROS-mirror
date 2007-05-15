<?php
function customError($errno, $errstr)
 { 
 if($errno != 8) {
	if ($errono != 1024) {
 echo "<b>Guru Meditation:</b> [$errno] $errstr<br />";
	}
 }
 //die();
 }
?>
