/* menu.c */
#include <aros/oldprograms.h>
#include <exec/types.h>
#include <intuition/intuition.h>
#include "defs.h"

float remin=-2;
char reminbuf[] ={'-','2','.','0','0','0','\0','\0','\0','\0','\0','\0'};
char reminundo[]={'-','2','.','0','0','0','\0','\0','\0','\0','\0','\0'};
float remax=2;
char remaxbuf[] ={' ','2','.','0','0','0','\0','\0','\0','\0','\0','\0'};
char remaxundo[]={' ','2','.','0','0','0','\0','\0','\0','\0','\0','\0'};
float immin=-2;
char imminbuf[] ={'-','2','.','0','0','0','\0','\0','\0','\0','\0','\0'};
char imminundo[]={'-','2','.','0','0','0','\0','\0','\0','\0','\0','\0'};
float immax=2;
char immaxbuf[] ={' ','2','.','0','0','0','\0','\0','\0','\0','\0','\0'};
char immaxundo[]={' ','2','.','0','0','0','\0','\0','\0','\0','\0','\0'};
float reconst=-1.25;
char reconstbuf[] ={'-','1','.','2','5','0','\0','\0','\0','\0','\0','\0'};
char reconstundo[]={'-','1','.','2','5','0','\0','\0','\0','\0','\0','\0'};
float imconst=0.0;
char imconstbuf[] ={' ','0','.','0','0','0','\0','\0','\0','\0','\0','\0'};
char imconstundo[]={' ','0','.','0','0','0','\0','\0','\0','\0','\0','\0'};
float re0const=-1.00;
char re0constbuf[] ={'-','1','.','0','0','0','\0','\0','\0','\0','\0','\0'};
char re0constundo[]={'-','1','.','0','0','0','\0','\0','\0','\0','\0','\0'};
float im0const=0.5;
char im0constbuf[] ={' ','0','.','5','0','0','\0','\0','\0','\0','\0','\0'};
char im0constundo[]={' ','0','.','5','0','0','\0','\0','\0','\0','\0','\0'};
float re1const=-1.00;
char re1constbuf[] ={'-','1','.','0','0','0','\0','\0','\0','\0','\0','\0'};
char re1constundo[]={'-','1','.','0','0','0','\0','\0','\0','\0','\0','\0'};
float im1const=0.5;
char im1constbuf[] ={' ','0','.','5','0','0','\0','\0','\0','\0','\0','\0'};
char im1constundo[]={' ','0','.','5','0','0','\0','\0','\0','\0','\0','\0'};
float re2const=-1.00;
char re2constbuf[] ={'-','1','.','0','0','0','\0','\0','\0','\0','\0','\0'};
char re2constundo[]={'-','1','.','0','0','0','\0','\0','\0','\0','\0','\0'};
float im2const=0.5;
char im2constbuf[] ={' ','0','.','5','0','0','\0','\0','\0','\0','\0','\0'};
char im2constundo[]={' ','0','.','5','0','0','\0','\0','\0','\0','\0','\0'};
float re3const=-1.00;
char re3constbuf[] ={'-','1','.','0','0','0','\0','\0','\0','\0','\0','\0'};
char re3constundo[]={'-','1','.','0','0','0','\0','\0','\0','\0','\0','\0'};
float im3const=0.5;
char im3constbuf[] ={' ','0','.','5','0','0','\0','\0','\0','\0','\0','\0'};
char im3constundo[]={' ','0','.','5','0','0','\0','\0','\0','\0','\0','\0'};
float re4const=-1.00;
char re4constbuf[] ={'-','1','.','0','0','0','\0','\0','\0','\0','\0','\0'};
char re4constundo[]={'-','1','.','0','0','0','\0','\0','\0','\0','\0','\0'};
float im4const=0.5;
char im4constbuf[] ={' ','0','.','5','0','0','\0','\0','\0','\0','\0','\0'};
char im4constundo[]={' ','0','.','5','0','0','\0','\0','\0','\0','\0','\0'};
float re5const=-1.00;
char re5constbuf[] ={'-','1','.','0','0','0','\0','\0','\0','\0','\0','\0'};
char re5constundo[]={'-','1','.','0','0','0','\0','\0','\0','\0','\0','\0'};
float im5const=0.5;
char im5constbuf[] ={' ','0','.','5','0','0','\0','\0','\0','\0','\0','\0'};
char im5constundo[]={' ','0','.','5','0','0','\0','\0','\0','\0','\0','\0'};
float re6const=-1.00;
char re6constbuf[] ={'-','1','.','0','0','0','\0','\0','\0','\0','\0','\0'};
char re6constundo[]={'-','1','.','0','0','0','\0','\0','\0','\0','\0','\0'};
float im6const=0.5;
char im6constbuf[] ={' ','0','.','5','0','0','\0','\0','\0','\0','\0','\0'};
char im6constundo[]={' ','0','.','5','0','0','\0','\0','\0','\0','\0','\0'};
float re7const=-1.00;
char re7constbuf[] ={'-','1','.','0','0','0','\0','\0','\0','\0','\0','\0'};
char re7constundo[]={'-','1','.','0','0','0','\0','\0','\0','\0','\0','\0'};
float im7const=0.5;
char im7constbuf[] ={' ','0','.','5','0','0','\0','\0','\0','\0','\0','\0'};
char im7constundo[]={' ','0','.','5','0','0','\0','\0','\0','\0','\0','\0'};
float re8const=-1.00;
char re8constbuf[] ={'-','1','.','0','0','0','\0','\0','\0','\0','\0','\0'};
char re8constundo[]={'-','1','.','0','0','0','\0','\0','\0','\0','\0','\0'};
float im8const=0.5;
char im8constbuf[] ={' ','0','.','5','0','0','\0','\0','\0','\0','\0','\0'};
char im8constundo[]={' ','0','.','5','0','0','\0','\0','\0','\0','\0','\0'};
float re9const=-1.00;
char re9constbuf[] ={'-','1','.','0','0','0','\0','\0','\0','\0','\0','\0'};
char re9constundo[]={'-','1','.','0','0','0','\0','\0','\0','\0','\0','\0'};
float im9const=0.5;
char im9constbuf[] ={' ','0','.','5','0','0','\0','\0','\0','\0','\0','\0'};
char im9constundo[]={' ','0','.','5','0','0','\0','\0','\0','\0','\0','\0'};
float lim1=10;
char lim1buf[] ={' ','1','0','\0','\0','\0','\0','\0','\0','\0','\0','\0'};
char lim1undo[]={' ','1','0','\0','\0','\0','\0','\0','\0','\0','\0','\0'};
float lim2=10;
char lim2buf[] ={' ','1','0','\0','\0','\0','\0','\0','\0','\0','\0','\0'};
char lim2undo[]={' ','1','0','\0','\0','\0','\0','\0','\0','\0','\0','\0'};
float lim3=10;
char lim3buf[] ={' ','1','0','\0','\0','\0','\0','\0','\0','\0','\0','\0'};
char lim3undo[]={' ','1','0','\0','\0','\0','\0','\0','\0','\0','\0','\0'};
float lim4=0.00001;
char lim4buf[] ={' ','0','.','0','0','0','0','1','\0','\0','\0','\0'};
char lim4undo[]={' ','0','.','0','0','0','0','1','\0','\0','\0','\0'};



