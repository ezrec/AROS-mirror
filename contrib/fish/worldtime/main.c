//#include "wtime.h"
#include "data.c"
#include <aros/oldprograms.h>
#include <exec/types.h>
#include <exec/memory.h>
#include <graphics/gfxbase.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <proto/alib.h>


void fixcr(char * buf);
int dotimes(void);
void quit(void);
int iconify(void);
void drawtimes();
int yestom(int here, float heretime, float theretime);
void CalcDate(int *day, int *month, int *year2);


void main(argc,argv)
int argc;
char *argv[];
{
	int a,x,y,starticon=0;
	ULONG class;
	USHORT code;
	FILE *in=NULL;
	char buf[80],datfile[80];

	defotherplace[0]=datfile[0]=0;
	todaycol=2; yesterdaycol=tomorrowcol=clockcol=3; citycol=1;

	if (argc>1) in=fopen(argv[1],"r");
	if (!in) {
		if (!(in=fopen("s:worldtime.cfg","r")))
			if (!(in=fopen("worldtime.cfg","r"))) exit(0);
	}
	while (!(feof(in))) {
		buf[0]=0;
		fgets(buf,80,in);
		if (strncmp(buf,"-lp",3)==0) {
			strncpy(yourplace,&buf[3],19);
			yourplace[19]=0;
			fixcr(yourplace);
		}
		else if (strncmp(buf,"-lt",3)==0) yourtime=atof(&buf[3]);
		else if (strncmp(buf,"-op",3)==0) {
			strncpy(defotherplace,&buf[3],19);
			defotherplace[19]=0;
			fixcr(defotherplace);
		}
		else if (strncmp(buf,"-tc",3)==0) todaycol=atoi(&buf[3]);
		else if (strncmp(buf,"-yc",3)==0) yesterdaycol=atoi(&buf[3]);
		else if (strncmp(buf,"-mc",3)==0) tomorrowcol=atoi(&buf[3]);
		else if (strncmp(buf,"-cc",3)==0) clockcol=atoi(&buf[3]);
		else if (strncmp(buf,"-ic",3)==0) citycol=atoi(&buf[3]);
		else if (strncmp(buf,"-ot",3)==0) defothertime=atof(&buf[3]);
		else if (strncmp(buf,"-x",2)==0) iconwin.LeftEdge=atoi(&buf[2]);
		else if (strncmp(buf,"-y",2)==0) iconwin.TopEdge=atoi(&buf[2]);
		else if (strncmp(buf,"-i",2)==0) starticon=1;
		else if (strncmp(buf,"-d",2)==0) {
			strncpy(datfile,&buf[3],79);
			datfile[79]=0;
			fixcr(datfile);
		}
	}
	fclose(in);
	in=NULL;
	if (datfile[0]) in=fopen(datfile,"r");
	if (!in) {
		if (!(in=fopen("s:worldtime.dat","r")))
			if (!(in=fopen("worldtime.dat","r"))) exit(0);
	}
	for (a=0;a<84;a++) {
		buf[0]=0;
		fgets(buf,80,in);
		if (feof(in)) break;
		fixcr(buf);
		for (x=0;x<80;x++) {
			if (buf[x]==',' || buf[x]==0) break;
		}
		strncpy(datfile,buf,x);
		if (x>13) x=13; datfile[x]=0;
		strcpy(TimeZones[a].name,datfile); TimeZones[a].time=atof(&buf[x+1]);
	}
	fclose(in);
	timerport=NULL;
	Window=NULL;

	IntuitionBase=(struct IntuitionBase *)OpenLibrary("intuition.library",0);
	GfxBase=(struct GfxBase *)OpenLibrary("graphics.library",0);
	if (!(timerport=CreatePort(0,0))) quit();
	OpenDevice(TIMERNAME,UNIT_VBLANK,(struct IORequest *)&timereq,0);

	timereq.tr_node.io_Message.mn_ReplyPort=timerport;
	timereq.tr_node.io_Command=TR_ADDREQUEST;
	timereq.tr_node.io_Flags=0;
	timereq.tr_time.tv_secs=1;
	timereq.tr_time.tv_micro=0;
	SendIO((struct IORequest *)&timereq.tr_node);

	if (starticon) {
		strcpy(otherplace,defotherplace);
		othertime=defothertime;
		iconify();
	}
	else {
		if (!(Window=OpenWindow(&win))) quit();
		rp=Window->RPort;
	}

	drawtimes();
	lastmin=lastday=-1;
	dotimes();

	FOREVER {
		Wait(1<<Window->UserPort->mp_SigBit|1<<timerport->mp_SigBit);
		if (CheckIO((struct IORequest *)&timereq.tr_node)) {
			WaitIO((struct IORequest *)&timereq.tr_node);
			dotimes();
			timereq.tr_time.tv_secs=1;		
			timereq.tr_time.tv_micro=0;
			SendIO((struct IORequest *)&timereq.tr_node);
		}
		while (NULL !=(Mesg=(struct IntuiMessage *)GetMsg(Window->UserPort))) {
			class=Mesg->Class; code=Mesg->Code;
			x=Window->MouseX; y=Window->MouseY;
			ReplyMsg((struct Message *)Mesg);
			if (class==CLOSEWINDOW) quit();
			if (class==MOUSEBUTTONS && code==SELECTDOWN) {
				if (y>185 && y<195) {
					strcpy(otherplace,defotherplace);
					othertime=defothertime;
				}
				else {
					x-=8; y-=12;
					x/=156; y/=8;
					if (x>3 || x<0 || y>20 || y<0) continue;
					a=(x*21)+y;
					strcpy(otherplace,TimeZones[a].name);
					othertime=TimeZones[a].time;
				}
				iconify();
				drawtimes();
				lastmin=lastday=-1;
				dotimes();
			}
		}
	}
}

