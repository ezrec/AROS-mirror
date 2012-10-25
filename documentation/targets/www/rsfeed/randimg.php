<?php
/*
Random image PHP script, written by Henric Blomgren (henric@digital-bless.com), 08 Jan 2006.
Updated to version 1.1: 18 January 2006.
You are free to use, modify but not redistribuite any and all parts this
script, copyright © 2006 Henric Blomgren. Please email any comments & patches :)

Useage:
<?php
include_once('/home/groups/a/ar/aros/htdocs/rsfeed/randimg.php');
random_image("images/top/","175","120");
?>

Where "images/top/" is the directory the script will look for images in, "175" is the width
and "120" is the height.
Width and height are optional, and if they are left blank ("") then those attributes
will be left out of the <img> tag.

To make it more compact in a PHP file you can make it into a single line:
<?php include_once 'rand_img.php'; random_image("images/top/","175","120"); ?>
Or without the height/width arguments:
<?php include_once 'rand_img.php'; random_image("images/top/","",""); ?>

Keep in mind NOT to pass paths to the function like:
<?php include_once 'rand_img.php'; random_image("$path","",""); ?>
You should NOT do this as on some servers where enable_globals is set to on,
the end-user could just:
http://www.domain.com/file.php?path=<bad path>
And by doing so potentially access sensetive files.
*/
function random_image($dir,$w,$h) {
    $dir2 = '/home/project-web/aros/htdocs'.$dir;
    //echo $dir2;
    if(file_exists($dir2) AND !ereg("\.\.",$dir) AND !ereg("\.ht",$dir)) {
        $d = dir("$dir2");
        $i = 0;
        while (false !== ($entry = $d->read())) {
            if(eregi(".jpg|.jpeg|.gif|.png",$entry)) {
                $array[$i] = $entry;
                $i++;
            }
        }
        $d->close();
        $i = $i -1;
        $i = rand(0,$i);
        if($array[$i] != "") {
            echo "<img src=\"$dir$array[$i]\"";
            if(isset($w) AND is_numeric($w)) {
                echo " width=\"$w\"";
            }
            if(isset($h) AND is_numeric($h)) {
                echo " height=\"$h\" style=\"float: right; padding: 1.5em;\"";
            }
            echo ">";
        } else {
            echo "Error: Failed to select a random image (does the dir contain any .jpg/.png/gif's?).";
        }
    } else {
        echo "Error: Directory $dir doesn't seem to exist.";
    }
}
?>
