<?
/*
 * bugs_globals.php
 *
 * @(#) $Header$
 *
 */

Function DefaultDebugFunction($message)
{
	global $bugs_project_debug;

	if($bugs_project_debug)
	{
		Header("Content-Type: text/plain");
		echo $message;
	}
	exit;
}

$bugs_report_destination = "";
$bugs_shared_include_file="bugs_shared.php";
$bugs_project_debug=0;
$bugs_debug=DefaultDebugFunction;
$bugs_default_locale="en";
$bugs_current_locale="en";
$bugs_locale_text=array();
$bugs_password_file="";
$bugs_database="";
$bugs_database_name="bugs";
$bugs_database_host="localhost";
$bugs_read_only_user="";
$bugs_report_access_failures="";
$bugs_database_user_name="";
$bugs_allow_bug_deletion=0;
$bugs_modify_graphic="";
$bugs_form_submit_method="POST";

if(file_exists("bugs_local_config.php"))
	include("bugs_local_config.php");

if($bugs_report_destination=="")
	$bugs_debug("It was not defined the bug report mail message destination address!\n");

if($bugs_shared_include_file=="")
	$bugs_debug("It was not defined the shared code include file!\n");

if($bugs_default_locale==""
&& $bugs_current_locale=="")
	$bugs_debug("It was not defined the current locale!\n");

if(file_exists("bugs_app_config.php"))
	include("bugs_app_config.php");

if(!IsSet($bugs_problem_types)
|| GetType($bugs_problem_types)!="array")
	$bugs_debug("It was not defined the array of problem types!\n");

if($bugs_password_file=="")
	$bugs_debug("It was not defined the password file name!\n");

if($bugs_database_name=="")
	$bugs_debug("It was not defined the bugs database name!\n");

if($bugs_database_host=="")
	$bugs_debug("It was not defined the bugs database host address!\n");

if(!IsSet($bugs_versions)
|| GetType($bugs_versions)!="array"
|| count($bugs_versions)==0)
	$bugs_debug("It was not defined the array of application versions listed in the bug submit form!\n");

if(!IsSet($bugs_submit_information)
|| GetType($bugs_submit_information)!="array"
|| count($bugs_submit_information)==0)
	$bugs_debug("It was not defined the array of types of information to submit with bug reports!\n");


?>