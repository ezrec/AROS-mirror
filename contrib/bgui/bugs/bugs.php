<?
/*
 * bugs.php
 *
 * @(#) $Header$
 *
 */

require($bugs_application_path."bugs_globals.php");
require($bugs_application_path."bugs_locale.php");
require($bugs_application_path."bugs_database.php");
require($bugs_shared_include_file);
commonHeader(LocaleText("Bug-Reporting"));
echo "<font size=-1>\n";

function indent($string, $prefix) {
	$string = ereg_replace(13, "", $string); /* get rid of Ctrl-M */
	return $prefix . ereg_replace("\n", "\n$prefix", $string) . "\n";
}

function show_state_options($state, $show_all) {
	global $bugs_allow_bug_deletion;

	if ($state) { echo "<option value=\"$state\">",LocaleText($state),"\n"; }
	if($state!="Open") { echo "<option value=\"Open\">",LocaleText("Open"),"\n"; }
	if($state!="Closed") { echo "<option value=\"Closed\">",LocaleText("Closed"),"\n"; }
	if($state!="Assigned") { echo "<option value=\"Assigned\">",LocaleText("Assigned"),"\n"; }
	if($state!="Analyzed") { echo "<option value=\"Analyzed\">",LocaleText("Analyzed"),"\n"; }
	if($state!="Suspended") { echo "<option value=\"Suspended\">",LocaleText("Suspended"),"\n"; }
	if($state!="Duplicate") { echo "<option value=\"Duplicate\">",LocaleText("Duplicate"),"\n"; }
	if($state!="Delete!" && !$show_all && $bugs_allow_bug_deletion) { echo "<option value=\"Delete!\">",LocaleText("Deleted"),"\n"; }
	if($state!="All" && $show_all) { echo "<option value=\"All\">",LocaleText("All"),"\n"; }
}

function show_menu($state) {
	global $PHP_SELF, $bug_type,$bugs_form_submit_method;

	if(!isset($bug_type)) { $bug_type="Any"; }
	echo "<form method=$bugs_form_submit_method action=\"$PHP_SELF\">\n";
	echo "<input type=hidden name=cmd value=\"Display Bugs\">\n";
	echo "<center><table bgcolor=#aabbcc cellspacing=0><tr><td><input type=submit value=\"",LocaleText("Display"),"\"></td><td><select name=\"status\">\n";
	show_state_options($state, 1);
	echo "</select></td><td align=right>",LocaleText("Bugs-of-type"),": </td><td>";
	show_types($bug_type,1,"bug_type");

	$fields = array( "id" => LocaleText("Bug-ID"),
					"bug_type" => LocaleText("Bug-Type"),
					"email" => LocaleText("Email-address"),
					"sdesc" => LocaleText("Short-Description"),
					"ldesc" => LocaleText("Long-Description"),
					"program_version" => LocaleText("Version"),
					"os" => LocaleText("Platform"),
					"status" => LocaleText("Status"),
					"comments" => LocaleText("Comments"),
					"ts1" => LocaleText("TimeStamp-1"),
					"ts2" => LocaleText("Timestamp-2"),
					"assign" => LocaleText("Assigned"));

	reset($fields);
	echo "</td><td align=right>",LocaleText("Order-by"),":</td><td> <select name='order_by_clause'>\n";
	while(list($field,$name) = each($fields)) {
		echo "<option value='$field'>$name\n";
	}
	echo "</select></td><td> <a href=\"bugs_statistics.html\">",LocaleText("Statistics"),"</a></td></tr>\n";
	echo "<tr><td colspan=3 align=right>",LocaleText("Where-the-bug-description-contains"),":</td>\n";
	echo "<td colspan=4><input type=text name=\"search_for\"></td></tr>\n";
	echo "</table></center>\n";
	echo "<i><center>",LocaleText("Feature-Change-requests-must-be-explicitly-selected-to-be-shown"),"</center></i>\n";
	echo "</form>\n";

}