float magre=4.00;
char magrebuf[] ={' ','4','.','0','0','0','\0','\0','\0','\0','\0','\0'};
char magreundo[]={' ','4','.','0','0','0','\0','\0','\0','\0','\0','\0'};
float magim=0;
char magimbuf[] ={' ','0','.','0','0','0','\0','\0','\0','\0','\0','\0'};
char magimundo[]={' ','0','.','0','0','0','\0','\0','\0','\0','\0','\0'};
float lambda=2.96;
char lambdabuf[] ={' ','2','.','9','6','0','\0','\0','\0','\0','\0','\0'};
char lambdaundo[]={' ','2','.','9','6','0','\0','\0','\0','\0','\0','\0'};
float lamexp=1.0;
char lamexpbuf[] ={' ','1','.','0','0','0','\0','\0','\0','\0','\0','\0'};
char lamexpundo[]={' ','1','.','0','0','0','\0','\0','\0','\0','\0','\0'};
float epsilon=0.2;
char epsilonbuf[] ={' ','0','.','2','0','0','\0','\0','\0','\0','\0','\0'};
char epsilonundo[]={' ','0','.','2','0','0','\0','\0','\0','\0','\0','\0'};
short maxiter=40;
char maxiterbuf[] ={' ','4','0','\0','\0','\0','\0','\0','\0','\0','\0','\0'};
char maxiterundo[]={' ','4','0','\0','\0','\0','\0','\0','\0','\0','\0','\0'};
float bioreconst=0.5;
char biorebuf[] ={' ','0','.','5','0','0','\0','\0','\0','\0','\0','\0'};
char bioreundo[]={' ','0','.','5','0','0','\0','\0','\0','\0','\0','\0'};
float bioimconst=0.0;
char bioimbuf[] ={' ','0','.','0','0','0','\0','\0','\0','\0','\0','\0'};
char bioimundo[]={' ','0','.','0','0','0','\0','\0','\0','\0','\0','\0'};
float heightconst=10;
char hcbuf[] ={' ','1','0','.','0','0','0','\0','\0','\0','\0','\0'};
char hcundo[]={' ','1','0','.','0','0','0','\0','\0','\0','\0','\0'};
short radius=80;
char radiusbuf[] ={' ','8','0','\0','\0','\0','\0','\0','\0','\0','\0','\0'};
char radiusundo[]={' ','8','0','\0','\0','\0','\0','\0','\0','\0','\0','\0'};
short step=1;
char stepbuf[] ={' ','1','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'};
char stepundo[]={' ','1','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'};
char namebuf[] ={'F','r','a','c','.','p','i','c','\0','\0','\0','\0'
                ,'\0','\0','\0','\0','\0','\0'};
char nameundo[]={'F','r','a','c','.','p','i','c','\0','\0','\0','\0'
                ,'\0','\0','\0','\0','\0','\0'};

short rulelen=4;
char rulebuf[] ={'K','F','A','q','\0','\0','\0','\0','\0','\0','\0','\0'
                ,'\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'
                ,'\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'
                ,'\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'
                ,'\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'
                ,'\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'
                ,'\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'
                ,'\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'
                ,'\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'
                };

char ruleundo[]={'K','F','A','q','\0','\0','\0','\0','\0','\0','\0','\0'
                ,'\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'
                ,'\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'
                ,'\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'
                ,'\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'
                ,'\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'
                ,'\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'
                ,'\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'
                ,'\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'
                };


/* formulas2.c */
float str_a=2.24;
char str_abuf[] ={' ','2','.','2','4','0','\0','\0','\0','\0','\0','\0'};
char str_aundo[]={' ','2','.','2','4','0','\0','\0','\0','\0','\0','\0'};
float str_b=0.43;
char str_bbuf[] ={' ','0','.','4','3','0','\0','\0','\0','\0','\0','\0'};
char str_bundo[]={' ','0','.','4','3','0','\0','\0','\0','\0','\0','\0'};
float str_c=-0.65;
char str_cbuf[] ={'-','0','.','6','5','0','\0','\0','\0','\0','\0','\0'};
char str_cundo[]={'-','0','.','6','5','0','\0','\0','\0','\0','\0','\0'};
float str_d=-2.43;
char str_dbuf[] ={'-','2','.','4','3','0','\0','\0','\0','\0','\0','\0'};
char str_dundo[]={'-','2','.','4','3','0','\0','\0','\0','\0','\0','\0'};
float str_e=1;
char str_ebuf[] ={' ','1','.','0','0','0','\0','\0','\0','\0','\0','\0'};
char str_eundo[]={' ','1','.','0','0','0','\0','\0','\0','\0','\0','\0'};
float c_xmin=-20;
char c_xminbuf[] ={'-','2','0','.','0','0','\0','\0','\0','\0','\0','\0'};
char c_xminundo[]={'-','2','0','.','0','0','\0','\0','\0','\0','\0','\0'};
float c_xmax=20;
char c_xmaxbuf[] ={' ','2','0','.','0','0','\0','\0','\0','\0','\0','\0'};
char c_xmaxundo[]={' ','2','0','.','0','0','\0','\0','\0','\0','\0','\0'};
float c_ymin=-20;
char c_yminbuf[] ={'-','2','0','.','0','0','\0','\0','\0','\0','\0','\0'};
char c_yminundo[]={'-','2','0','.','0','0','\0','\0','\0','\0','\0','\0'};
float c_ymax=20;
char c_ymaxbuf[] ={' ','2','0','.','0','0','\0','\0','\0','\0','\0','\0'};
char c_ymaxundo[]={' ','2','0','.','0','0','\0','\0','\0','\0','\0','\0'};
float c_p=3.0;
char c_pbuf[] ={' ','3','.','0','0','0','\0','\0','\0','\0','\0','\0'};
char c_pundo[]={' ','3','.','0','0','0','\0','\0','\0','\0','\0','\0'};
float c_h=0.1;
char c_hbuf[] ={' ','0','.','1','0','0','\0','\0','\0','\0','\0','\0'};
char c_hundo[]={' ','0','.','1','0','0','\0','\0','\0','\0','\0','\0'};
short c_res=50;
char c_resbuf[] ={' ','5','0','\0','\0','\0','\0','\0','\0','\0','\0','\0'};
char c_resundo[]={' ','5','0','\0','\0','\0','\0','\0','\0','\0','\0','\0'};
short c_iter=30;
char c_iterbuf[] ={' ','3','0','\0','\0','\0','\0','\0','\0','\0','\0','\0'};
char c_iterundo[]={' ','3','0','\0','\0','\0','\0','\0','\0','\0','\0','\0'};
/* end formulas2.c */