void quit()
{
	if (Window) CloseWindow(Window);
	if (timerport) {
		AbortIO((struct IORequest *)&timereq);
		WaitIO((struct IORequest *)&timereq);
		CloseDevice((struct IORequest *)&timereq);
		DeletePort(timerport);
	}
	CloseLibrary((struct Library *)IntuitionBase);
	CloseLibrary((struct Library *)GfxBase);
	exit(0);
}

int dotimes()
{
	int myhour,myminute,day,month,year,wday,mytot,x,y,gmttot,min,hour,a;
	struct DateStamp now;
	char buf[40],buf1[40],c,*ptr;

	DateStamp(&now);
	myhour=now.ds_Minute/60; myminute=now.ds_Minute%60;

	if (now.ds_Minute!=lastmin) {
		lastmin=now.ds_Minute;
		gmttot=now.ds_Minute-(int)(yourtime*(float)60);
		if (gmttot>1439) gmttot-=1440;
		else if (gmttot<0) gmttot+=1440;

		x=112; y=18;
		for (a=0;a<84;a++) {
			mytot=gmttot+(int)(TimeZones[a].time*(float)60);
			if (mytot>1439) mytot-=1440;
			else if (mytot<0) mytot+=1440;
			switch (yestom(lastmin,yourtime,TimeZones[a].time)) {
				case -1:
					c='-';
					SetAPen(rp,yesterdaycol);
					break;
				case 1:
					c='+';
					SetAPen(rp,tomorrowcol);
					break;
				case 0:
					c=' ';
					SetAPen(rp,todaycol);
					break;
			}
			hour=mytot/60; min=mytot%60;
			sprintf(buf,"%c%2d:%02d",c,hour,min);
			Move(rp,x,y);
			Text(rp,buf,6);
			y+=8;
			if (y>=184) {
				x+=158; y=18;
			}
		}
	}
	sprintf(buf,"%2d:%02d:%02d, ",myhour,myminute,now.ds_Tick/TICKS_PER_SECOND);
	SetAPen(rp,clockcol);
	Move(rp,timeoff,192);
	Text(rp,buf,10);
	day=now.ds_Days;
	if (day==lastday) return(0);
	lastday=day;
	CalcDate(&day,&month,&year);
	wday=now.ds_Days%7;
	if (day==1 || day==21 || day==31) ptr="st";
	else if (day==2 || day==22) ptr="nd";
	else if (day==3 || day==23) ptr="rd";
	else ptr="th";
	sprintf(buf,"%s, the %d%s of %s, 19%d",days[wday],day,ptr,months[month],year);
	sprintf(buf1,"%-40s",buf);
	Text(rp,buf1,40);
return 0;
}

void CalcDate(day,month,year2)
int *day,*month,*year2;
{
	int i,mdays,year;

	year=78;
	year+=((*day)/1461)*4;
	(*day)%=1461;
	while ((*day)) {
		mdays=365;
		if ((year&3)==0) mdays++;
		if ((*day)<mdays) break;
		(*day)-=mdays;
		year++;
	}
	for (i=0,(*day)++;i<12;i++) {
		mdays=wdays[i];
		if (i==1 && (year&3)==0) mdays++;
		if ((*day)<=mdays) break;
		(*day)-=mdays;
	}
	(*month)=i;
	(*year2)=year;
}

