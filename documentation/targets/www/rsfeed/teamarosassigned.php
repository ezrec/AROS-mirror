<?php

include("/home/groups/a/ar/aros/htdocs/rsfeed/Snoopy.class.php");

/*$html = "<b>bold text</b><a href=howdy.html>click me</a><b>bold  
text</b><a href=howdy.html>click  
me</a>text<table><tr><td>column1</td><td>column2</ 
td><tr><table>text<table><tr><td>column1</td><td>column2</ 
td><tr><table>text";
*/
//$html = 
//implode('',file('http://thenostromo.com/teamaros2/index.php?query=assigned'));

$snoopy = new snoopy;
$snoopy->fetch("http://thenostromo.com/teamaros2/index.php?query=assigned");

$html = $snoopy->results;


// use this to only match "td" tags
#preg_match_all ( "/(<(td)>)([^<]*)(<\/\\2>)/", $html, $matches );
#preg_match_all ( "/(<(tr)>)([^<]*)(<\/\\2>)/", $html, $matches );
// us this to match any tags
preg_match_all("/(<([\w]+)[^>]*>)([^<]*)(<\/\\2>)/", $html, $matches);
#preg_match_all("/(<(td+)[^>]*>)([^<]*)(<\/\\2>)/", $html, $matches);

echo "The following bounties have been assigned to a developer, and are currently being worked on:<br><br>";

$assignedint = 18;
$testint = 16;
for ( $i=16; $i< count($matches[0]) -5; $i++)
	{
			$match21 = preg_replace("{<td>}", "", $matches[0][$assignedint]);
			$match22 = preg_replace("{</td>}", "", $match21);
			$matches2 = preg_replace("{<td>}", "", $matches[0][$i]);
			$matches3 = preg_replace("{</td>}", "", $matches2);
			$matches4 = preg_replace("{href=\"}", "href=\"http://www.thenostromo.com/teamaros2/", $matches3);
                        echo $matches4 . "<br>Assigned to: " . $match22;
			$i = $i +3;
			$assignedint = $assignedint + 4;
//			if($i == $testint + 3) {
//			$testint = $testint + 4;
			echo "<br>";
//			}
			
			
				
			//echo "part 1: " . $matches[1][$i] . "&nbsp;";
			//echo "part 2: " . $matches[2][$i] . "&nbsp;";
			//echo "part 3: " . $matches[3][$i] . "&nbsp;";
			//echo "part 4: " . $matches[4][$i] . "&nbsp;<br><br>";
		
}

?>
