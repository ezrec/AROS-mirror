/*

Directory Opus 4
Original GPL release version 4.12
Copyright 1993-2000 Jonathan Potter

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

All users of Directory Opus 4 (including versions distributed
under the GPL) are entitled to upgrade to the latest version of
Directory Opus version 5 at a reduced price. Please see
http://www.gpsoft.com.au for more information.

The release of Directory Opus 4 under the GPL in NO WAY affects
the existing commercial status of Directory Opus 5.

*/

#include "dopuslib.h"

extern int calcdate(int *,int *,int *);
extern char *getnextbit(char *,int *,int);

char
	*def_weekdays[7]={
		"Sunday","Monday","Tuesday","Wednesday","Thursday","Friday","Saturday"},
	*def_months[12]={
		"January","February","March","April","May","June",
		"July","August","September","October","November","December"},
	*def_special_days[4]={
		"Yesterday","Today","Tomorrow","Future"};

#define SPEC_YESTERDAY 0
#define SPEC_TODAY     1
#define SPEC_TOMORROW  2
#define SPEC_FUTURE    3

char mondays[12]={31,28,31,30,31,30,31,31,30,31,30,31};


/*****************************************************************************

    NAME */

	AROS_LH1(int, StampToStr,

/*  SYNOPSIS */
	AROS_LHA(struct DOpusDateTime *, dt, A0),

/*  LOCATION */
	struct Library *, DOpusBase, 38, DOpus)
/*  FUNCTION

    INPUTS

    RESULT

    NOTES

    EXAMPLE

    BUGS

    SEE ALSO

    INTERNALS

    HISTORY
	27-11-96    digulla automatically created from
			    asl_lib.fd and clib/asl_protos.h

*****************************************************************************/
{
	AROS_LIBFUNC_INIT
	AROS_LIBBASE_EXT_DECL(struct Library *,DOpusBase)

	int day,mon,yr,hr,min,sec,wday,diff;
	struct DateStamp now;
	char **weekdays=NULL,**special_days=NULL,**shortmonths=NULL;
	ULONG ds_days,ds_minute,ds_tick;

	ds_days=dt->dat_Stamp.ds_Days;
	ds_minute=dt->dat_Stamp.ds_Minute;
	ds_tick=dt->dat_Stamp.ds_Tick;

	if (ds_minute>1440) ds_minute%=1440;
	if (ds_tick>3000) ds_tick%=3000;

	if (dt->dat_Flags&DDTF_CUSTOM) {
		weekdays=dt->custom_weekdays;
		shortmonths=dt->custom_shortmonths;
		special_days=dt->custom_special_days;
	}

	if (!weekdays) weekdays=def_weekdays;
	if (!special_days) special_days=def_special_days;

	DateStamp(&now);
	day=ds_days;
	calcdate(&day,&mon,&yr);
	while (yr>99) yr-=100;

/*
UWORD DayOfWeek (UWORD Year, UWORD Month, UWORD Day)
{
   return (((3 * Year - (7 * (Year + (Month + 9) / 12)) / 4 +
           (23 * Month) / 9 + Day + 2 + 15 -
           ((Year - (Month < 3)) / 100 + 1) * 3 / 4 ) % 7 ));
}
*/
	wday=ds_days%7;
	if (wday<0) wday=6;
	hr=ds_minute/60;
	min=ds_minute%60;
	sec=ds_tick/50;

	if (dt->dat_StrDay) strcpy(dt->dat_StrDay,weekdays[wday]);
	if (dt->dat_StrTime) {
		if (dt->dat_Flags&DDTF_12HOUR) {
			int ampm;

			if (hr>=12) {
				if (hr>12) hr-=12;
				ampm=1;
			}
			else {
				if (hr==0) hr=12;
				ampm=0;
			}
			LSprintf(dt->dat_StrTime,"%2ld:%02ld:%02ld%lc",hr,min,sec,(ampm)?'P':'A');
		}
		else LSprintf(dt->dat_StrTime,"%02ld:%02ld:%02ld",hr,min,sec);
	}
	if (dt->dat_StrDate) {
		diff=ds_days-now.ds_Days;
		if (dt->dat_Flags&DDTF_SUBST && diff>-7) {
			char *temp;

			if (diff==-1) temp=special_days[SPEC_YESTERDAY];
			else if (diff==0) temp=special_days[SPEC_TODAY];
			else if (diff==1) temp=special_days[SPEC_TOMORROW];
			else if (diff>1) temp=special_days[SPEC_FUTURE];
			else temp=weekdays[wday];

			strcpy(dt->dat_StrDate,temp);
		}
		else switch (dt->dat_Format) {
			case FORMAT_INT:
				LSprintf(dt->dat_StrDate,"%02ld-%02ld-%02ld",yr,mon+1,day);
				break;

			case FORMAT_USA:
				LSprintf(dt->dat_StrDate,"%02ld-%02ld-%02ld",mon+1,day,yr);
				break;

			case FORMAT_CDN:
				LSprintf(dt->dat_StrDate,"%02ld-%02ld-%02ld",day,mon+1,yr);
				break;

			case FORMAT_DOS:
			default:
				{
					char temp[10];

					if (shortmonths) strcpy(temp,shortmonths[mon]);
					else {
						temp[0]=def_months[mon][0];
						temp[1]=def_months[mon][1];
						temp[2]=def_months[mon][2];
						temp[3]=0;
					}

					LSprintf(dt->dat_StrDate,"%02ld-%3s-%02ld",day,temp,yr);
				}
				break;
		}
	}
	return(1);
	
	AROS_LIBFUNC_EXIT
}

