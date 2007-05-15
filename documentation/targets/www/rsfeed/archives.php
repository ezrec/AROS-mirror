<?php

require_once("customerror.php");
//require_once("newtest.php");
set_error_handler("customError");

$file = fopen('http://archives.aros-exec.org/share/RECENT.readme', 
'r');

//$file = fopen('http://archives.aros-exec.org/share/FULLINDEX.readme','r');
if (!$file) {
	echo 'Archive information not available.'; 
	exit;
}
 

while($theData = fgets($file)) {
if($theData[0]!=';'){
	list ($field1, $field2, $field3, $field4, $field5, $field6, 
$field7, $field8, $field9, $field10, $field11, $field12, $field13, 
$field14, $field15, $field16 ) = 
split (' ', 
$theData);
	echo"<li><a href=\"http://archives.aros-exec.org/share/";
	echo $field1;
	echo "/";
	echo $field2;
	echo "\">";
	echo $field6;
	echo " ";
	echo $field7;
	echo " ";
	echo $field8;
	echo " ";
	echo $field9;
	echo " ";
	echo $field10;
	echo " ";
	echo $field11;
	echo " ";
	echo $field12;
	echo " ";
	echo $field13;
	echo " ";
	echo $field14;
	echo " ";
	echo $field15;
	echo "</a></li>";
/*
	echo "field 1: "; 	
	echo $field1;
	echo "<br>";
	echo "field 2: ";
	echo $field2;
	echo "<br>";
	echo "field 3: ";
	echo $field3;
	echo "<br>";
	echo "field 4: ";
	echo $field4;
        echo "<br>";
	echo "field 5: ";
	echo $field5;
        echo "<br>";
	echo "field 6: ";
	echo $field6;
        echo "<br>";
	echo "field 7: ";
	echo $field7;
        echo "<br>";
	echo "field 8: ";
	echo $field8;
	echo "<br>";
        echo "field 9: ";
        echo $field9;
        echo "<br>field 10: ";
        echo $field10;
        echo "<br>field 11: ";
        echo $field11;
        echo "<br>field 12: ";
        echo $field12;
        echo "<br>field 13: ";
        echo $field13;
        echo "<br>field 14: ";
        echo $field14;
        echo "<br>field 15: ";
        echo $field15;
        echo "<br>field 16: ";
        echo $field16;
*/
}
}
fclose($file);



?>
