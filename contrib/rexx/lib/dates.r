/* dates.r */
arg day month year
days = 'Sunday Monday Tuesday Wednesday Thrusday Friday Saturday'
say word(days,DayOfWeek(day,month,year)+1)
say 'Easter = 'Easter(year)'/04/'year
say 'Days = 'DaysOfMonth(month,year)
say 'Dates Library'
exit

/* ------------------ DayOfWeek -------------------- */
/* return day of week 0=Sun, 1=Mon, ... 6=Sat  */
DayOfWeek: procedure
	if arg() = 3	then parse arg day,month,year
			else parse arg day '/' month '/' year
	if month < 3 then do
		month = month + 10
		year  = year - 1
	end;
	else month = month - 2
	century = year % 100
	Yr = year // 100
	Dw = (((26*Month - 2) % 10) + Day + Yr + (Yr % 4) +,
		(Century % 4) - (2 * Century)) // 7;
	if Dw < 0	then DayOfWeek = Dw + 7
			else DayOfWeek = Dw
return DayOfWeek

/* ------------------ Easter In April ----------------------- */
/* if Easter > 30 then Month = May, Easter = Easter - 30      */
/* this algoritm works only for Greek Orthodox Easter         */
Easter: procedure
	arg year
	a = year // 19;
	b = year // 4;
	c = year // 7;
	d = (19*a+16) // 30;
	e = (2*b + 4*c + 6*d) // 7;
return 3 + d + e;

/* -------------------- daysOfMonth ---------------------- */
/* With the correction of "Byron <bjhinkle@worldnet.att.net>"
 * for the leap years
 */
daysOfMonth: procedure
daysInMonth = '312831303130313130313031'
	if arg() = 2	then parse arg month,year
			else parse arg month '/' year
	Days = substr(DaysInMonth,Month*2-1,2)
	if Month=2 then
	if Year//4=0 & Year//100^=0 | Year//400=0
		then Days = 29
		else Days = 28

return Days

/* ------------------ addMonths ------------------- */
addMonths: procedure
	if arg() = 2 then parse arg day '/' month '/' year, months
		else parse arg day,month,year,months

	if months = '' then months = 1
	month = month + months
	if months > 0 then do
		do while month > 12
			month = month - 12
			year = year + 1
		end
	end
	else do
		do while month < 1
			month = month + 12
			year = year - 1
		end
	end
return day'/'month'/'year

/* ----------------------- addDays ------------------- */
addDays: procedure
	if arg() = 2 then parse arg day '/' month '/' year, days
			else parse arg day,month,year,days
	if days = '' then days = 1
	day = day + days
	if days > 0 then do
		do while day > DaysOfMonth(month,year)
			day = day - DaysOfMonth(month,year)
			parse value addMonths(day'/'month'/'year,1) with day '/' month '/' year
		end
	end
	else do
		do while day < 1
			parse value addMonths(day'/'month'/'year,-1) with day '/' month '/' year
			day = day + DaysOfMonth(month,year)
		end
	end
return day'/'month'/'year

/* -------------------- swapDate ----------------- */
swapDate: procedure
	if arg()=1 then parse arg day '/' month '/' year
		else parse arg day,month,year
return month'/'day'/'year

/***********************************************************************
 *                                                                     *
 * JULIAN -- Ordered to Julian and Julian to Ordered date conversion   *
 *                                                                     *
 *         (c) Eric Thomas 1986          (ERIC at FRECP11)             *
 *                                                                     *
 * This file was last updated on Saturday, June the 14th of 1986       *
 *                                                                     *
 *                                                                     *
 * This program will  convert a date from yy/mm/dd to  yyddd format or *
 *  vice-versa. It  can be invoked either  as a REXX function  or as a *
 *  command. The input  parameter is assumed to be  of correct syntax; *
 *  although some basic checks are performed, it is most probable that *
 *  a REXX error  will occur if the argument is  incorrect (eg 85999). *
 *                                                                     *
 * Examples:                                                           *
 *                                                                     *
 * (from CMS mode): Julian 84249                                       *
 *              --> 84249 = 84/09/05                                   *
 *                  Julian 84/09/05                                    *
 *              --> 84/09/05 = 84249                                   *
 *                                                                     *
 * (from REXX pgm): Say Julian(85123)                                  *
 *              --> 85/05/03                                           *
 *                  Say Julian('85/05/03')                             *
 *              --> 85123                                              *
 *                                                                     *
 ***********************************************************************/
Julian: 
Parse source . calltype .
normdays = '31 28 31 30 31 30 31 31 30 31 30 31'
bissdays = '31 29 31 30 31 30 31 31 30 31 30 31'
 
Parse arg yy'/'mm'/'dd
If dd ^= ''
 Then
  Do
    If ^Datatype(yy||mm||dd,'W') Then Do
                                        Say "Invalid yy/mm/dd date:" Arg(1)
                                        Exit
                                      End
    yy = Right(yy,2,0)
    If yy // 4 = 0 Then days = bissdays
                   Else days = normdays
    Do i = 1 to mm-1
      dd = dd + Word(days,i)
    End
    result = yy||Right(dd,3,0)
  End
 Else
  Do
    If ^Datatype(yy,'W') | Length(yy) ^= 5 Then
     Do
       Say "Invalid yyddd date:" Arg(1)
       Exit
     End
    ddd = Right(yy,3)
    yy = Left(yy,2)
    If yy // 4 = 0 Then days = bissdays
                   Else days = normdays
    mm = 1
    Do i = 1
      md = Word(days,i)
      If md >= ddd Then Leave
      mm = mm + 1
      ddd = ddd - md
    End
    result = yy'/'Right(mm,2,0)'/'Right(ddd,2,0)
  End
 
If calltype ^= 'COMMAND' Then Return result
Say Arg(1) "=" result