int calcdate(day,month,year)
int *day,*month,*year;
{
	int i,mdays,ldays,year2;

	ldays=1461; year2=78;
	year2+=((*day)/ldays)*4;
	(*day)%=ldays;
	while ((*day)) {
		mdays=365;
		if ((year2&3)==0) mdays++;
		if ((*day)<mdays) break;
		(*day)-=mdays;
		year2++;
	}
	for (i=0,(*day)++;i<12;i++) {
		mdays=mondays[i];
		if (i==1&&(year2&3)==0) mdays++;
		if ((*day)<=mdays) break;
		(*day)-=mdays;
	}
	(*month)=i;
	(*year)=year2;
	return(0);
}

/*****************************************************************************

    NAME */

	AROS_LH1(int, StrToStamp,

/*  SYNOPSIS */
	AROS_LHA(struct DOpusDateTime *, dt, A0),

/*  LOCATION */
	struct Library *, DOpusBase, 39, DOpus)
/*  FUNCTION

    INPUTS

    RESULT

    NOTES

    EXAMPLE

    BUGS

    SEE ALSO

    INTERNALS

    HISTORY
	27-11-96    digulla automatically created from
			    asl_lib.fd and clib/asl_protos.h

*****************************************************************************/
{
	AROS_LIBFUNC_INIT
	AROS_LIBBASE_EXT_DECL(struct Library *,DOpusBase)

	int day,mon,yr,hr,min,sec,a,b,wday,c;
	char *ptr,monstr[20];
	struct DateStamp now;
	char **weekdays=NULL,**shortweekdays=NULL,**special_days=NULL,
		**months=NULL,**shortmonths=NULL;

	if (dt->dat_Flags&DDTF_CUSTOM) {
		weekdays=dt->custom_weekdays;
		shortweekdays=dt->custom_shortweekdays;
		special_days=dt->custom_special_days;
		months=dt->custom_months;
		shortmonths=dt->custom_shortmonths;
	}

	if (!weekdays) weekdays=def_weekdays;
	if (!special_days) special_days=def_special_days;
	if (!months) months=def_months;

	c=0;

	if (dt->dat_StrTime) {
		ptr=getnextbit(dt->dat_StrTime,&hr,0);
		ptr=getnextbit(ptr,&min,0);
		getnextbit(ptr,&sec,0);
		dt->dat_Stamp.ds_Minute=(hr*60)+min;
		dt->dat_Stamp.ds_Tick=sec*50;
		c=1;
	}
	if (dt->dat_StrDate) {
		DateStamp(&now);
		for (a=0;a<7;a++) {
			if (LStrCmpI(dt->dat_StrDate,weekdays[a])==0) break;
			if (shortweekdays) {
				if (LStrnCmpI(dt->dat_StrDate,shortweekdays[a],3)==0) break;
			}
			else if (LStrnCmpI(dt->dat_StrDate,weekdays[a],3)==0) break;
		}
		if (a<7) {
			wday=now.ds_Days%7;
			b=0;
			if (dt->dat_Flags&DDTF_FUTURE) {
				while (wday!=a) {
					++wday; ++b;
					if (wday>6) wday=0;
				};
			}
			else {
				while (wday!=a) {
					--wday; --b;
					if (wday<0) wday=6;
				};
			}
			day=now.ds_Days+b;
		}
		else {
			if (LStrCmpI(dt->dat_StrDate,special_days[SPEC_TOMORROW])==0) day=now.ds_Days+1;
			else if (LStrCmpI(dt->dat_StrDate,special_days[SPEC_TODAY])==0) day=now.ds_Days;
			else if (LStrCmpI(dt->dat_StrDate,special_days[SPEC_YESTERDAY])==0) day=now.ds_Days-1;
			else {
				day=1; mon=1; yr=78;
				switch (dt->dat_Format) {
					case FORMAT_DOS:
						ptr=getnextbit(dt->dat_StrDate,&day,0);
						ptr=getnextbit(ptr,(int *)monstr,1);
						getnextbit(ptr,&yr,0);
						for (a=0;a<12;a++) {
							if ((LStrCmpI(monstr,months[a])==0) ||
								(shortmonths && LStrCmpI(monstr,shortmonths[a])==0) ||
								(!shortmonths && LStrnCmpI(monstr,months[a],3)==0)) {
								mon=a+1;
								break;
							}
						}
						break;

					case FORMAT_INT:
						ptr=getnextbit(dt->dat_StrDate,&yr,0);
						ptr=getnextbit(ptr,&mon,0);
						getnextbit(ptr,&day,0);
						break;

					case FORMAT_USA:
						ptr=getnextbit(dt->dat_StrDate,&mon,0);
						ptr=getnextbit(ptr,&day,0);
						getnextbit(ptr,&yr,0);
						break;

					case FORMAT_CDN:
						ptr=getnextbit(dt->dat_StrDate,&day,0);
						ptr=getnextbit(ptr,&mon,0);
						getnextbit(ptr,&yr,0);
						break;
				}
				if (yr<78) yr+=2000;
				else if (yr<100) yr+=1900;
				if (yr<1978) yr=1978;
				else if (yr>2045) yr=2045;
				--day; --mon;
				for (a=0;a<mon;a++) {
					day+=mondays[a];
					if (a==1 && (yr%4)==0 && ((yr%100)!=0 || (yr%400)==0)) ++day;
				}
				for (a=1978;a<yr;a++) {
					day+=365;
					if ((a%4)==0 && ((a%100)!=0 || (a%400)==0)) ++day;
				}
			}
		}
		dt->dat_Stamp.ds_Days=day;
		c=1;
	}
	return(c);
	
	AROS_LIBFUNC_EXIT
}