static struct StringInfo sti[]=
{ {(UBYTE *)&reminbuf[0],(UBYTE *)&reminundo[0],0,11,0,0,0,0,0,0,NULL,NULL,NULL}
, {(UBYTE *)&remaxbuf[0],(UBYTE *)&remaxundo[0],0,11,0,0,0,0,0,0,NULL,NULL,NULL}
, {(UBYTE *)&imminbuf[0],(UBYTE *)&imminundo[0],0,11,0,0,0,0,0,0,NULL,NULL,NULL}
, {(UBYTE *)&immaxbuf[0],(UBYTE *)&immaxundo[0],0,11,0,0,0,0,0,0,NULL,NULL,NULL}
, {(UBYTE *)&lambdabuf[0],(UBYTE *)&lambdaundo[0],0,11,0,0,0,0,0,0,NULL,NULL,NULL}
  /* 5 */
, {(UBYTE *)&maxiterbuf[0],(UBYTE *)&maxiterundo[0],0,11,0,0,0,0,0,0,NULL,NULL,NULL}
, {(UBYTE *)&epsilonbuf[0],(UBYTE *)&epsilonundo[0],0,11,0,0,0,0,0,0,NULL,NULL,NULL}
, {(UBYTE *)&reconstbuf[0],(UBYTE *)&reconstundo[0],0,11,0,0,0,0,0,0,NULL,NULL,NULL}
, {(UBYTE *)&imconstbuf[0],(UBYTE *)&imconstundo[0],0,11,0,0,0,0,0,0,NULL,NULL,NULL}
, {(UBYTE *)&lamexpbuf[0],(UBYTE *)&lamexpundo[0],0,11,0,0,0,0,0,0,NULL,NULL,NULL}
  /* 10 */
, {(UBYTE *)&biorebuf[0],(UBYTE *)&bioreundo[0],0,11,0,0,0,0,0,0,NULL,NULL,NULL}
, {(UBYTE *)&bioimbuf[0],(UBYTE *)&bioimundo[0],0,11,0,0,0,0,0,0,NULL,NULL,NULL}
, {(UBYTE *)&hcbuf[0],(UBYTE *)&hcundo[0],0,11,0,0,0,0,0,0,NULL,NULL,NULL}
, {(UBYTE *)&radiusbuf[0],(UBYTE *)&radiusundo[0],0,11,0,0,0,0,0,0,NULL,NULL,NULL}
, {(UBYTE *)&stepbuf[0],(UBYTE *)&stepundo[0],0,11,0,0,0,0,0,0,NULL,NULL,NULL}
  /* 15 */
, {(UBYTE *)&namebuf[0],(UBYTE *)&nameundo[0],0,17,0,0,0,0,0,0,NULL,NULL,NULL}
, {(UBYTE *)&re0constbuf[0],(UBYTE *)&re0constundo[0],0,11,0,0,0,0,0,0,NULL,NULL,NULL}
, {(UBYTE *)&im0constbuf[0],(UBYTE *)&im0constundo[0],0,11,0,0,0,0,0,0,NULL,NULL,NULL}
, {(UBYTE *)&rulebuf[0],(UBYTE *)&ruleundo[0],0,105,0,0,0,0,0,0,NULL,NULL,NULL}
, {(UBYTE *)&magrebuf[0],(UBYTE *)&magreundo[0],0,11,0,0,0,0,0,0,NULL,NULL,NULL}
  /* 20 */
, {(UBYTE *)&namebuf[0],(UBYTE *)&nameundo[0],0,17,0,0,0,0,0,0,NULL,NULL,NULL}
, {(UBYTE *)&re1constbuf[0],(UBYTE *)&re1constundo[0],0,11,0,0,0,0,0,0,NULL,NULL,NULL}
, {(UBYTE *)&im1constbuf[0],(UBYTE *)&im1constundo[0],0,11,0,0,0,0,0,0,NULL,NULL,NULL}
, {(UBYTE *)&re2constbuf[0],(UBYTE *)&re2constundo[0],0,11,0,0,0,0,0,0,NULL,NULL,NULL}
, {(UBYTE *)&im2constbuf[0],(UBYTE *)&im2constundo[0],0,11,0,0,0,0,0,0,NULL,NULL,NULL}
  /* 25 */
, {(UBYTE *)&re3constbuf[0],(UBYTE *)&re3constundo[0],0,11,0,0,0,0,0,0,NULL,NULL,NULL}
, {(UBYTE *)&im3constbuf[0],(UBYTE *)&im3constundo[0],0,11,0,0,0,0,0,0,NULL,NULL,NULL}
, {(UBYTE *)&re4constbuf[0],(UBYTE *)&re4constundo[0],0,11,0,0,0,0,0,0,NULL,NULL,NULL}
, {(UBYTE *)&im4constbuf[0],(UBYTE *)&im4constundo[0],0,11,0,0,0,0,0,0,NULL,NULL,NULL}
, {(UBYTE *)&re5constbuf[0],(UBYTE *)&re5constundo[0],0,11,0,0,0,0,0,0,NULL,NULL,NULL}
  /* 30 */
, {(UBYTE *)&im5constbuf[0],(UBYTE *)&im5constundo[0],0,11,0,0,0,0,0,0,NULL,NULL,NULL}
, {(UBYTE *)&re6constbuf[0],(UBYTE *)&re6constundo[0],0,11,0,0,0,0,0,0,NULL,NULL,NULL}
, {(UBYTE *)&im6constbuf[0],(UBYTE *)&im6constundo[0],0,11,0,0,0,0,0,0,NULL,NULL,NULL}
, {(UBYTE *)&re7constbuf[0],(UBYTE *)&re7constundo[0],0,11,0,0,0,0,0,0,NULL,NULL,NULL}
, {(UBYTE *)&im7constbuf[0],(UBYTE *)&im7constundo[0],0,11,0,0,0,0,0,0,NULL,NULL,NULL}
  /* 35 */
, {(UBYTE *)&re8constbuf[0],(UBYTE *)&re8constundo[0],0,11,0,0,0,0,0,0,NULL,NULL,NULL}
, {(UBYTE *)&im8constbuf[0],(UBYTE *)&im8constundo[0],0,11,0,0,0,0,0,0,NULL,NULL,NULL}
, {(UBYTE *)&re9constbuf[0],(UBYTE *)&re9constundo[0],0,11,0,0,0,0,0,0,NULL,NULL,NULL}
, {(UBYTE *)&im9constbuf[0],(UBYTE *)&im9constundo[0],0,11,0,0,0,0,0,0,NULL,NULL,NULL}
, {(UBYTE *)&lim1buf[0],(UBYTE *)&lim1undo[0],0,11,0,0,0,0,0,0,NULL,NULL,NULL}
  /* 40 */
, {(UBYTE *)&lim2buf[0],(UBYTE *)&lim2undo[0],0,11,0,0,0,0,0,0,NULL,NULL,NULL}
, {(UBYTE *)&lim3buf[0],(UBYTE *)&lim3undo[0],0,11,0,0,0,0,0,0,NULL,NULL,NULL}
, {(UBYTE *)&lim4buf[0],(UBYTE *)&lim4undo[0],0,11,0,0,0,0,0,0,NULL,NULL,NULL}


};

