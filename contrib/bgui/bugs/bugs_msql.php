<?
/*
 * bugs_msql.php
 *
 * @(#) $Header$
 *
 */
 
class bugs_msql_class
{
	var $database_hostname="";
	var $database_connection=0;
	
	Function Connect($database_name)
	{
		global $bugs_database_host,$bugs_debug;

		if($this->database_connection!=0)
		{
			$bugs_debug("The connection to the bugs database is already established!\n");
			return(0);
		}
		if(($this->database_connection=msql_pconnect($this->database_hostname))<=0)
		{
			$bugs_debug("It was not possible to connect to the bugs database! (".msql_error().")\n");
			return(0);
		}
		msql_select_db($database_name,$this->database_connection);
		return($this->database_connection);
	}

	Function Query($query)
	{
		global $bugs_debug;

		if($this->database_connection<=0)
			$bugs_debug("The connection to the bugs database is not yet established!\n");
		return(msql_query($query,$this->database_connection));
	}

	Function Error()
	{
		return(msql_error());
	}

	Function CreateDatabase($name)
	{
		if($this->database_connection==0)
		{
			if($this->database_hostname=="")
				$link=msql_Connect();
			else
				$link=msql_Connect($this->database_hostname);
			if($link==0)
				return(0);
		}
		else
			$link=$this->database_connection;
		$success=msql_create_db($name,$link);
		if($this->database_connection==0)
			msql_Close($link);
		return($success);
	}

	Function Close()
	{
		if($this->database_connection!=0)
		{
			msql_Close($this->database_connection);
			$this->database_connection=0;
		}
	}

	Function SeparateDateTime()
	{
		return(1);
	}

	Function DateFormat()
	{
		return("d-M-Y");
	}

	Function ScanDate($date)
	{
		$scanned_date["day"]=strtok($date,"-");
		$scanned_date["month"]=strtok("-");
		$scanned_date["year"]=strtok(" ");
		return($scanned_date);
	}
	Function TextFieldsNeedLength()
	{
		return(1);
	}

	Function SupportAutoIncrement()
	{
		return(0);
	}

	Function SupportSequences()
	{
		return(0);
	}

	Function NumberOfRows($result)
	{
		return(msql_num_rows($result));
	}

	Function FreeResult($result)
	{
		return(msql_free_result($result));
	}

	Function FetchNextRow($result)
	{
		return(msql_fetch_row($result));
	}

	Function NumberOfFields($result)
	{
		return(msql_num_fields($result));
	}

	Function FetchNextField($result)
	{
		return(msql_fetch_field($result));
	}

	Function FetchArray($result)
	{
		return(msql_fetch_array($result));
	}

	Function FetchResult($result,$row,$field)
	{
		return(msql_result($result,$row,$field));
	}
};

?>