int iconify()
{
	char buf[60];
	int day,month,year,hour,min,sec,x,y,gmttot,dayoff,len=-1,xp=-1,h,w,v2=0;
	struct DateStamp now;
	ULONG class;
	USHORT code;

	if (GfxBase->LibNode.lib_Version>=36) v2=1;

	if (Window) CloseWindow(Window);
	if (otherplace[0]==0) iconwin.Height=10;
	else iconwin.Height=21;
	h=IntuitionBase->ActiveScreen->Height;
	w=IntuitionBase->ActiveScreen->Width;
	if (iconwin.Height+iconwin.TopEdge>h) iconwin.TopEdge=h-iconwin.Height;
	if (iconwin.LeftEdge+388>w) iconwin.LeftEdge=w-388;

	if (!(Window=OpenWindow(&iconwin))) quit();
	rp=Window->RPort;

	FOREVER {
		Wait(1<<Window->UserPort->mp_SigBit|1<<timerport->mp_SigBit);
		if (CheckIO((struct IORequest *)&timereq.tr_node)) {
			WaitIO((struct IORequest *)&timereq.tr_node);
			DateStamp(&now);
			hour=now.ds_Minute/60; min=now.ds_Minute%60;
			sec=now.ds_Tick/TICKS_PER_SECOND;
			day=now.ds_Days;
			CalcDate(&day,&month,&year);
			sprintf(buf,"CHIP:%4d FAST:%4d %02d-%3s-%2d %2d:%02d:%02d",
				AvailMem(MEMF_CHIP)>>10,AvailMem(MEMF_FAST)>>10,
				day,shortmonths[month],year,hour,min,sec);
			if (v2) {
				if (IntuitionBase->ActiveWindow==Window) {
					SetAPen(rp,2); SetBPen(rp,3);
				}
				else {
					SetAPen(rp,1); SetBPen(rp,0);
				}
			}
			else {
				SetAPen(rp,2); SetBPen(rp,1);
			}
			Move(rp,30,7); Text(rp,buf,38);
			gmttot=now.ds_Minute-(int)(yourtime*(float)60);
			if (gmttot>1439) gmttot-=1440;
			else if (gmttot<0) gmttot+=1440;
			gmttot+=(int)(othertime*(float)60);
			if (gmttot>1439) gmttot-=1440;
			else if (gmttot<0) gmttot+=1440;
			dayoff=yestom(now.ds_Minute,yourtime,othertime);
			hour=gmttot/60; min=gmttot%60;
			day=now.ds_Days+dayoff; x=day;
			CalcDate(&day,&month,&year);
			sprintf(buf,"%s : %9s, %02d-%3s-%2d, %2d:%02d:%02d",
				otherplace,days[x%7],day,shortmonths[month],year,hour,min,sec);
			if (len==-1) {
				len=strlen(buf);
				xp=(388-(len*8))/2;
				if (xp<0) xp=0;
			}
			SetAPen(rp,1); SetBPen(rp,0);
			Move(rp,xp,17); Text(rp,buf,len);
			timereq.tr_time.tv_secs=1;		
			timereq.tr_time.tv_micro=0;
			SendIO((struct IORequest *)&timereq.tr_node);
			continue;
		}
		while (Mesg=(struct IntuiMessage *)GetMsg(Window->UserPort)) {
			class=Mesg->Class; code=Mesg->Code;
			x=Window->MouseX; y=Window->MouseY;
			ReplyMsg((struct Message *)Mesg);
			if (class==CLOSEWINDOW) quit();
			if (class==MOUSEBUTTONS && code==MENUDOWN) {
				CloseWindow(Window);
				if (!(Window=OpenWindow(&win))) quit();
				rp=Window->RPort;
				return(0);
			}
		}
	}
}

void fixcr(buf)
char *buf;
{
	while (*buf && *buf!='\n') ++buf;
	*buf=0;
}

int yestom(here,heretime,theretime)
int here;
float heretime,theretime;
{
	float diff;
	int mind;

	diff=(heretime+12)-(theretime+12);
	mind=(int)(diff*(float)60);
	if (mind<0) {
		if ((here-mind)>1439) return(1);
	}
	else if (mind>0) {
		if ((here-mind)<0) return(-1);
	}
	return(0);
}

void drawtimes()
{
	int a,x,y,b;

	x=8; y=18;
	SetAPen(rp,citycol);
	SetBPen(rp,0);
	SetDrMd(rp,JAM2);
	for (a=0;a<84;a++) {
		Move(rp,x,y);
		Text(rp,TimeZones[a].name,strlen(TimeZones[a].name));
		y+=8;
		if (y>=184) {
			x+=158; y=18;
		}
	}

	SetAPen(rp,clockcol);
	Move(rp,8,192);
	b=strlen(yourplace);
	Text(rp,yourplace,b);
	Text(rp,",",1);
	timeoff=24+(b*8);
}