/* Formulas1.c */
static struct StringInfo sti1[]=
{ {(UBYTE *)&str_abuf[0],(UBYTE *)&str_aundo[0],0,11,0,0,0,0,0,0,NULL,NULL,NULL}
, {(UBYTE *)&str_bbuf[0],(UBYTE *)&str_bundo[0],0,11,0,0,0,0,0,0,NULL,NULL,NULL}
, {(UBYTE *)&str_cbuf[0],(UBYTE *)&str_cundo[0],0,11,0,0,0,0,0,0,NULL,NULL,NULL}
, {(UBYTE *)&str_dbuf[0],(UBYTE *)&str_dundo[0],0,11,0,0,0,0,0,0,NULL,NULL,NULL}
, {(UBYTE *)&str_ebuf[0],(UBYTE *)&str_eundo[0],0,11,0,0,0,0,0,0,NULL,NULL,NULL}
  /* 5 */
, {(UBYTE *)&c_xminbuf[0],(UBYTE *)&c_xminundo[0],0,11,0,0,0,0,0,0,NULL,NULL,NULL}
, {(UBYTE *)&c_xmaxbuf[0],(UBYTE *)&c_xmaxundo[0],0,11,0,0,0,0,0,0,NULL,NULL,NULL}
, {(UBYTE *)&c_yminbuf[0],(UBYTE *)&c_yminundo[0],0,11,0,0,0,0,0,0,NULL,NULL,NULL}
, {(UBYTE *)&c_ymaxbuf[0],(UBYTE *)&c_ymaxundo[0],0,11,0,0,0,0,0,0,NULL,NULL,NULL}
, {(UBYTE *)&c_pbuf[0],(UBYTE *)&c_pundo[0],0,11,0,0,0,0,0,0,NULL,NULL,NULL}
  /* 10 */
, {(UBYTE *)&c_hbuf[0],(UBYTE *)&c_hundo[0],0,11,0,0,0,0,0,0,NULL,NULL,NULL}
, {(UBYTE *)&c_resbuf[0],(UBYTE *)&c_resundo[0],0,11,0,0,0,0,0,0,NULL,NULL,NULL}
, {(UBYTE *)&c_iterbuf[0],(UBYTE *)&c_iterundo[0],0,11,0,0,0,0,0,0,NULL,NULL,NULL}
};
/* formulas1.c end */


struct Gadget gad[]=
{ {NULL,0,0,90,9,(USHORT)GADGHCOMP,(USHORT)0,(USHORT)STRGADGET,
   NULL,NULL,NULL,NULL,(APTR)(sti+0),(USHORT)0,NULL}
, {NULL,0,0,90,9,(USHORT)GADGHCOMP,(USHORT)0,(USHORT)STRGADGET,
   NULL,NULL,NULL,NULL,(APTR)(sti+1),(USHORT)0,NULL}
, {NULL,0,0,90,9,(USHORT)GADGHCOMP,(USHORT)0,(USHORT)STRGADGET,
   NULL,NULL,NULL,NULL,(APTR)(sti+2),(USHORT)0,NULL}
, {NULL,0,0,90,9,(USHORT)GADGHCOMP,(USHORT)0,(USHORT)STRGADGET,
   NULL,NULL,NULL,NULL,(APTR)(sti+3),(USHORT)0,NULL}
, {NULL,0,0,90,9,(USHORT)GADGHCOMP,(USHORT)0,(USHORT)STRGADGET,
   NULL,NULL,NULL,NULL,(APTR)(sti+4),(USHORT)0,NULL}
   /* 5 */
, {NULL,0,0,90,9,(USHORT)GADGHCOMP,(USHORT)0,(USHORT)STRGADGET,
   NULL,NULL,NULL,NULL,(APTR)(sti+5),(USHORT)0,NULL}
, {NULL,0,0,90,9,(USHORT)GADGHCOMP,(USHORT)0,(USHORT)STRGADGET,
   NULL,NULL,NULL,NULL,(APTR)(sti+6),(USHORT)0,NULL}
, {NULL,0,0,90,9,(USHORT)GADGHCOMP,(USHORT)0,(USHORT)STRGADGET,
   NULL,NULL,NULL,NULL,(APTR)(sti+7),(USHORT)0,NULL}
, {NULL,0,0,90,9,(USHORT)GADGHCOMP,(USHORT)0,(USHORT)STRGADGET,
   NULL,NULL,NULL,NULL,(APTR)(sti+8),(USHORT)0,NULL}
, {NULL,0,0,90,9,(USHORT)GADGHCOMP,(USHORT)0,(USHORT)STRGADGET,
   NULL,NULL,NULL,NULL,(APTR)(sti+9),(USHORT)0,NULL}
/* 10 */
, {NULL,0,0,90,9,(USHORT)GADGHCOMP,(USHORT)0,(USHORT)STRGADGET,
   NULL,NULL,NULL,NULL,(APTR)(sti+10),(USHORT)0,NULL}
, {NULL,0,0,90,9,(USHORT)GADGHCOMP,(USHORT)0,(USHORT)STRGADGET,
   NULL,NULL,NULL,NULL,(APTR)(sti+11),(USHORT)0,NULL}
, {NULL,0,0,90,9,(USHORT)GADGHCOMP,(USHORT)0,(USHORT)STRGADGET,
   NULL,NULL,NULL,NULL,(APTR)(sti+12),(USHORT)0,NULL}
, {NULL,0,0,90,9,(USHORT)GADGHCOMP,(USHORT)0,(USHORT)STRGADGET,
   NULL,NULL,NULL,NULL,(APTR)(sti+13),(USHORT)0,NULL}
, {NULL,0,0,90,9,(USHORT)GADGHCOMP,(USHORT)0,(USHORT)STRGADGET,
   NULL,NULL,NULL,NULL,(APTR)(sti+14),(USHORT)0,NULL}
   /* 15 */
, {NULL,0,0,140,9,(USHORT)GADGHCOMP,(USHORT)0,(USHORT)STRGADGET,
   NULL,NULL,NULL,NULL,(APTR)(sti+15),(USHORT)0,NULL}
, {NULL,0,0,90,9,(USHORT)GADGHCOMP,(USHORT)0,(USHORT)STRGADGET,
   NULL,NULL,NULL,NULL,(APTR)(sti+16),(USHORT)0,NULL}
, {NULL,0,0,90,9,(USHORT)GADGHCOMP,(USHORT)0,(USHORT)STRGADGET,
   NULL,NULL,NULL,NULL,(APTR)(sti+17),(USHORT)0,NULL}
, {NULL,0,0,300,9,(USHORT)GADGHCOMP,(USHORT)0,(USHORT)STRGADGET,
   NULL,NULL,NULL,NULL,(APTR)(sti+18),(USHORT)0,NULL}
, {NULL,0,0,90,9,(USHORT)GADGHCOMP,(USHORT)0,(USHORT)STRGADGET,
   NULL,NULL,NULL,NULL,(APTR)(sti+19),(USHORT)0,NULL}
   /* 20 */
, {NULL,0,0,90,9,(USHORT)GADGHCOMP,(USHORT)0,(USHORT)STRGADGET,
   NULL,NULL,NULL,NULL,(APTR)(sti+20),(USHORT)0,NULL}
, {NULL,0,0,90,9,(USHORT)GADGHCOMP,(USHORT)0,(USHORT)STRGADGET,
   NULL,NULL,NULL,NULL,(APTR)(sti+21),(USHORT)0,NULL}
, {NULL,0,0,90,9,(USHORT)GADGHCOMP,(USHORT)0,(USHORT)STRGADGET,
   NULL,NULL,NULL,NULL,(APTR)(sti+22),(USHORT)0,NULL}
, {NULL,0,0,90,9,(USHORT)GADGHCOMP,(USHORT)0,(USHORT)STRGADGET,
   NULL,NULL,NULL,NULL,(APTR)(sti+23),(USHORT)0,NULL}
, {NULL,0,0,90,9,(USHORT)GADGHCOMP,(USHORT)0,(USHORT)STRGADGET,
   NULL,NULL,NULL,NULL,(APTR)(sti+24),(USHORT)0,NULL}
   /* 25 */
, {NULL,0,0,90,9,(USHORT)GADGHCOMP,(USHORT)0,(USHORT)STRGADGET,
   NULL,NULL,NULL,NULL,(APTR)(sti+25),(USHORT)0,NULL}
, {NULL,0,0,90,9,(USHORT)GADGHCOMP,(USHORT)0,(USHORT)STRGADGET,
   NULL,NULL,NULL,NULL,(APTR)(sti+26),(USHORT)0,NULL}
, {NULL,0,0,90,9,(USHORT)GADGHCOMP,(USHORT)0,(USHORT)STRGADGET,
   NULL,NULL,NULL,NULL,(APTR)(sti+27),(USHORT)0,NULL}
, {NULL,0,0,90,9,(USHORT)GADGHCOMP,(USHORT)0,(USHORT)STRGADGET,
   NULL,NULL,NULL,NULL,(APTR)(sti+28),(USHORT)0,NULL}
, {NULL,0,0,90,9,(USHORT)GADGHCOMP,(USHORT)0,(USHORT)STRGADGET,
   NULL,NULL,NULL,NULL,(APTR)(sti+29),(USHORT)0,NULL}
   /* 30 */
, {NULL,0,0,90,9,(USHORT)GADGHCOMP,(USHORT)0,(USHORT)STRGADGET,
   NULL,NULL,NULL,NULL,(APTR)(sti+30),(USHORT)0,NULL}
, {NULL,0,0,90,9,(USHORT)GADGHCOMP,(USHORT)0,(USHORT)STRGADGET,
   NULL,NULL,NULL,NULL,(APTR)(sti+31),(USHORT)0,NULL}
, {NULL,0,0,90,9,(USHORT)GADGHCOMP,(USHORT)0,(USHORT)STRGADGET,
   NULL,NULL,NULL,NULL,(APTR)(sti+32),(USHORT)0,NULL}
, {NULL,0,0,90,9,(USHORT)GADGHCOMP,(USHORT)0,(USHORT)STRGADGET,
   NULL,NULL,NULL,NULL,(APTR)(sti+33),(USHORT)0,NULL}
, {NULL,0,0,90,9,(USHORT)GADGHCOMP,(USHORT)0,(USHORT)STRGADGET,
   NULL,NULL,NULL,NULL,(APTR)(sti+34),(USHORT)0,NULL}
   /* 35 */
, {NULL,0,0,90,9,(USHORT)GADGHCOMP,(USHORT)0,(USHORT)STRGADGET,
   NULL,NULL,NULL,NULL,(APTR)(sti+35),(USHORT)0,NULL}
, {NULL,0,0,90,9,(USHORT)GADGHCOMP,(USHORT)0,(USHORT)STRGADGET,
   NULL,NULL,NULL,NULL,(APTR)(sti+36),(USHORT)0,NULL}
, {NULL,0,0,90,9,(USHORT)GADGHCOMP,(USHORT)0,(USHORT)STRGADGET,
   NULL,NULL,NULL,NULL,(APTR)(sti+37),(USHORT)0,NULL}
, {NULL,0,0,90,9,(USHORT)GADGHCOMP,(USHORT)0,(USHORT)STRGADGET,
   NULL,NULL,NULL,NULL,(APTR)(sti+38),(USHORT)0,NULL}
, {NULL,0,0,90,9,(USHORT)GADGHCOMP,(USHORT)0,(USHORT)STRGADGET,
   NULL,NULL,NULL,NULL,(APTR)(sti+39),(USHORT)0,NULL}
   /* 40 */
, {NULL,0,0,90,9,(USHORT)GADGHCOMP,(USHORT)0,(USHORT)STRGADGET,
   NULL,NULL,NULL,NULL,(APTR)(sti+40),(USHORT)0,NULL}
, {NULL,0,0,90,9,(USHORT)GADGHCOMP,(USHORT)0,(USHORT)STRGADGET,
   NULL,NULL,NULL,NULL,(APTR)(sti+41),(USHORT)0,NULL}
, {NULL,0,0,90,9,(USHORT)GADGHCOMP,(USHORT)0,(USHORT)STRGADGET,
   NULL,NULL,NULL,NULL,(APTR)(sti+42),(USHORT)0,NULL}

};

