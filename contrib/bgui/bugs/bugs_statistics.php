<?
/*
 * bugs_statistics.php
 *
 * @(#) $Header$
 *
 */

require($bugs_application_path."bugs_globals.php");
require($bugs_application_path."bugs_locale.php");
require($bugs_application_path."bugs_database.php");
require($bugs_shared_include_file);

	function mydate($date,$time) {
		$scanned_date=BugsDatabaseScanDate($date);
		$year = $scanned_date["year"];
		$month = $scanned_date["month"];
		$day = $scanned_date["day"];
		$hour = substr($time,0,2);
		$min = substr($time,3,2);
		$sec = substr($time,6,2);
		return mktime($hour,$min,$sec,$month,$day,$year);
	}

	function ShowTime($sec) {
		if($sec==1) {
			return sprintf(LocaleText("1-second"),$sec);
		} else if($sec<60) {
			return sprintf(LocaleText("n-seconds"),$sec);
		} else if($sec<120) {
			return sprintf(LocaleText("1-minute-n-seconds"),$sec%60);
		} else if($sec<3600) {
			return sprintf(LocaleText("n-minutes-n-seconds"),(int)($sec/60),($sec%60));
		} else if($sec<7200) {
			return sprintf(LocaleText("1-hour-n-minutes-n-seconds"),(int)(($sec%3600)/60),sec%60);
		} else if($sec<86400) {
			return sprintf(LocaleText("n-hours-n-minutes-n-seconds"),(int)($sec/3600),(int)(($sec%3600)/60),(($sec%3600)%60));
		} else if($sec<172800) {
			return sprintf(LocaleText("1-day-n-hours-n-minutes-n-seconds"),(int)($sec/3600),(int)(($sec%3600)/60),(($sec%3600)%60));
		} else {
			return sprintf(LocaleText("n-days-n-hours-n-minutes-n-seconds"),(int)($sec/86400),(int)($sec/3600),(int)(($sec%3600)/60),(($sec%3600)%60));
		}
	}

	commonHeader(LocaleText("Bug-Statistics"));

	if((BugsDatabaseConnect($bugs_database_name))<=0)
		$bugs_debug("Could not connect to the database server!");
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
	if(($result=BugsDatabaseQuery("SELECT id,bug_type,email,sdesc,ldesc,program_version,os,status,comments,$date_time,dev_id,assign from bugdb"))<=0)
		$bugs_debug("It was not possible to query the bugs database! (".BugsDatabaseError().")\n");
	if(BugsDatabaseNumberOfRows($result)==0)
	{
		echo "<H1><CENTER>",LocaleText("There-are-no-bug-reports-to-produce-statistics"),"</CENTER></H1>";
		BugsDatabaseFreeResult($result);
	}
	else
	{
		echo "<H1><CENTER>",LocaleText("Bug-Report-Statistics"),"</CENTER></H1>\n<HR>\n";
		for($bug_type=$email=$closed_by=$time_to_close=array();$row=BugsDatabaseFetchNextRow($result);) {
			$bug_type['all'][$row[1]]++;
			$bug_status=strtok($row[7]," ");
			if($bug_status=="Open") {
				$bug_type['open'][$row[1]]++;
			}
			if($bug_status=="Analyzed") {
				$bug_type['analyzed'][$row[1]]++;
			}
			if($bug_status=="Suspended") {
				$bug_type['suspended'][$row[1]]++;
			}
			if($bug_status=="Duplicate") {
				$bug_type['duplicate'][$row[1]]++;
			}
			if($bug_status=="Assigned") {
				$bug_type['assigned'][$row[1]]++;
			}
			$email[$row[2]]++;
			$program_version[$row[5]]++;
			$os[$row[6]]++;
			$status[$bug_status]++;
			if($bug_status=="Closed") {
				$bug_type['closed'][$row[1]]++;
				if(BugsDatabaseSeparateDateTime())
				{
					$date1=$row[9];
					$time1=$row[10];
					$date2=$row[11];
					$time2=$row[12];
					$closed_by[$row[13]]++;
				}
				else
				{
					$date1=strtok($row[9]," ");
					$time1=strtok(" ");
					$date2=strtok($row[10]," ");
					$time2=strtok(" ");
					$closed_by[$row[11]]++;
				}
				$time_to_close[] = mydate($date2,$time2) - mydate($date1,$time1);
			}
			$total++;
		}

		function bugstats($status, $type) {
			global $bug_type,$PHP_SELF;
			if ($bug_type[$status][$type] > 0) {
				return '<A href=$PHP_SELF."?cmd=Display+Bugs&status=' . ucfirst($status) . '&bug_type=' . urlencode($type) . '">' . $bug_type[$status][$type] . "</A>\n";
			}
		}

		BugsDatabaseFreeResult($result);
		if(IsSet($bug_type['all']))
		{
			echo "<center><table border>\n";
			echo "<tr bgcolor=#aabbcc><th align=right>",LocaleText("Total-bug-entries"),":</th><td>$total</td><th>",LocaleText("Closed"),"</th><th>",LocaleText("Open"),"</th><th>",LocaleText("Analyzed"),"</th><th>",LocaleText("Suspended"),"</th><th>",LocaleText("Duplicate"),"</th><th>",LocaleText("Assigned"),"</th></tr>\n";
			while(list($type,$value)=each($bug_type['all'])) {
				$type=strtok($type," ");
				echo "<tr><th align=right bgcolor=#aabbcc>",LocaleText(strtok($type," ")),":</th><td align=center bgcolor=#ccddee>$value</td><td align=center bgcolor=#ddeeff>".bugstats('closed', $type)."&nbsp;</td><td align=center bgcolor=#ccddee>".bugstats('open', $type)."&nbsp;</td><td align=center bgcolor=#ddeeff>".bugstats('analyzed', $type)."&nbsp;</td><td align=center bgcolor=#ccddee>".bugstats('suspended',$type)."&nbsp;</td><td align=center bgcolor=#ddeeff>".bugstats('duplicate', $type)."&nbsp;</td><td align=center bgcolor=#ccddee>".bugstats('assigned',$type)."&nbsp;</td></tr>\n";
			}
			echo "</table></center><hr>\n";
		}
	
		if(($c=count($time_to_close))!=0)
		{
			sort($time_to_close);
			$sum=0;
			for($i=0;$i<$c;$i++) {
				$sum+=$time_to_close[$i];
			}
			$median = $time_to_close[(int)($c/2)];
			$slowest=$time_to_close[$c-1];
			$quickest=$time_to_close[0];
			echo "<h2><center>",LocaleText("Bug-Report-Time-to-Close-Statistics"),"</center></h2>\n";
			echo "<center><table border>\n";
			echo "<tr bgcolor=#aabbcc><th align=right>",LocaleText("Average-life-of-a-report"),":</th><td bgcolor=#ccddee>".ShowTime($c==0 ? 0 : (int)($sum/$c))."</td></tr>\n";
			echo "<tr bgcolor=#aabbcc><th align=right>",LocaleText("Median-life-of-a-report"),":</th><td bgcolor=#ccddee>".ShowTime($median)."</td></tr>\n";
			echo "<tr bgcolor=#aabbcc><th align=right>",LocaleText("Slowest-report-closure"),":</th><td bgcolor=#ccddee>".ShowTime($slowest)."</td></tr>\n";
			echo "<tr bgcolor=#aabbcc><th align=right>",LocaleText("Quickest-report-closure"),":</th><td bgcolor=#ccddee>".ShowTime($quickest)."</td></tr>\n";
			echo "</table></center><hr>\n";
			arsort($closed_by);
			echo "<h2><center>",LocaleText("Who-is-closing-the-bug-reports"),"</center></h2>\n";
			echo "<center><table border>\n";
			while(list($who,$value)=each($closed_by)) {
				echo "<tr bgcolor=#aabbcc><th>$who</th><td bgcolor=#ccddee>$value</td></tr>\n";
			}
			echo "</table></center><hr>\n";
		}

		if(count($email))
		{
			arsort($email);
			echo "<h2><center>",LocaleText("Who-is-submitting-bug-reports"),"</center></h2>\n";
			echo "<center><table border>\n";
			while(list($who,$value)=each($email)) {
				echo "<tr bgcolor=#aabbcc><th>$who</th><td bgcolor=#ccddee>$value</td></tr>\n";
			}
			echo "</table></center><hr>\n";
		}
	}

	commonFooter();
?>
