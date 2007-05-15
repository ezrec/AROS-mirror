<?php

include("/home/groups/a/ar/aros/htdocs/rsfeed/Snoopy.class.php");

require_once("customerror.php");
set_error_handler("customError");

/*$html = "<b>bold text</b><a href=howdy.html>click me</a><b>bold  
text</b><a href=howdy.html>click  
me</a>text<table><tr><td>column1</td><td>column2</ 
td><tr><table>text<table><tr><td>column1</td><td>column2</ 
td><tr><table>text";
*/

$snoopy = new snoopy;
$snoopy->fetch("http://thenostromo.com/teamaros2/index.php?query=open");

$html = $snoopy->results;

// use this to only match "td" tags
#preg_match_all ( "/(<(td)>)([^<]*)(<\/\\2>)/", $html, $matches );
#preg_match_all ( "/(<(tr)>)([^<]*)(<\/\\2>)/", $html, $matches );
// us this to match any tags
preg_match_all("/(<([\w]+)[^>]*>)([^<]*)(<\/\\2>)/", $html, $matches);
#preg_match_all("/(<(td+)[^>]*>)([^<]*)(<\/\\2>)/", $html, $matches);

echo "The following bounties are currently waiting on a developer to undertake the projects: <br><br>";

$testint = 14;
for ( $i=14; $i< count($matches[0]) -5; $i++)
	{
		if($matches[0][$i] != "<font color=\"#997777\">not available</font>") {
			$matches2 = preg_replace("{href=\"}", "href=\"http://www.thenostromo.com/teamaros2/", $matches[0][$i]);
#			echo "<small><small>";
			echo $matches2; 
#			echo "</small></small>";
			$i = $i + 1;
"&nbsp;";
//			if($i == $testint + 1) {
//			$testint = $testint + 2;
			echo "<br>";
//			}
			
			//echo "part 1: " . $matches[1][$i] . "&nbsp;";
			//echo "part 2: " . $matches[2][$i] . "&nbsp;";
			//echo "part 3: " . $matches[3][$i] . "&nbsp;";
			//echo "part 4: " . $matches[4][$i] . "&nbsp;<br><br>";
		}	
}

?>