/* formulas1.c */
struct Gadget gad1[]=
{ {NULL,0,0,90,9,(USHORT)GADGHCOMP,(USHORT)0,(USHORT)STRGADGET,
   NULL,NULL,NULL,NULL,(APTR)(sti1+0),(USHORT)0,NULL}
, {NULL,0,0,90,9,(USHORT)GADGHCOMP,(USHORT)0,(USHORT)STRGADGET,
   NULL,NULL,NULL,NULL,(APTR)(sti1+1),(USHORT)0,NULL}
, {NULL,0,0,90,9,(USHORT)GADGHCOMP,(USHORT)0,(USHORT)STRGADGET,
   NULL,NULL,NULL,NULL,(APTR)(sti1+2),(USHORT)0,NULL}
, {NULL,0,0,90,9,(USHORT)GADGHCOMP,(USHORT)0,(USHORT)STRGADGET,
   NULL,NULL,NULL,NULL,(APTR)(sti1+3),(USHORT)0,NULL}
, {NULL,0,0,90,9,(USHORT)GADGHCOMP,(USHORT)0,(USHORT)STRGADGET,
   NULL,NULL,NULL,NULL,(APTR)(sti1+4),(USHORT)0,NULL}
   /* 5 */
, {NULL,0,0,90,9,(USHORT)GADGHCOMP,(USHORT)0,(USHORT)STRGADGET,
   NULL,NULL,NULL,NULL,(APTR)(sti1+5),(USHORT)0,NULL}
, {NULL,0,0,90,9,(USHORT)GADGHCOMP,(USHORT)0,(USHORT)STRGADGET,
   NULL,NULL,NULL,NULL,(APTR)(sti1+6),(USHORT)0,NULL}
, {NULL,0,0,90,9,(USHORT)GADGHCOMP,(USHORT)0,(USHORT)STRGADGET,
   NULL,NULL,NULL,NULL,(APTR)(sti1+7),(USHORT)0,NULL}
, {NULL,0,0,90,9,(USHORT)GADGHCOMP,(USHORT)0,(USHORT)STRGADGET,
   NULL,NULL,NULL,NULL,(APTR)(sti1+8),(USHORT)0,NULL}
, {NULL,0,0,90,9,(USHORT)GADGHCOMP,(USHORT)0,(USHORT)STRGADGET,
   NULL,NULL,NULL,NULL,(APTR)(sti1+9),(USHORT)0,NULL}
  /* 10 */
, {NULL,0,0,90,9,(USHORT)GADGHCOMP,(USHORT)0,(USHORT)STRGADGET,
   NULL,NULL,NULL,NULL,(APTR)(sti1+10),(USHORT)0,NULL}
, {NULL,0,0,90,9,(USHORT)GADGHCOMP,(USHORT)0,(USHORT)STRGADGET,
   NULL,NULL,NULL,NULL,(APTR)(sti1+11),(USHORT)0,NULL}
, {NULL,0,0,90,9,(USHORT)GADGHCOMP,(USHORT)0,(USHORT)STRGADGET,
   NULL,NULL,NULL,NULL,(APTR)(sti1+12),(USHORT)0,NULL}
};
/* formulas1.c end */


