<?
/*
 * bugs_mysql.php
 *
 * @(#) $Header$
 *
 */
 
class bugs_mysql_class
{
	var $database_hostname="";
	var $database_user="";
	var $database_password="";
	
	var $database_connection=0;

	Function Connect($database_name)
	{
		global $bugs_database_host;

		if($this->database_connection!=0)
		{
			$bugs_debug("The connection to the bugs database is already established!\n");
			return(0);
		}
		if(($this->database_connection=mysql_pconnect($this->database_hostname,$this->$database_user,$this->$database_password))<=0)
		{
			$bugs_debug("It was not possible to connect to the bugs database! (".mysql_error().")\n");
			return(0);
		}
		mysql_select_db($database_name,$this->database_connection);
		return($this->database_connection);
	}

	Function Query($query)
	{
		if($this->database_connection<=0)
			$bugs_debug("The connection to the bugs database is not yet established!\n");
		return(mysql_query($query,$this->database_connection));
	}

	Function Error()
	{
		return(mysql_error());
	}

	Function CreateDatabase($name)
	{
		if($this->database_connection==0)
		{
			if(($link=mysql_Connect($this->database_hostname,$this->database_user,$this->database_password))==0)
				return(0);
		}
		else
			$link=$this->database_connection;
		$success=mysql_create_db($name,$link);
		if($this->database_connection==0)
			mysql_Close($link);
		return($success);
	}

	Function Close()
	{
		if($this->database_connection!=0)
		{
			mysql_Close($this->database_connection);
			$this->database_connection=0;
		}
	}

	Function SeparateDateTime()
	{
		return(0);
	}

	Function DateFormat()
	{
		return("Y-m-d");
	}

	Function ScanDate($date)
	{
		$scanned_date["year"]=strtok($date,"-");
		$scanned_date["month"]=strtok("-");
		$scanned_date["day"]=strtok(" ");
		return($scanned_date);
	}

	Function TextFieldsNeedLength()
	{
		return(0);
	}

	Function SupportAutoIncrement()
	{
		return(1);
	}

	Function SupportSequences()
	{
		return(0);
	}

	Function FetchInsertId()
	{
		return(mysql_insert_id());
	}

	Function NumberOfRows($result)
	{
		return(mysql_num_rows($result));
	}

	Function FreeResult($result)
	{
		return(mysql_free_result($result));
	}

	Function FetchNextRow($result)
	{
		return(mysql_fetch_row($result));
	}

	Function NumberOfFields($result)
	{
		return(mysql_num_fields($result));
	}

	Function FetchNextField($result)
	{
		return(mysql_fetch_field($result));
	}

	Function FetchArray($result)
	{
		return(mysql_fetch_array($result));
	}

	Function FetchResult($result,$row,$field)
	{
		return(mysql_result($result,$row,$field));
	}
};
 
?>