<?
/*
 * bugs_locale.php
 *
 * @(#) $Header$
 *
 */

if($bugs_default_locale!="")
{
	if(file_exists($bugs_application_path."bugs_locale-$bugs_default_locale.php"))
	{
		include($bugs_application_path."bugs_locale-$bugs_default_locale.php");
	}
	else
		$bugs_debug("The include file for locale \"$bugs_default_locale\" does not exist!\n");
	if(file_exists("bugs_app_locale-$bugs_default_locale.php"))
	{
		include("bugs_app_locale-$bugs_default_locale.php");
	}
	else
		$bugs_debug("The include file for the application locale \"$bugs_default_locale\" does not exist!\n");
}

if($bugs_current_locale!=""
&& $bugs_current_locale!=$bugs_default_locale)
{
	if(file_exists("bugs_locale-$bugs_current_locale.php"))
	{
		include($bugs_application_path."bugs_locale-$bugs_current_locale.php");
	}
	else
		$bugs_debug("The include file for locale \"$bugs_current_locale\" does not exist!\n");
	if(file_exists("bugs_app_locale-$bugs_current_locale.php"))
	{
		include("bugs_app_locale-$bugs_current_locale.php");
	}
	else
		$bugs_debug("The include file for the application locale \"$bugs_current_locale\" does not exist!\n");
}

Function LocaleText($text)
{
	global $bugs_locale_text,$bugs_debug;
	
	if($text=="")
		$bugs_debug("It was not defined the name of the text to be localized!\n");
	if(IsSet($bugs_locale_text[$text]))
		return(/*"LOCALE: $text"*/$bugs_locale_text[$text]);
	else
	{
		$bugs_debug("It was not defined the locale text \"$text\"!\n");
		return("");
	}
}

?>