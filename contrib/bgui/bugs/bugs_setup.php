<?
/*
 * bugs_setup.php
 *
 * @(#) $Header$
 *
 */

require("bugs_globals.php");
require("bugs_database.php");

set_time_limit(0);

echo ("Bugs database setup program\n\n");
echo ("This script will create the database and tables required to run\n");
echo ("the bugs database application.\n\n");

echo ("Creating the empty databases ................................. ");
if($bugs_database_object->CreateDatabase($bugs_database_name)==0)
{
	echo "\ncould not create \"$bugs_database_name\" database! (". BugsDatabaseError() .")\n";
	exit;
}
echo("done.\n");

if(BugsDatabaseConnect($bugs_database_name)==0)
{
	echo "\nCould not connect to the database server!\n";
	exit;
}

echo("\nSetting up the application database\n");
echo("-------------------------------------------------\n\n");

echo("Creating the bugdb table ..................................... ");
if(BugsDatabaseSeparateDateTime())
	$date_time="ts1_date date, ts1_time time, ts2_date date, ts2_time time";
else
	$date_time="ts1 date, ts2 date";
if(BugsDatabaseTextFieldsNeedLength())
	$length="(80)";
else
	$length="";
if(BugsDatabaseSupportAutoIncrement())
{
	$auto_increment=" NOT NULL AUTO_INCREMENT";
	$primary_key=", PRIMARY KEY (id)";
}
else
{
	$auto_increment="";
	$primary_key="";
}
if(BugsDatabaseQuery("CREATE TABLE bugdb(id int$auto_increment, bug_type char(32), email char(40), sdesc char(80), ldesc text$length, program_version char(16), os char(32), status char(16), comments text$length, $date_time, dev_id char(16), assign char(16)$primary_key)")==0)
{
	echo "\nCouldn't create the bugs database table! (".BugsDatabaseError().")\n";
	exit;
}
echo("done.\n");

if(!BugsDatabaseSupportAutoIncrement())
{
	echo("Creating the serial_numbers table ............................ ");
	if(BugsDatabaseQuery("CREATE TABLE serial_numbers(id int)")==0)
	{
		echo "\nCouldn't create the serial_numbers database table! (".BugsDatabaseError().")\n";
		exit;
	}
	if(BugsDatabaseQuery("INSERT INTO serial_numbers (id) values(0)")==0)
	{
		echo "\nCouldn't initialize the serial_numbers table values! (".BugsDatabaseError().").\n";
		exit;
	}
	echo("done.\n");
}

echo("Creating the id index on bugdb table ......................... ");
if(BugsDatabaseQuery("CREATE UNIQUE INDEX id_index ON bugdb(id)")==0)
{
	echo "\nCouldn't create the the id_index index. (".BugsDatabaseError().")\n";
	exit;
}
echo("done.\n");

BugsDatabaseClose();

echo ("\nSetup complete.\n\n");



?>