static struct IntuiText git[] =
{ {1,0,JAM2,1,1,NULL,(UBYTE *)"  OK  ",NULL}
, {1,0,JAM2,1,1,NULL,(UBYTE *)"CANCEL",NULL}
, {1,0,JAM2,1,1,NULL,(UBYTE *)" SAVE ",NULL}
};

struct Gadget gadb[] =
{ {NULL,100,130,50,9,(USHORT)GADGHCOMP,(USHORT)RELVERIFY,
   (USHORT)BOOLGADGET,NULL,NULL,(git+0),NULL,NULL,(USHORT)0,NULL}
, {NULL,100,130,50,9,(USHORT)GADGHCOMP,(USHORT)RELVERIFY,
   (USHORT)BOOLGADGET,NULL,NULL,(git+1),NULL,NULL,(USHORT)1,NULL}
, {NULL,100,130,50,9,(USHORT)GADGHCOMP,(USHORT)RELVERIFY,
   (USHORT)BOOLGADGET,NULL,NULL,(git+2),NULL,NULL,(USHORT)2,NULL}
};

static struct IntuiText it[] =
{ {(UBYTE)0,(UBYTE)1,(UBYTE)JAM2,2,0,NULL,(UBYTE *) "About",NULL}
, {(UBYTE)0,(UBYTE)1,(UBYTE)JAM2,2,0,NULL,(UBYTE *) "Start",NULL}
, {(UBYTE)0,(UBYTE)1,(UBYTE)JAM2,2,0,NULL,(UBYTE *) "Color",NULL}
, {(UBYTE)0,(UBYTE)1,(UBYTE)JAM2,2,0,NULL,(UBYTE *) "Save ",NULL}
, {(UBYTE)0,(UBYTE)1,(UBYTE)JAM2,12,0,NULL,(UBYTE *)"lCOSz    ",NULL}
  /* 5 */
, {(UBYTE)0,(UBYTE)1,(UBYTE)JAM2,12,0,NULL,(UBYTE *)"eSINz    ",NULL}
, {(UBYTE)0,(UBYTE)1,(UBYTE)JAM2,2,0,NULL,(UBYTE *) "Quit ",NULL}
, {(UBYTE)0,(UBYTE)1,(UBYTE)JAM2,12,0,NULL,(UBYTE *)"LoRes ",NULL}
, {(UBYTE)0,(UBYTE)1,(UBYTE)JAM2,12,0,NULL,(UBYTE *)"HiRes ",NULL}
, {(UBYTE)0,(UBYTE)1,(UBYTE)JAM2,12,0,NULL,(UBYTE *)"NoLace",NULL}
  /* 10 */
, {(UBYTE)0,(UBYTE)1,(UBYTE)JAM2,12,0,NULL,(UBYTE *)"Lace  ",NULL}
, {(UBYTE)0,(UBYTE)1,(UBYTE)JAM2,2,0,NULL,(UBYTE *) "Title",NULL}
, {(UBYTE)0,(UBYTE)1,(UBYTE)JAM2,2,0,NULL,(UBYTE *) "     ",NULL}
, {(UBYTE)0,(UBYTE)1,(UBYTE)JAM2,2,0,NULL,(UBYTE *) "Stop ",NULL}
, {(UBYTE)0,(UBYTE)1,(UBYTE)JAM2,2,0,NULL,(UBYTE *) "Color",NULL}
  /* 15 */
, {(UBYTE)0,(UBYTE)1,(UBYTE)JAM2,2,0,NULL,(UBYTE *) "Title",NULL}
, {(UBYTE)0,(UBYTE)1,(UBYTE)JAM2,12,0,NULL,(UBYTE *)"Mandel   ",NULL}
, {(UBYTE)0,(UBYTE)1,(UBYTE)JAM2,12,0,NULL,(UBYTE *)"Julia    ",NULL}
, {(UBYTE)0,(UBYTE)1,(UBYTE)JAM2,12,0,NULL,(UBYTE *)"2D-Rect",NULL}
, {(UBYTE)0,(UBYTE)1,(UBYTE)JAM2,12,0,NULL,(UBYTE *)"3D-Rect",NULL}
  /* 20 */
, {(UBYTE)0,(UBYTE)1,(UBYTE)JAM2,12,0,NULL,(UBYTE *)"lEXPz    ",NULL}
, {(UBYTE)0,(UBYTE)1,(UBYTE)JAM2,12,0,NULL,(UBYTE *)"Biomorph ",NULL}
, {(UBYTE)0,(UBYTE)1,(UBYTE)JAM2,12,0,NULL,(UBYTE *)"Z3+C     ",NULL}
, {(UBYTE)0,(UBYTE)1,(UBYTE)JAM2,12,0,NULL,(UBYTE *)"Z5+C     ",NULL}
, {(UBYTE)0,(UBYTE)1,(UBYTE)JAM2,12,0,NULL,(UBYTE *)"   Type  ",NULL}
  /* 25 */
, {(UBYTE)0,(UBYTE)1,(UBYTE)JAM2,12,0,NULL,(UBYTE *)"Sphere ",NULL}
, {(UBYTE)0,(UBYTE)1,(UBYTE)JAM2,12,0,NULL,(UBYTE *)"ZZ+Z5+C  ",NULL}
, {(UBYTE)0,(UBYTE)1,(UBYTE)JAM2,12,0,NULL,(UBYTE *)"ZZ+Z6+C  ",NULL}
, {(UBYTE)0,(UBYTE)1,(UBYTE)JAM2,12,0,NULL,(UBYTE *)"SINZ+Z2+C",NULL}
, {(UBYTE)0,(UBYTE)1,(UBYTE)JAM2,12,0,NULL,(UBYTE *)"SINZ+EZ+C",NULL}
  /* 30 */
, {(UBYTE)0,(UBYTE)1,(UBYTE)JAM2,12,0,NULL,(UBYTE *)"Newton-R ",NULL}
, {(UBYTE)0,(UBYTE)1,(UBYTE)JAM2,12,0,NULL,(UBYTE *)"   Type  ",NULL}
, {(UBYTE)0,(UBYTE)1,(UBYTE)JAM2,12,0,NULL,(UBYTE *)"Z3-1     ",NULL}
, {(UBYTE)0,(UBYTE)1,(UBYTE)JAM2,12,0,NULL,(UBYTE *)"Z4-1     ",NULL}
, {(UBYTE)0,(UBYTE)1,(UBYTE)JAM2,12,0,NULL,(UBYTE *)"Z5-1     ",NULL}
  /* 35 */
, {(UBYTE)0,(UBYTE)1,(UBYTE)JAM2,12,0,NULL,(UBYTE *)"Z5+Z3-1  ",NULL}
, {(UBYTE)0,(UBYTE)1,(UBYTE)JAM2,12,0,NULL,(UBYTE *)"         ",NULL}
, {(UBYTE)0,(UBYTE)1,(UBYTE)JAM2,12,0,NULL,(UBYTE *)"         ",NULL}
, {(UBYTE)0,(UBYTE)1,(UBYTE)JAM2,12,0,NULL,(UBYTE *)"Ultimate ",NULL}
, {(UBYTE)0,(UBYTE)1,(UBYTE)JAM2,12,0,NULL,(UBYTE *)"Magnet1 x",NULL}
  /* 40 */
, {(UBYTE)0,(UBYTE)1,(UBYTE)JAM2,12,0,NULL,(UBYTE *)"Magnet1 q",NULL}
, {(UBYTE)0,(UBYTE)1,(UBYTE)JAM2,12,0,NULL,(UBYTE *)"Magnet2 x",NULL}
, {(UBYTE)0,(UBYTE)1,(UBYTE)JAM2,12,0,NULL,(UBYTE *)"Magnet2 q",NULL}
, {(UBYTE)0,(UBYTE)1,(UBYTE)JAM2,12,0,NULL,(UBYTE *)"NEXT  (2)",NULL}
, {(UBYTE)0,(UBYTE)1,(UBYTE)JAM2,12,0,NULL,(UBYTE *)"NEXT  (1)",NULL}
  /* 45 */
, {(UBYTE)0,(UBYTE)1,(UBYTE)JAM2,12,0,NULL,(UBYTE *)"S-Attract",NULL}
, {(UBYTE)0,(UBYTE)1,(UBYTE)JAM2,12,0,NULL,(UBYTE *)"Cycl.syst",NULL}
, {(UBYTE)0,(UBYTE)1,(UBYTE)JAM2,12,0,NULL,(UBYTE *)"   Type  ",NULL}
, {(UBYTE)0,(UBYTE)1,(UBYTE)JAM2,12,0,NULL,(UBYTE *)"C Syst.  0",NULL}
, {(UBYTE)0,(UBYTE)1,(UBYTE)JAM2,12,0,NULL,(UBYTE *)"C Syst.  1",NULL}
  /* 50 */
, {(UBYTE)0,(UBYTE)1,(UBYTE)JAM2,12,0,NULL,(UBYTE *)"C Syst.  2",NULL}
, {(UBYTE)0,(UBYTE)1,(UBYTE)JAM2,12,0,NULL,(UBYTE *)"C Syst.  3",NULL}
, {(UBYTE)0,(UBYTE)1,(UBYTE)JAM2,12,0,NULL,(UBYTE *)"C Syst.  4",NULL}
, {(UBYTE)0,(UBYTE)1,(UBYTE)JAM2,12,0,NULL,(UBYTE *)"C Syst.  5",NULL}
};

