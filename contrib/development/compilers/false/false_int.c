/* Introducing: Portable False!!!!

   PortableFalse is different from AmigaFalse in:
   - its Portable!!! :-)
   - full stack checking
   - strongly typed (no joke, really!)
   - debug-modi
   - real and meaningfull errormessages
   - ` inline assembly not supported
   - : and ; not supported for other than variable access.
   - "beta" (flush) and "zero-slash" (pick) from the amiga charset
     are now 'B' and 'O' resp.
   - 'D' toggles stack-watch mode on and off. format:
     [ bottom_of_stack , ... , top_of_stack | next_symbol ]
   - "-q" on command line is quiet mode: no title printing.
     (usefull for "filter"-type programs: 1> False -q filter.f <bla >burp)

   this is v1.2, changes from v1.1 include minor fixes for Ultrix and Irix.
   (the stunning original formatting hast been left intact now). Also,
   remaing commandline arguments are parsed as integers and put into
   variables b-z (a = argc).

   this source has been writtin in good C style:
   - no modularity whatsoever (only main())
   - only global variables
   - lots of ugly macros (replacing functions)
   - great source formatting and indentation

   still compiles on a great number of ansi-C compilers on Amiga/SunOs/
   Linux/Ultrix/Irix etc. if you have trouble porting it to your machine,
   your compiler sucks.

*/

#define MZ 10000
#define MS 1000
#include <stdio.h>
#define NIL 0
#define NUM 0
#define FUNC 1
#define VADR 2
#define UNDEF 3
#define l(x) ;break;case x:
#define x(num) {ernum=num;goto er;}
#define push(v,a) {if(S-2<sbeg)x(4)else{*--S=(v);*--S=(X)a;};}
#define pop(v,a) {if(S+2>se)x(5)else\
  {if((ex=(int)a)!=(ge=(int)*S++))x(6);v= *S++;};}
#define pa(v,av) {if(S+2>se)x(5)else{av= *S++;v= *S++;};}
#define ru(v) {if(rp-1<rbeg)x(13)else{*--rp=(v);};}
#define ro(v) {if(rp+1>rend)x(14)else{v= *rp++;};}
#define CA(c) {ru(p);p=c;}
#define pu(x) push(x,NUM)
#define po(x) pop(x,NUM)
#define op(o) {po(b)po(d);pu((X)((int)d o (int)b));}

#if defined(__GNUC__) && (__GNUC__ >= 4)
#warning "GCC 4 Fix. CHECKME. Not tested!"
#define cm(o,tt) {pa(b,(tt))pop(d,(X)tt);pu((X)(-(int)((int)d o (int)b)));}
#else
#define cm(o,tt) {pa(b,((X)tt))pop(d,(X)tt);pu((X)(-(int)((int)d o (int)b)));}
#endif

#define un(o) {po(b)pu((X)(o (int)b));}
#define ne (p<ent)
#define W while
#define ec {W((*p!='}')&&ne)p++;p++;if(!ne)x(10);}
#define P printf
typedef char* X;
typedef char** XP;
X ST[MS],RST[MS],var[52],b,d,e,f,t1,t2,t3;
XP sbeg=ST+12,se=ST+MS-12,S,ts,rbeg=RST+12,rend=RST+MS-12,rp,vp;
int ernum=0,t,tt,db=0,ex,ge,qq,ac=1,ic=0,it;
FILE* fh;
char *fn=0,src[MZ+5],a,c=0,*s,*ent,*beg,*p=0,
*erstr[]={"no args","could not open source file","source too large",
"data stack overflow","data stack underflow","type conflict",
"stack not empty at exit of program","unknown symbol",
"portable inline assembly not available","unbalanced '{'",
"unbalanced '\"'","unbalanced '['","return stack overflow",
"return stack underflow"};
char*types[]={"Integer","Functional","Variabele","Uninitialised"};

