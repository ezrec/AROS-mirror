#include <exec/types.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

/* Date functions for MM2 */

char *Months = "JANFEBMARAPRMAYJUNJULAUGSEPOCTNOVDEC";
char Today_buf[10];

int Month2Num(char *MonthTxt);
BOOL Month2Txt(int MonthNum, char *MonthTxt);
BOOL CheckMemoDate(char *dbuf);
BOOL CheckDate(int dd, int mm, int yy);
void Today(char *Today_buf);
char *ctime();

/*-----------------------------------------------------------------------
 * Convert month as MMM (eg JUL) to month number (eg 2 for FEB).
 *   Does not assume case or that text is NULL terminated
 */
Month2Num(char *MonthTxt)
  {
  int x, month;
  char m[4];

  for (x=0 ; x<3 ; x++)
    {
    m[x] = toupper(*(MonthTxt+x));
    }

  m[3]='\0';
  month=(int) (strstr(Months, m) - Months);
  if (month%3 != 0) return 0;
  month = (month / 3) + 1;
  if ((month < 1) || (month > 12))
    return 0;
  else
    return month;
  }

/*-----------------------------------------------------------------------
 * Convert month number (eg 3 for March) to three char string in caller
 * supplied buffer. Return TRUE of FALSE.
 */
BOOL Month2Txt(int MonthNum, char *MonthTxt)
  {
  if ((MonthNum < 1) || (MonthNum > 12))
    {
    *MonthTxt = '\0';
    return FALSE;
    }
  MonthNum = (MonthNum - 1) * 3;
  *MonthTxt = *(Months+MonthNum);
  *(MonthTxt+1) = *(Months+MonthNum+1);
  *(MonthTxt+2) = *(Months+MonthNum+2);
  *(MonthTxt+3) = '\0';
  return TRUE;
  }

/*===========================================================================*/
BOOL CheckMemoDate(char *dbuf)
  {
  char DDay[3];
  char DYear[3];

  DDay[0]=dbuf[0];
  DDay[1]=dbuf[1];
  DDay[2]='\0';
  DYear[0]=dbuf[7];
  DYear[1]=dbuf[8];
  DYear[2]='\0';
  return (CheckDate(atoi(DDay), Month2Num((char*)(dbuf+3)), atoi(DYear)));
  }
/*==========================================================================*/
BOOL CheckDate(int dd, int mm, int yy)
  {
  int md;
  int cd_md[]={0,31,28,31,30,31,30,31,31,30,31,30,31};

  if ((mm<1)||(mm>12)) return FALSE;  /* Validate month */
  md=cd_md[mm];
  if (yy<20) yy+=100;       /* Cope with years from 1920 to 2019  ???  */
  if ((mm==2)&&((yy-68)%4==0))
      md++;		       /* If Feb in a leap year, allow 29th */
  if (md<dd) return FALSE;        /* Validate day */
  return TRUE;			     /* Date passed checks so OK */
  }


/*------------------------------------------------------------------------
 * void Today(char *Today_buf)
 *   Sets up string containing todays date as dd-mmm-yy (eg 04-jan-56) in
 *   buffer supplied by caller. Buffer must be at least 10 bytes long.
 */
void Today(char *Today_buf)
  {
  long GMTsecs;
  char *time_str;

  GMTsecs = time(NULL);
  time_str = ctime(&GMTsecs);
  Today_buf[0] = *(time_str+8);
  Today_buf[1] = *(time_str+9);
  Today_buf[2] = '-';
  Today_buf[3] = *(time_str+4);
  Today_buf[4] = *(time_str+5);
  Today_buf[5] = *(time_str+6);
  Today_buf[6] = '-';
  Today_buf[7] = *(time_str+22);
  Today_buf[8] = *(time_str+23);
  Today_buf[9] = '\0';
  }

/*==========================================================================*/
date2days(dd,mm,yy)
int dd,mm,yy;
/* Returns number of days since start of 1970, ie 1 Jan 1970 is day 1 */
{
    int wy,wyd,lyd,wm,wmd;
    int ad[]={0,3,3,6,8,11,13,16,19,21,24,26};

    wy=yy-70;			/* whole years elapsed */
    wyd=wy*365; 		/* days in whole years ignoring leap years */
    if (wy>=3)                  /**/
	lyd=((wy-3)/4)+1;       /* work out additions due to leap years */
    else			/**/
	lyd=0;			/**/
    wm=mm-1;			/* whole months in last year */
    wmd=wm*28;			/**/
    if (((wy-2)%4==0)&&(wm>=2)) /**/
	wmd++;			/* days in whole months in latest year */
    wmd+=ad[wm];		/**/
    return wyd+lyd+wmd+dd;	/* return total */
}

/*==========================================================================*/

