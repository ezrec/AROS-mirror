<?php // -*- C++ -*-

function commonHeader($title) {
?><HTML>
<HEAD>
<TITLE><? echo $title; ?></TITLE>
</HEAD>
<BODY BGCOLOR="#F2D291" BACKGROUND="graphics/bgui_background.gif">
<H1><CENTER><? echo $title; ?></CENTER></H1>
<?
}


function commonFooter() {
?>
<HR>
<? echo LocaleText("footer-message"); ?>
</BODY>
</HTML>
<?
}
?>