function show_types($first_item,$show_any,$var_name) {
	global $bugs_problem_types;

	echo "<select name=\"$var_name\">\n<option value=\"$first_item\">",LocaleText($first_item),"\n";
	if($first_item!="Any") {
		if($show_any) {
			echo "<option value=\"Any\">",LocaleText("Any"),"\n";
		}
	}
	for($i=0;$i<count($bugs_problem_types);$i++) {
		if($first_item!=$bugs_problem_types[$i]) {
			if($show_any || $bugs_problem_types[$i]!="Any") {
				echo "<option value=\"",$bugs_problem_types[$i],"\">",LocaleText($bugs_problem_types[$i]),"\n";
			}
		}
	}
	echo "</select>\n";
}

function find_password($user) {
	global $bugs_password_file,$bugs_debug;

	if(($fp=@fopen($bugs_password_file,"r"))==0)
	{
		$bugs_debug("Could not open the password file!\n");
		return("");
	}
	while(!feof($fp)) {
		$line=fgets($fp,120);
		$luser=strtok($line,":");
		$passwd=strtok(":\n");
		if($user==$luser) {
			fclose($fp);
			return($passwd);
		}
	}
	fclose($fp);
	return("");
}

function addlinks($text) {
	$text = htmlspecialchars($text);
	$new_text = ereg_replace("(http:[^ \n\t]*)","<a href=\"\1-=-\">\1</a>",$text);
	$new_text = ereg_replace("(ftp:[^ \n\t]*)","<a href=\"\1-=-\">\1</a>",$text);
	$new_text = ereg_replace("[.,]-=-\"","\"",$new_text);
	$new_text = ereg_replace("-=-\"","\"",$new_text);
	return $new_text;
}