char *getnextbit(ptr,retbuf,t)
char *ptr;
int *retbuf,t;
{
	int a,b,c;
	char *str;

	if (!ptr) return(NULL);

	if (t==0) *retbuf=0;
	else str=(char *)retbuf;
	c=1;
	for (a=0;;a++) {
		if (!ptr[a] || ptr[a]=='-' || ptr[a]=='/' || ptr[a]=='.' || ptr[a]==' ') break;
		if (t==0 && (ptr[a]<'0' || ptr[a]>'9')) break;
		c*=10;
	}
	c/=10;
	for (b=0;b<a;b++) {
		if (t==0) {
			*retbuf+=(ptr[b]-'0')*c;
			c/=10;
		}
		else {
			str[b]=ptr[b];
			str[b+1]=0;
		}
	}
	if (ptr[a]==0) return(NULL);
	return(ptr+a+1);
}


/*****************************************************************************

    NAME */

	AROS_LH2(int, CompareDate,

/*  SYNOPSIS */
	AROS_LHA(struct DateStamp *, date,	A0),
	AROS_LHA(struct DateStamp *, date2,	A1),

/*  LOCATION */
	struct Library *, DOpusBase, 54, DOpus)
/*  FUNCTION

    INPUTS

    RESULT

    NOTES

    EXAMPLE

    BUGS

    SEE ALSO

    INTERNALS

    HISTORY
	27-11-96    digulla automatically created from
			    asl_lib.fd and clib/asl_protos.h

*****************************************************************************/
{
	AROS_LIBFUNC_INIT
	AROS_LIBBASE_EXT_DECL(struct Library *,DOpusBase)

	if (!date) return(-1);
	if (!date2) return(1);
	if (date->ds_Days>date2->ds_Days) return(1);
	if (date->ds_Days==date2->ds_Days) {
		if (date->ds_Minute>date2->ds_Minute) return(1);
		if (date->ds_Minute==date2->ds_Minute) {
			if (date->ds_Tick>date2->ds_Tick) return(1);
			if (date->ds_Tick==date2->ds_Tick) return(0);
		}
	}
	return(-1);
	
	AROS_LIBFUNC_EXIT
}
