<?
/*
 * bugs_database.php
 *
 * @(#) $Header$
 *
 */
 
switch($bugs_database)
{
	case "msql";
		$bugs_database_include="bugs_msql.php";
		$bugs_database_class_name="bugs_msql_class";
		break;
	case "mysql";
		$bugs_database_include="bugs_mysql.php";
		$bugs_database_class_name="bugs_mysql_class";
		break;
	case "pgsql";
		$bugs_database_include="bugs_pgsql.php";
		$bugs_database_class_name="bugs_pgsql_class";
		break;
	default:
		$bugs_debug("It was not defined the database the bugs application will work with!\n");
}
if(file_exists($bugs_application_path.$bugs_database_include))
{
	include($bugs_application_path.$bugs_database_include);
}
else
	$bugs_debug("Could not include bugs database class file \"$bugs_database_include\"!\n");
$bugs_database_object=new $bugs_database_class_name;
if($bugs_database_host!="")
	$bugs_database_object->database_hostname=$bugs_database_host;

Function BugsDatabaseConnect($database)
{
	global $bugs_database_object;

	return($bugs_database_object->Connect($database));
}

Function BugsDatabaseClose()
{
	global $bugs_database_object;

	$bugs_database_object->Close();
}

Function BugsDatabaseError()
{
	global $bugs_database_object;

	return($bugs_database_object->Error());
}

Function BugsDatabaseQuery($query)
{
	global $bugs_database_object,$bugs_project_debug;

	$result=$bugs_database_object->Query($query);
	if($bugs_project_debug)
	{
		echo "Query - \"$query\"\n";
		if($result<=0)
			echo "Result - \"$result\" , Error - \"",BugsDatabaseError(),"\"\n";
	}
	return($result);
}

Function BugsDatabaseDateFormat()
{
	global $bugs_database_object;

	return($bugs_database_object->DateFormat());
}

Function BugsDatabaseScanDate($date)
{
	global $bugs_database_object;

	return($bugs_database_object->ScanDate($date));
}

Function BugsDatabaseSeparateDateTime()
{
	global $bugs_database_object;

	return($bugs_database_object->SeparateDateTime());
}

Function BugsDatabaseSupportAutoIncrement()
{
	global $bugs_database_object;

	return($bugs_database_object->SupportAutoIncrement());
}

Function BugsDatabaseSupportSequences()
{
	global $bugs_database_object;

	return($bugs_database_object->SupportAutoIncrement());
}

Function BugsDatabaseTextFieldsNeedLength()
{
	global $bugs_database_object;

	return($bugs_database_object->TextFieldsNeedLength());
}

Function BugsDatabaseNumberOfRows($result)
{
	global $bugs_database_object;

	return($bugs_database_object->NumberOfRows($result));
}

Function BugsDatabaseFreeResult($result)
{
	global $bugs_database_object;

	return($bugs_database_object->FreeResult($result));
}

Function BugsDatabaseFetchNextRow($result)
{
	global $bugs_database_object;

	return($bugs_database_object->FetchNextRow($result));
}

Function BugsDatabaseNumberOfFields($result)
{
	global $bugs_database_object;

	return($bugs_database_object->NumberOfFields($result));
}

Function BugsDatabaseFetchNextField($result)
{
	global $bugs_database_object;

	return($bugs_database_object->FetchNextField($result));
}

Function BugsDatabaseFetchArray($result)
{
	global $bugs_database_object;

	return($bugs_database_object->FetchArray($result));
}

Function BugsDatabaseFetchResult($result,$row,$field)
{
	global $bugs_database_object;

	return($bugs_database_object->FetchResult($result,$row,$field));
}

Function BugsDatabaseFetchInsertId()
{
	global $bugs_database_object;

	return($bugs_database_object->FetchInsertId());
}

?>