#define MIF0 ITEMTEXT|HIGHBOX|ITEMENABLED
#define MIF1 ITEMTEXT|HIGHBOX|ITEMENABLED|COMMSEQ
#define MIF2 ITEMTEXT|HIGHBOX|ITEMENABLED|CHECKIT
#define MIF3 ITEMTEXT|HIGHBOX|ITEMENABLED|CHECKIT|CHECKED

struct MenuItem mit[] =
{ {(mit+11),5,0,70,10,(USHORT)MIF0,NULL,(APTR)(it+0),NULL,
   (BYTE)0,NULL,(USHORT)0}
, {(mit+12),5,20,70,10,(USHORT)MIF1,NULL,(APTR)(it+1),NULL,
   (BYTE)'S',NULL,(USHORT)0}
, {(mit+3),5,40,70,10,(USHORT)MIF1,NULL,(APTR)(it+2),NULL,
   (BYTE)'C',NULL,(USHORT)0}
, {(mit+6),5,50,70,10,(USHORT)MIF0,NULL,(APTR)(it+3),NULL,
   (BYTE)0,NULL,(USHORT)0}
, {(mit+5),5,40,90,10,(USHORT)MIF3,~(1L<<LCOSZ),(APTR)(it+4),NULL,
   (BYTE)0,NULL,(USHORT)0}
   /* 5 */
, {(mit+20),5,50,90,10,(USHORT)MIF2,~(1L<<ESINZ),(APTR)(it+5),NULL,
   (BYTE)0,NULL,(USHORT)0}
, {NULL,5,60,70,10,(USHORT)MIF1,NULL,(APTR)(it+6),NULL,
   (BYTE)'Q',NULL,(USHORT)0}
, {(mit+8),5,0,70,10,(USHORT)MIF3,2L,(APTR)(it+7),NULL,
   (BYTE)0,NULL,(USHORT)0}
, {(mit+9),5,10,70,10,(USHORT)MIF2,1L,(APTR)(it+8),NULL,
   (BYTE)0,NULL,(USHORT)0}
, {(mit+10),5,20,70,10,(USHORT)MIF3,8L,(APTR)(it+9),NULL,
   (BYTE)0,NULL,(USHORT)0}
   /* 10 */
, {NULL,5,30,70,10,(USHORT)MIF2,4L,(APTR)(it+10),NULL,
   (BYTE)0,NULL,(USHORT)0}
, {(mit+1),5,10,70,10,(USHORT)MIF1,NULL,(APTR)(it+11),NULL,
   (BYTE)'T',NULL,(USHORT)0}
, {(mit+2),5,30,70,10,(USHORT)MIF0,NULL,(APTR)(it+12),NULL,
   (BYTE)0,NULL,(USHORT)0}
, {(mit+14),5,0,70,10,(USHORT)MIF1,NULL,(APTR)(it+13),NULL,
   (BYTE)'S',NULL,(USHORT)0}
, {(mit+15),5,10,70,10,(USHORT)MIF1,NULL,(APTR)(it+14),NULL,
   (BYTE)'C',NULL,(USHORT)0}
   /* 15 */
, {NULL,5,20,70,10,(USHORT)MIF1,NULL,(APTR)(it+15),NULL,
   (BYTE)'T',NULL,(USHORT)0}
, {(mit+17),5,10,90,10,(USHORT)MIF2,~(1L<<MANDELBROT),(APTR)(it+16),NULL,
   (BYTE)0,NULL,(USHORT)0}
, {(mit+38),5,20,90,10,(USHORT)MIF2,~(1L<<JULIA),(APTR)(it+17),NULL,
   (BYTE)0,NULL,(USHORT)0}
, {(mit+19),5,0,80,10,(USHORT)MIF3,~(1L<<D2RECT),(APTR)(it+18),NULL,
   (BYTE)0,NULL,(USHORT)0}
, {(mit+25),5,10,80,10,(USHORT)MIF2,~(1L<<D3RECT),(APTR)(it+19),NULL,
   (BYTE)0,NULL,(USHORT)0}
   /* 20 */
, {(mit+21),5,60,90,10,(USHORT)MIF2,~(1L<<LEXPZ),(APTR)(it+20),NULL,
   (BYTE)0,NULL,(USHORT)0}
, {(mit+24),5,70,90,10,(USHORT)MIF2,~(1L<<BIOMORPH),(APTR)(it+21),NULL,
   (BYTE)0,NULL,(USHORT)0}
, {(mit+23),80,-20,100,10,(USHORT)MIF3,~(1L<<BIO0),(APTR)(it+22),NULL,
   (BYTE)0,NULL,(USHORT)0}
, {(mit+26),80,-10,100,10,(USHORT)MIF2,~(1L<<BIO1),(APTR)(it+23),NULL,
   (BYTE)0,NULL,(USHORT)0}
, {(mit+30),5,80,90,10,(USHORT)MIF2,NULL,(APTR)(it+24),NULL,
   (BYTE)0,(mit+22),(USHORT)0}
   /* 25 */
, {NULL,5,20,80,10,(USHORT)MIF2,~(1L<<SPHERE),(APTR)(it+25),NULL,
   (BYTE)0,NULL,(USHORT)0}
, {(mit+27),80,0,100,10,(USHORT)MIF2,~(1L<<BIO2),(APTR)(it+26),NULL,
   (BYTE)0,NULL,(USHORT)0}
, {(mit+28),80,10,100,10,(USHORT)MIF2,~(1L<<BIO3),(APTR)(it+27),NULL,
   (BYTE)0,NULL,(USHORT)0}
, {(mit+29),80,20,100,10,(USHORT)MIF2,~(1L<<BIO4),(APTR)(it+28),NULL,
   (BYTE)0,NULL,(USHORT)0}
, {NULL,80,30,100,10,(USHORT)MIF2,~(1L<<BIO5),(APTR)(it+29),NULL,
   (BYTE)0,NULL,(USHORT)0}
   /* 30 */
, {(mit+31),5,90,90,10,(USHORT)MIF2,~(1L<<NEWTON),(APTR)(it+30),NULL,
   (BYTE)0,NULL,(USHORT)0}
, {(mit+39),5,100,90,10,(USHORT)MIF2,NULL,(APTR)(it+31),NULL,
   (BYTE)0,(mit+32),(USHORT)0}
, {(mit+33),80,-20,100,10,(USHORT)MIF3,~(1L<<NR0),(APTR)(it+32),NULL,
   (BYTE)0,NULL,(USHORT)0}
, {(mit+34),80,-10,100,10,(USHORT)MIF2,~(1L<<NR1),(APTR)(it+33),NULL,
   (BYTE)0,NULL,(USHORT)0}
, {(mit+35),80,0,100,10,(USHORT)MIF2,~(1L<<NR2),(APTR)(it+34),NULL,
   (BYTE)0,NULL,(USHORT)0}
  /* 35 */
, {(mit+36),80,10,100,10,(USHORT)MIF2,~(1L<<NR3),(APTR)(it+35),NULL,
   (BYTE)0,NULL,(USHORT)0}
, {(mit+37),80,20,100,10,(USHORT)MIF2,~(1L<<NR4),(APTR)(it+36),NULL,
   (BYTE)0,NULL,(USHORT)0}
, {NULL,80,30,100,10,(USHORT)MIF2,~(1L<<NR5),(APTR)(it+37),NULL,
   (BYTE)0,NULL,(USHORT)0}
, {(mit+4),5,30,90,10,(USHORT)MIF2,~(1L<<ULTIMATE),(APTR)(it+38),NULL,
   (BYTE)0,NULL,(USHORT)0}
, {(mit+40),5,110,90,10,(USHORT)MIF2,~(1L<<MAGNET0),(APTR)(it+39),NULL,
   (BYTE)0,NULL,(USHORT)0}
  /* 40 */
, {(mit+41),5,120,90,10,(USHORT)MIF2,~(1L<<MAGNET1),(APTR)(it+40),NULL,
   (BYTE)0,NULL,(USHORT)0}
, {(mit+42),5,130,90,10,(USHORT)MIF2,~(1L<<MAGNET2),(APTR)(it+41),NULL,
   (BYTE)0,NULL,(USHORT)0}
, {NULL,5,140,90,10,(USHORT)MIF2,~(1L<<MAGNET3),(APTR)(it+42),NULL,
   (BYTE)0,NULL,(USHORT)0}
, {(mit+16),5,0,90,10,(USHORT)MIF0,NULL,(APTR)(it+43),NULL,
   (BYTE)0,NULL,(USHORT)0}
, {(mit+45),5,0,90,10,(USHORT)MIF0,NULL,(APTR)(it+44),NULL,
   (BYTE)0,NULL,(USHORT)0}
  /* 45 */
, {(mit+46),5,10,90,10,(USHORT)MIF3,~(1L<<STRATT),(APTR)(it+45),NULL,
   (BYTE)0,NULL,(USHORT)0}
, {(mit+47),5,20,90,10,(USHORT)MIF2,~(1L<<CYCLIC),(APTR)(it+46),NULL,
   (BYTE)0,NULL,(USHORT)0}
, {NULL,5,30,90,10,(USHORT)MIF2,NULL,(APTR)(it+47),NULL,
   (BYTE)0,(mit+48),(USHORT)0}
, {(mit+49),80,-20,100,10,(USHORT)MIF3,~(1L<<CYCLE0),(APTR)(it+48),NULL,
   (BYTE)0,NULL,(USHORT)0}
, {(mit+50),80,-10,100,10,(USHORT)MIF2,~(1L<<CYCLE1),(APTR)(it+49),NULL,
   (BYTE)0,NULL,(USHORT)0}
   /* 50 */
, {(mit+51),80,0,100,10,(USHORT)MIF2,~(1L<<CYCLE2),(APTR)(it+50),NULL,
   (BYTE)0,NULL,(USHORT)0}
, {(mit+52),80,10,100,10,(USHORT)MIF2,~(1L<<CYCLE3),(APTR)(it+51),NULL,
   (BYTE)0,NULL,(USHORT)0}
, {(mit+53),80,20,100,10,(USHORT)MIF2,~(1L<<CYCLE4),(APTR)(it+52),NULL,
   (BYTE)0,NULL,(USHORT)0}
, {NULL,80,30,100,10,(USHORT)MIF2,~(1L<<CYCLE5),(APTR)(it+53),NULL,
   (BYTE)0,NULL,(USHORT)0}
};


struct Menu men[] =
{ {(men+1),0,0,60,80,(USHORT)MENUENABLED,(BYTE *)"Project",(mit+0),0,0,0,0}
, {(men+2),65,0,60,80,(USHORT)MENUENABLED,(BYTE *)"Fractal",(mit+43),0,0,0,0}
, {(men+4),130,0,55,80,(USHORT)MENUENABLED,(BYTE *)"Screen",(mit+7),0,0,0,0}
, {NULL,0,0,100,20,(USHORT)MENUENABLED,(BYTE *)"Drawing...",(mit+13),0,0,0,0}
, {NULL,190,0,65,80,(USHORT)MENUENABLED,(BYTE *)"Surface",(mit+18),0,0,0,0}
  /* 5 */
, {(men+6),0,0,60,80,(USHORT)MENUENABLED,(BYTE *)"Project",(mit+0),0,0,0,0}
, {(men+7),65,0,60,80,(USHORT)MENUENABLED,(BYTE *)"Fract.2",(mit+44),0,0,0,0}
, {NULL,130,0,55,80,(USHORT)MENUENABLED,(BYTE *)"Screen",(mit+7),0,0,0,0}
};