if (isset($cmd) && $cmd == "Send bug report") {
	if(!ereg("@",$email)) {
		echo LocaleText("Please-provide-a-valid-email-address"),"<P>\n";
		commonFooter();
		exit;
	}

	if($ebug_type=="-Please-Select-") {
		echo LocaleText("Please-select-an-appropriate-bug-type"),"<P>\n";
		commonFooter();
		exit;
	}

	show_menu(strtok($status," "));
	echo "<hr>\n";

	if((BugsDatabaseConnect($bugs_database_name))<=0)
		$bugs_debug("Could not connect to the database server!");
	$ts_date=date(BugsDatabaseDateFormat());
	$ts_time=date("H:i:s");
	if(BugsDatabaseSeparateDateTime())
		$ts="'$ts_date', '$ts_time'";
	else
		$ts="'$ts_date $ts_time'";
	if(BugsDatabaseSupportAutoIncrement())
		$cid=0;
	else
	{
		if(($result=BugsDatabaseQuery("SELECT id FROM serial_numbers"))<=0)
			$bugs_debug("It was not possible to query the bugs database! (".BugsDatabaseError().")\n");
		$cid=BugsDatabaseFetchResult($result,0,"id")+1;
		if(BugsDatabaseQuery("UPDATE serial_numbers SET id=".$cid)<=0)
			$bugs_debug("It was not possible to query the bugs database! (".BugsDatabaseError().")\n");
	}
	if(BugsDatabaseQuery("INSERT into bugdb values ($cid,'$ebug_type','$email','$sdesc','$ldesc','$program_version','$os','Open','', $ts, $ts,'','')")<=0)
		$bugs_debug("It was not possible to query the bugs database! (".BugsDatabaseError().")\n");
	if(BugsDatabaseSupportAutoIncrement())
		$cid=BugsDatabaseFetchInsertId();

	$report = "";
	echo("<pre>\n");

	$ldesc = stripslashes($ldesc);
	$sdesc = stripslashes($sdesc);
	$report .= LocaleText("From").":\t$email\n";
	$report .= LocaleText("Platform").":\t$os\n";
	$report .= LocaleText("Version").":\t".LocaleText($program_version)."\n";
	$report .= LocaleText("Bug-Type").":\t".LocaleText($ebug_type)."\n";
	$report .= LocaleText("Bug-description").":\t\n";
	$ascii_report = indent($report.$ldesc,"");
	$html_desc = ereg_replace("<", "&lt;", $ldesc);
	$html_desc = ereg_replace(">", "&gt;", $html_desc);
	$report .= indent($html_desc, "    ");

	$html_report = ereg_replace("<", "&lt;", $report);
	$html_report = ereg_replace(">", "&gt;", $html_report);

	echo $html_report;

	echo("</pre>\n");

	if (Mail($bugs_report_destination, LocaleText("Bug")."#$cid: $sdesc", $ascii_report, "From: $email")) {
		echo "<h2>",sprintf(LocaleText("Mail-sent-to"),$bugs_report_destination),"</h2>\n";
		echo "<P>",LocaleText("Thank-you-for-your-help"),"</P>";
		$url="http://".getenv("SERVER_NAME").$PHP_SELF."?id=$cid";
		echo "<P>",sprintf(LocaleText("bug-submit-message"),"<A HREF=\"$url\">$url</A>"),"</P>";
	} else {
		 echo "<h2>",LocaleText("Mail-not-sent"),"</h2>\n";
		 echo "<P>",sprintf(LocaleText("Please-send-this-page-to"),"<A HREF=\"mailto:$email\">$email</a>"),"/P>";
	}

} elseif(isset($cmd) && $cmd=="Display Bugs") {
	show_menu(strtok($status," "));
	echo "<hr>\n";

	include($bugs_application_path."bugs_table_wrapper.php");

	function external_processing($fieldname,$tablename,$data,$row)
	{
		global $PHP_SELF,$bugs_modify_graphic;

		switch($fieldname) {
			case "id":
				print "<a href=\"$PHP_SELF?id=$data\">$data</a>\n";	
				break;
			case "Originator":
				print "<a href=\"mailto:$data\">$data</a>\n";
				break;
			case "Modify":
				print "<a href=\"$PHP_SELF?id=${row[id]}&edit=1\">";
				if($bugs_modify_graphic!="")
					echo "<img src=\"$bugs_modify_graphic\" border=\"0\">";
				else
					echo "~";
				echo "</a>\n";
				break;
			case "bug_type":
			case "status":
			case "program_version":
				echo LocaleText(strtok($data," "));
				break;
			default:
				$data = ereg_replace("<","&lt;",$data);
				$data = ereg_replace(">","&gt;",$data);
				print $data;
				break;
		}
	}
	function row_coloring($row) {
		if ($row["bug_type"]=="Feature/Change Request") {
			return "#aaaaaa";
		}
		switch($row["Status"]) {
			case "Open":
				return "#ffbbaa";
				break;
			case "Closed":
				return "#aaffbb";
				break;
			case "Suspended":
				return "#ffccbb";
				break;
			case "Assigned":
				return "#bbaaff";
				break;
			case "Analyzed":
				return "#99bbaa";
				break;
			case "Duplicate":
				return "#bbbbbb";
				break;
			default:
				return "#aaaaaa";
				break;
		}
	}
	$external_processing_function="external_processing";
	$row_coloring_function="row_coloring";

	if((BugsDatabaseConnect($bugs_database_name))<=0)
	{
		$bugs_debug("Could not connect to the database server!");
	}

	$tables[] = "bugdb";
	$fields[] = "id";
	$fields[] = "bug_type";
	$fields[] = "status";
	$fields[] = "assign";
	$fields[] = "program_version";
	$fields[] = "os";
	$fields[] = "email";
	$fields[] = "sdesc";
	$conversion_table=array(
		"id"=>LocaleText("ID#"),
		"bug_type"=>LocaleText("Bug-Type"),
		"status"=>LocaleText("Status"),
		"assign"=>LocaleText("Assigned"),
		"program_version"=>LocaleText("Version"),
		"os"=>LocaleText("Platform"),
		"email"=>LocaleText("Email-address"),
		"sdesc"=>LocaleText("Short-Description"));
	$pass_on = ereg_replace(" ","+","&cmd=Display+Bugs&status=$status&bug_type=$bug_type");
	$default_header_color="aaaaaa";
	$centering["id"] = $centering["Modify"] = $centering["bug_type"] = $centering["status"] = $centering["program_version"] = "center";
	$dont_link["Modify"]=1;

	if (!isset($order_by_clause)) {
		$order_by_clause = "id";
	}
	if($status=="All" && $bug_type=="Any") {
		$where_clause = "bug_type<>'Feature/Change Request'";
		/* nothing */
	} elseif($status=="All" && $bug_type!="Any") {
		$where_clause = "bug_type='$bug_type'";
	} elseif($status!="All" && $bug_type=="Any") {
		/* Treat assigned and analyzed bugs as open */
		if($status=="Open") {
			$where_clause = "bug_type<>'Feature/Change Request' and (status='Open' or status='Assigned' or status='Analyzed')";
		} else {
			$where_clause = "status='$status' and bug_type<>'Feature/Change Request'";
		}
	} else {
		/* Treat assigned and analyzed bugs as open */
		if($status=="Open") {
			$where_clause = "(status='Open' or status='Assigned' or status='Analyzed') and bug_type='$bug_type'";
		} else {
			$where_clause = "status='$status' and bug_type='$bug_type'";
		}
	}
	if(strlen($search_for)) {
		$where_clause .= " and (sdesc like '%$search_for%' or ldesc like '%$search_for%' or comments like '%$search_for%')";
	}
	table_wrapper();
	echo "<br><center><a href=\"$PHP_SELF\">",LocaleText("Submit-Bug-Report"),"</a></center>\n";
} else if(!isset($cmd) && isset($id)) {
	show_menu($status);
	echo "<hr>\n";

	if((BugsDatabaseConnect($bugs_database_name))<=0)
	{
		$bugs_debug("Could not connect to the database server!");
	}
	if(isset($modify) && $modify=="Edit Bug") {
		$ok=0;
		if($user!=$bugs_read_only_user) {
			$psw=find_password($user);
			if(strlen($psw)>0) {
				if(crypt($pw,substr($psw,0,2))==$psw) {
					if($estatus=="Delete!") {
						$ok=BugsDatabaseQuery("DELETE from bugdb where id=$id");
					} else {
						$ts_date=date(BugsDatabaseDateFormat());
						$ts_time=date("H:i:s");
						if(BugsDatabaseSeparateDateTime())
							$ts="ts2_date='$ts_date', ts2_time='$ts_time'";
						else
							$ts="ts2='$ts_date $ts_time'";
						$ok=BugsDatabaseQuery("UPDATE bugdb set status='$estatus', bug_type='$ebug_type', assign='$eassign', comments='$comments', $ts, dev_id='$user' where id=$id");
					}
					if(!$ok)
						$bugs_debug("It was not possible to update the bugs database! (".BugsDatabaseError().")\n");
				}
			}
		}
		if(!$ok) {
			echo "<b>",LocaleText("Incorrect-user-password"),"</b><br>\n";
			if($bugs_report_access_failures!="")
				Mail($bugs_report_access_failures, sprintf(LocaleText("bugdb-access-failure-message-subject"),$user,$pw), "", "From: bugdb");
		} else {
			echo "<b>",LocaleText("Database-updated"),"</b><br>\n";
			if($estatus=="Delete!") {
				$text = sprintf(LocaleText("Bug-has-been-deleted"),$id,$user,stripslashes($comments));
			} else {
				$text = stripslashes(sprintf(LocaleText("Bug-has-been-updated"),$id,$user,$eemail,$estatus,$ebug_type,$eassign,$comments,"http://".getenv("SERVER_NAME").$PHP_SELF."?id=$id"));
			}
			$esdesc = stripslashes($esdesc);
			$subject=sprintf(LocaleText("Bug-updated"),$id,$esdesc);
			$from="From: $bugs_database_user_name <$bugs_report_destination>";
			Mail($eemail, $subject, $text, $from);
			Mail($bugs_report_destination, $subject, $text, $from);
		}
	}
	if(BugsDatabaseSeparateDateTime())
	{
		$date_time="ts1_date,ts1_time,ts2_date,ts2_time";
		$assigned_column=14;
		$dev_id_column=13;
	}
	else
	{
		$date_time="ts1,ts2";
		$assigned_column=12;
		$dev_id_column=11;
	}
	if(($result=BugsDatabaseQuery("SELECT id,bug_type,email,sdesc,ldesc,program_version,os,status,comments,$date_time,dev_id,assign from bugdb where id=$id"))<=0)
		$bugs_debug("It was not possible to query the bugs database! (".BugsDatabaseError().")\n");
	if(BugsDatabaseNumberOfRows($result)>0) {
		$row = BugsDatabaseFetchNextRow($result);	
		echo "<br><h1>",sprintf(LocaleText("Bug-id-#"),$id),"</h1>\n";
		if(BugsDatabaseSeparateDateTime())
		{
			$ts1=$row[9]." ".$row[10];
			$ts1=$row[11]." ".$row[12];
		}
		else
		{
			$ts1=$row[9];
			$ts1=$row[10];
		}
		echo "<table>\n";
		if(!isset($edit)) {
			echo "<tr><th align=right>",LocaleText("Status"),":</th><td>".LocaleText(strtok($row[7]," "))."</td>";
			echo "<td><a href=\"$PHP_SELF?id=$id&edit=1\"><font size=-1><tt>",LocaleText("Modify"),"</tt></font></a></td>";
		} else {
			echo "<form method=$bugs_form_submit_method action=\"$PHP_SELF\">\n";
			echo "<input type=hidden name=id value=\"$id\">\n";
			echo "<input type=hidden name=modify value=\"Edit Bug\">\n";
			echo "<tr><th align=right>",LocaleText("Status"),":</th><td><select name=\"estatus\">\n";
			show_state_options(strtok($row[7]," "), 0);
			echo "</select>\n";
			echo LocaleText("Assign-to"),": <input type=text name=eassign value=\"$row[$assigned_column]\">\n";
		}
		echo "</tr>\n";
		echo "<tr><th align=right>",LocaleText("From"),":</th><td><a href=\"mailto:".$row[2]."\">".$row[2]."</a>";
		echo "<input type=hidden name=eemail value=\"$row[2]\"></td></tr>\n";
		echo "<tr><th align=right>",LocaleText("Date"),":</th><td>".$ts1."</td></tr>\n";
		if(!isset($edit)) {
			echo "<tr><th align=right>",LocaleText("Bug-Type"),":</th><td>".LocaleText(strtok($row[1]," "))."</td></tr>\n";
		} else {
			echo "<tr><th align=right>",LocaleText("Bug-Type"),":</th><td>\n";
			show_types(strtok($row[1]," "),0,"ebug_type");
			echo "</td></tr>\n";
		}
		echo "<tr><th align=right>",LocaleText("Platform"),":</th><td>".$row[6]."</td></tr>\n";
		echo "<tr><th align=right>",LocaleText("Version"),":</th><td></b>".LocaleText(strtok($row[5]," "))."</td></tr>\n";
		echo "<tr><th align=right>",LocaleText("Assigned-to"),":</th><td></b>".$row[$assigned_column]."</td></tr>\n";
		$sd = ereg_replace("<","&lt;",$row[3]);
		$sd = ereg_replace(">","&gt;",$sd);
		echo "<tr><th align=right>",LocaleText("Short-Description"),":</th><td></b>$sd<input type=hidden name=esdesc value=\"$row[3]\"></td></tr>\n";
		echo "</table>\n";
		$text = addlinks($row[4]);
		echo "<blockquote><blockquote><pre>".$text."</pre></blockquote></blockquote>\n";
		if(!isset($edit)) {
			if(strlen($row[8])) {
				echo "<b><i>[".$ts2."] ",LocaleText("Updated-by")," ".$row[$dev_id_column]."</i></b><br>\n";
				$text=addlinks($row[8]);
				echo "<b><pre>".$text."</pre></b>\n";
			}
		} else {
			echo "<b><tt>",LocaleText("Developer-Comments"),":</tt></b><br>\n";
			echo "<textarea cols=60 rows=15 name=\"comments\">".$row[8]."</textarea><br>\n";
			echo LocaleText("User-id"),": <input type=text size=10 name=user>\n";
			echo LocaleText("Password"),": <input type=password size=10 name=pw>\n";
			echo "<input type=submit value=\"",LocaleText("Commit-Changes"),"\">\n";
			echo "</form>\n";
		}
	} else {
		if(isset($modify) && $estatus=="Delete!") {
			echo "<br><h1>",sprintf(LocaleText("Bug-was-deleted"),$id),"</h1>\n";
		} else {
			echo "<br><h1>",sprintf(LocaleText("Bug-does-not-exist"),$id),"</h1>\n";
		}
	}
	BugsDatabaseFreeResult($result);
} else {
	show_menu(strtok($status," "));
	echo LocaleText("use-the-form-below");
?>
<hr>
<form method=$bugs_form_submit_method action="<? echo $PHP_SELF;?>">
<input type=hidden name=cmd value="Send bug report">
<?
	echo LocaleText("message-before-bug-report"),"<P>",LocaleText("more-than-one-bug"),"</P>";
?>
<table>
 <tr>
  <th align=right><? echo LocaleText("Your-email-address"); ?>:</th>
  <td colspan="2">
   <input type=text size=20 name="email" value="<?if(isset($email)) { echo $email; }?>">
  </td>
 </tr><tr>
  <th align=right><? echo LocaleText("Version"); ?>:</th>
  <td>
   <select name="program_version">
<?
		for(Reset($bugs_versions),$version=0;$version<count($bugs_versions);$version++,Next($bugs_versions))
			echo "<option value=\"",$bugs_versions[Key($bugs_versions)],"\">",LocaleText($bugs_versions[Key($bugs_versions)]),"</option>\n";
?>
   </select>
  </td>
<?
	if(($message=LocaleText("message-after-version"))!="")
	{
?>
  <td>
   <font size="-2">
   <? echo $message; ?>
   </font>
  </td>
<?
	}
?>
 </tr><tr>
  <th align=right><? echo LocaleText("Bug-Type"); ?>:</th>
  <td colspan="2">
	<?show_types("-Please-Select-",0,"ebug_type")?>
   </td>
  </tr><tr>
  <th align=right><? echo LocaleText("Platform"); ?>:</th>
  <td colspan="2">
   <input type=text size=20 name="os" value="<?echo isset($operating_system)?$operating_system:"";?>">
  </td>
 </tr><tr>
  <th align=right><? echo LocaleText("Bug-description"); ?>:</th>
  <td colspan="2">
   <input type=text size=40 maxlength=79 name="sdesc">
  </td>
</table>

<table>
<tr>
<td valign="top">
<? echo LocaleText("Please-supply-any-information"); ?>
<ul>
<?
		for(Reset($bugs_submit_information),$information=0;$information<count($bugs_submit_information);$information++,Next($bugs_submit_information))
			echo "<li>",LocaleText($bugs_submit_information[Key($bugs_submit_information)]),"</li>\n";
?>
</ul>
<center>
<input type=submit value="<? echo LocaleText("Send-bug-report"); ?>">
</center>
</td>
<td>
<textarea cols=60 rows=15 name="ldesc" wrap=virtual></textarea>
</td>
</tr>
</table>

<p>

</form>

<? } ?>
</font>
<?
commonFooter();
?>
