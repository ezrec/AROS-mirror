<?php
	
        require_once("/home/groups/a/ar/aros/htdocs/rsfeed/rsslib.php");
	require_once("/home/groups/a/ar/aros/htdocs/rsfeed/customerror.php");
	set_error_handler("customError");
?>

<?php


	$url= "http://aros-exec.org/modules/newbb/rss.php?c=0";
	echo RSS_Links($url, 15);
?>




