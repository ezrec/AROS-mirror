<?
/*
 * bugs_pgsql.php
 *
 * @(#) $Header$
 *
 */
 
class bugs_pgsql_field_class
{
	var $name="";
	var $table="";

	Function SetFieldResult($result,$field)
	{
		$this->name=pg_fieldname($result,$field);
		$this->table="";
	}
}
 
class bugs_pgsql_class
{
	var $database_hostname="";
  var $database_hostport="";
	var $database_connection=0;
	var $result_fields=array();
	var $result_rows=array();
	
	Function Connect($database_name)
	{
		global $bugs_database_host,$bugs_debug;

		if($this->database_connection!=0)
		{
			$bugs_debug("The connection to the bugs database is already established!\n");
			return(0);
		}
		if(($this->database_connection=@pg_pconnect($this->database_hostname,$this->database_hostport,$database_name))<=0)
		{
			$bugs_debug("It was not possible to connect to the bugs database!\n");
			return(0);
		}
		return($this->database_connection);
	}

	Function Query($query)
	{
		global $bugs_debug;

		if($this->database_connection<=0)
			$bugs_debug("The connection to the bugs database is not yet established!\n");
		return(pg_Exec($this->database_connection,$query));
	}

	Function Error()
	{
		return(pg_errormessage($this->database_connection));
	}

	Function CreateDatabase($name)
	{
		if($this->database_connection==0)
		{
			if(($link=pg_Connect($this->database_hostname,$this->database_hostport,"template1"))==0)
				return(0);
		}
		else
			$link=$this->database_connection;
		$success=pg_Exec($link,"CREATE DATABASE $name");
		if($this->database_connection==0)
			pg_Close($link);
		return($success);
	}

	Function Close()
	{
		if($this->database_connection!=0)
		{
			pg_Close($this->database_connection);
			$this->database_connection=0;
		}
	}

	Function SeparateDateTime()
	{
		return(1);
	}

	Function DateFormat()
	{
		return("m-d-Y");
	}

	Function ScanDate($date)
	{
		$scanned_date["month"]=strtok($date,"-");
		$scanned_date["day"]=strtok("-");
		$scanned_date["year"]=strtok(" ");
		return($scanned_date);
	}

	Function TextFieldsNeedLength()
	{
		return(0);
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
		return(pg_NumRows($result));
	}

	Function FreeResult($result)
	{
		return(pg_FreeResult($result));
	}

	Function FetchRow($result,$current_row)
	{
		for($row=array(),$field=0;$field<pg_NumFields($result);$field++)
			$row[$field]=pg_result($result,$current_row,$field);
		for($field=0;$field<pg_NumFields($result);$field++)
			$row[pg_FieldName($result,$field)]=pg_result($result,$current_row,$field);
		return($row);
	}

	Function FetchNextRow($result)
	{
		if(!IsSet($this->result_rows["R$result"]))
			$this->result_rows["R$result"]=0;
		if($this->result_rows["R$result"]>=pg_NumRows($result))
			return(0);
		$current_row=$this->result_rows["R$result"]++;
		return($this->FetchRow($result,$current_row));
	}

	Function NumberOfFields($result)
	{
		return(pg_NumFields($result));
	}

	Function FetchNextField($result)
	{
		if(!IsSet($this->result_fields["R$result"]["Object"]))
		{
			$this->result_fields["R$result"]["Object"]=new bugs_pgsql_field_class;
			$this->result_fields["R$result"]["NextField"]=0;
		}
		if($this->result_fields["R$result"]["NextField"]>=pg_NumFields($result))
			return(0);
		$field=$this->result_fields["R$result"]["Object"];
		$field->SetFieldResult($result,$this->result_fields["R$result"]["NextField"]);
		$this->result_fields["R$result"]["NextField"]++;
		return($field);
	}

	Function FetchArray($result)
	{
		return($this->FetchNextRow($result));
	}

	Function FetchResult($result,$row,$field)
	{
		return(pg_result($result,$row,$field));
	}
};

?>