int main(int narg,char*args[]) {
  S=se;rp=rend;t=1;for(vp=var;vp<(var+52);){*vp++=(X)UNDEF;};
  while(ac<narg) {
    if(args[ac][0]=='-'&&args[ac][1]=='q'){t=2;}
    else if(!fn){fn=args[ac];}
    else {it=0;sscanf(args[ac],"%d",&it);var[(++ic)*2+1]=(X)NUM;var[ic*2]=(X)it;};
    ac++;
  };
  var[1]=(X)NUM;var[0]=(X)ic;
  if(t==1)P("Portable False Interpreter/Debugger v0.1 (c) 1993 $#%%!\n");
  if(!fn)x(1);if((fh=fopen(fn,"r"))==NIL)x(2);s=src; *s++='\n';
  W(a=fgetc(fh),!feof(fh))if((src+MZ)<=s){fclose(fh);x(3)}else{*s++=a;};
  *s++='\n';fclose(fh);ent=s-1;beg=src+1;p=beg;
  W(ne) {
    c= *p++;if(c>='0'&&c<='9'){int num;sscanf(p-1,"%d",&num);
    W((*p>='0')&&(*p<='9'))p++;push((X)num,NUM);}
    else if(c>='a'&&c<='z'){push((X)&var[(c-'a')*2],VADR);}
    else switch(c) {
      case' ':case '\n':case'\t':
      l('+')op(+)
      l('-')op(-)
      l('*')op(*)
      l('/')op(/)  
      l('&')op(&)
      l('|')op(|)
      l('_')un(-)
      l('~')un(~)
      l('=')cm(==,tt)
      l('>')cm(>,tt)
      l('%')pa(b,e)
      l('$')pa(b,e)push(b,e)push(b,e)
      l('\\')pa(b,e)pa(d,f)push(b,e)push(d,f)
      l('@')pa(b,t1)pa(d,t2)pa(e,t3)push(d,t2)push(b,t1)push(e,t3)
      l('O')po(b)if(S+((t=(int)b*2)+2)>se)x(5)b= *(S+t);d= *(S+t+1);push(d,b)
      l(':')pop(b,VADR)pa(d,e)*((XP)b)=d;*(((XP)b)+1)=e;
      l(';')pop(b,VADR)push(*((XP)b),*(((XP)b)+1));
      l('.')po(b)P("%d",(int)b);
      l(',')po(b)P("%c",(char)b);
      l('^')pu((X)fgetc(stdin));
      l('B')fflush(stdout);fflush(stdin);
      l('\"')W((*p!='\"')&&ne){fputc(*p,stdout);p++;};p++;if(!ne)x(11);
      l('{')ec;
      l('\'')pu((X)*p++);
      l('`')x(9);
      l('D')db=!db;
      l('[')push((X)p,FUNC)t=1;W(t>0&&ne){a= *p++;if(a=='['){t++;}else
        if(a==']'){t--;}else if(a=='{'){ec}else if(a=='\"'){W((*p!='\"')
        &&ne)p++;p++;if(!ne)x(11);};};if(!ne)x(12);
      l(']')ro(e)if((int)e==0){ro(p)po(b)if((int)b) { ro(d)ru(d)CA(d)
        ru((X)1);} else { ro(d)ro(d); };}else if((int)e==1){ro(p)ro(b)
        ro(d)ru(d)ru(b)CA(d)ru((X)0);}else{p=e;};
      l('!')pop(b,FUNC)CA(b);
      l('?')pop(b,FUNC)po(d)if((int)d){CA(b);};
      l('#')pop(b,FUNC)pop(d,FUNC)ru(d)ru(b);CA(d)ru((X)0);
      break;default:x(8);
    };
    if(db){c= *p;
      if(c!=' '&&c!='\n'&&c!='\t'&&c!='{'&&c!='\"'){
        ts=S+20;if(ts>se)ts=se;P("[");
        W(ts>S){t=(int)*(ts-2);if(t==FUNC){P("<func>");}else if(t==VADR){
          P("<var>");}else P("%d",(int)*(ts-1));ts-=2;if(ts>S)P(",");};
        P("|'%c']\n",*p);
      };
    };
  };
  c=0;p=0;if(S!=se)x(7);
  er:if(ernum) {
    P("\nERROR: %s!\n",erstr[ernum-1]);
    if(c)P("WORD:  '%c'\n",c);
    if(ernum==6)P("INFO:  Expecting %s type, while reading %s type.\n",
      types[ex],types[ge]);
    if(p){
      ent=p;beg=p;
      W(*(beg-1)!='\n'){beg--;};
      W(*ent!='\n'){ent++;};
      t=ent-beg;*ent=0;
      if(t>0) {
        P("LINE:  %s\n",beg);
        qq=p-beg+3;
        P("AT:");for(t=0;t<qq;t++){putchar(' ');};P("^\n");
      };
    };
  };
  return 0;
};
