/* formulas.c */
#define Ult_Zmax lim1
#define Ult_dZmin lim4
#define Ult_Zremax lim2
#define Ult_Zimmax lim3
#include <aros/oldprograms.h>
#include <stdio.h>
#include <math.h>
#include <exec/types.h>
#include <intuition/intuition.h>
#include "defs.h"


extern float remin;
extern char reminbuf[];
extern float remax;
extern char remaxbuf[];
extern float immin;
extern char imminbuf[];
extern float immax;
extern char immaxbuf[];
extern float reconst;
extern char reconstbuf[];
extern float imconst;
extern char imconstbuf[];
extern float magre;
extern char magrebuf[];
extern float magim;
extern char magimbuf[];
extern float re0const;
extern char re0constbuf[];
extern float im0const;
extern char im0constbuf[];
extern float re1const;
extern char re1constbuf[];
extern float im1const;
extern char im1constbuf[];
extern float re2const;
extern char re2constbuf[];
extern float im2const;
extern char im2constbuf[];
extern float re3const;
extern char re3constbuf[];
extern float im3const;
extern char im3constbuf[];
extern float re4const;
extern char re4constbuf[];
extern float im4const;
extern char im4constbuf[];
extern float re5const;
extern char re5constbuf[];
extern float im5const;
extern char im5constbuf[];
extern float re6const;
extern char re6constbuf[];
extern float im6const;
extern char im6constbuf[];
extern float re7const;
extern char re7constbuf[];
extern float im7const;
extern char im7constbuf[];
extern float re8const;
extern char re8constbuf[];
extern float im8const;
extern char im8constbuf[];
extern float re9const;
extern char re9constbuf[];
extern float im9const;
extern char im9constbuf[];
extern float lim1;
extern char lim1buf[];
extern float lim2;
extern char lim2buf[];
extern float lim3;
extern char lim3buf[];
extern float lim4;
extern char lim4buf[];

extern float lambda;
extern char lambdabuf[];
extern float lamexp;
extern char lamexpbuf[];
extern float epsilon;
extern char epsilonbuf[];
extern short maxiter;
extern char maxiterbuf[];
extern float bioreconst;
extern char biorebuf[];
extern float bioimconst;
extern char bioimbuf[];
extern float heightconst;
extern char hcbuf[];
extern short radius;
extern char radiusbuf[];
extern short step;
extern char stepbuf[];
extern short rulelen;
extern char rulebuf[];

extern struct Gadget gad[];
extern struct Gadget gadb[];
extern struct Screen *scr,*openscreen();
extern struct Window *gwdw,*wdw,*openwindow();
extern struct Menu men[];
extern long titelflag;

extern struct IntuitionBase *IntuitionBase;
extern struct GfxBase *GfxBase;
extern struct RastPort *rport;
extern short tmpcol,mennum,itmnum,subnum;
extern short width;
extern short height;
extern short depth;

struct IntuiMessage *imsg;
struct ViewPort *WVP;
void ColorWindow(),scanmenu();
void PutBoolGadget(),PutGadget();
void prepult(),runfractal();
void toggletitle(),ShowTitle();

static short biomorph=BIO0;
static short newton=NR0;
extern short surface;
static short fractal=LCOSZ;
static short counter;
extern short xc,yc,xoffset,yoffset;
static float resave,imsave,restep,imstep;
static float re,re2,im,im2,tmp,lambdahalva;


double sqrt(),atan(),log(),atof(),exp(),sin(),cos(),fabs();

void getfractal(),getvalues();
void magnet0(),magnet1(),magnet2(),magnet3();
void mandelbrot(),julia(),ultimate(),lcosz(),esinz(),lexpz();
void bio0(),bio1(),bio2(),bio3(),bio4(),bio5();
void nr0(),nr1(),nr2(),nr3(),nr4(),nr5();
void runfractal(),frac1(),frac2(),frac3();
void (*fracalg)()=lcosz;


void Ult_A(),Ult_B(),Ult_C(),Ult_D(),Ult_E(),Ult_F(),Ult_G(),Ult_H();
void Ult_I(),Ult_J(),Ult_K(),Ult_L(),Ult_M(),Ult_N(),Ult_O(),Ult_P();
void Ult_Q(),Ult_R(),Ult_S(),Ult_T();
void Ult_a(),Ult_b(),Ult_c(),Ult_d(),Ult_e(),Ult_f(),Ult_g(),Ult_h();
void Ult_i(),Ult_j(),Ult_k(),Ult_l(),Ult_m(),Ult_n(),Ult_o(),Ult_p();
void Ult_q(),Ult_r(),Ult_s(),Ult_t();
void Ult_swplo(),Ult_swphi(),Ult_mmlo(),Ult_plo();
float Ult_Z0re,Ult_Z0im,Ult_Znre,Ult_Znim;
float Ult_re,Ult_im;
float Ult_tmpre[12],Ult_tmpim[12];
float Ult_cre[10],Ult_cim[10];
short Ult_cnt,Ult_brk;


void lcosz()
{
register float rereg,imreg,tmpreg,lambdahalvareg;

lambdahalvareg=lambdahalva;
imreg=imsave;
rereg=resave;
for(counter=0;counter<maxiter;counter++)
   {
    tmpreg=exp(-imreg);
    im2=imreg=tmpreg*sin(rereg);
    re2=rereg=tmpreg*cos(rereg);
    if((fabs(imreg) > 1e9)||(fabs(rereg) > 1e9))
      { counter=maxiter;  }
    else
       tmpreg=rereg*rereg+imreg*imreg;
    if( tmpreg==0 )tmpreg=0.0000000000001;
    rereg=rereg/tmpreg;
    imreg=-imreg/tmpreg;
    rereg=(rereg+re2)*lambdahalvareg;
    imreg=(imreg+im2)*lambdahalvareg;
    if( fabs(imreg) > 19 )
       return;
   };
}


void bio0()
{
register float rereg,imreg,re2reg,im2reg;
float tmp;
short coltmp;

imreg=imsave;
rereg=resave;
re2reg=rereg*rereg;
im2reg=imreg*imreg;
for(counter=0;counter<maxiter;counter++)
   {
    tmp  =rereg*(re2reg-3*im2reg)+bioreconst;
    imreg=imreg*(3*re2reg-im2reg)+bioimconst;
    rereg=tmp;
    re2reg=rereg*rereg;
    im2reg=imreg*imreg;
    if( (fabs(imreg) > 10) || (fabs(rereg) > 10) || (fabs(re2reg+im2reg) >100 ) )
       {
        coltmp=
           (fabs(rereg)<10)?2:0;

        if(fabs(imreg)<10)coltmp++;

        (coltmp!=0)? (counter+=coltmp):(counter=maxiter);

        return;
       };
   };
}

void bio1()
{
register float rereg,imreg,re2reg,im2reg;
float bel,arg;
short coltmp;

imreg=imsave;
rereg=resave;
re2reg=rereg*rereg;
im2reg=imreg*imreg;
for(counter=0;counter<maxiter;counter++)
   {
    bel=sqrt((double)(re2reg+im2reg));
    if(rereg>0)arg=atan((double)(imreg/rereg));
    else if(rereg<0)arg=-3.1415+atan((double)(imreg/rereg));
    else if(imreg>0)arg=1.5708;
    else arg=-1.5708;
    arg*=5;
    bel=bel*bel*bel*bel*bel;
    rereg=bel*cos((double)arg)+bioreconst;
    imreg=bel*sin((double)arg)+bioimconst;
    re2reg=rereg*rereg;
    im2reg=imreg*imreg;
    if( fabs(re2reg+im2reg) > 100 )
       {
        coltmp=
           (fabs(rereg)<10)?2:0;

        if(fabs(imreg)<10)coltmp++;

        (coltmp!=0)? (counter+=coltmp):(counter=maxiter);

        return;
       };
   };
}


void bio2()
{
register float rereg,imreg,re2reg,im2reg;
float bel,arg;
float lnbel,zzbel,zzarg,zzre,zzim;
float z5bel,z5arg,z5re,z5im;
short coltmp;

imreg=imsave;
rereg=resave;
re2reg=rereg*rereg;
im2reg=imreg*imreg;
for(counter=0;counter<maxiter;counter++)
   {
    bel=sqrt((double)(re2reg+im2reg));
    if(rereg>0)arg=atan((double)(imreg/rereg));
    else if((rereg<0)&&(imreg>0))arg= 3.1415+atan((double)(imreg/rereg));
    else if((rereg<0)&&(imreg<0))arg= -3.1415+atan((double)(imreg/rereg));
    else if(imreg>0)arg=1.5708;
    else arg=-1.5708;

    lnbel=log((double)bel);
    zzbel=exp((double)(rereg*lnbel-imreg*arg));
    zzarg=rereg*fabs((double)arg)+fabs((double)imreg)*lnbel;
    zzre=zzbel*cos((double)zzarg);
    zzim=zzbel*sin((double)zzarg);

    z5bel=bel*bel*bel*bel*bel;
    z5arg=5*arg;
    z5re=z5bel*cos((double)z5arg);
    z5im=z5bel*sin((double)z5arg);

    rereg=zzre+z5re+bioreconst;
    imreg=zzim+z5im+bioimconst;

    re2reg=rereg*rereg;
    im2reg=imreg*imreg;
    if( fabs(re2reg+im2reg) > 100 )
       {
        coltmp=
           (fabs(rereg)<10)?2:0;

        if(fabs(imreg)<10)coltmp++;

        (coltmp!=0)? (counter+=coltmp):(counter=maxiter);

        return;
       };
   };
}

void bio3()
{
register float rereg,imreg,re2reg,im2reg;
float bel,arg;
float lnbel,zzbel,zzarg,zzre,zzim;
float z6bel,z6arg,z6re,z6im;
short coltmp;

imreg=imsave;
rereg=resave;
re2reg=rereg*rereg;
im2reg=imreg*imreg;
for(counter=0;counter<maxiter;counter++)
   {
    bel=sqrt((double)(re2reg+im2reg));
    if(rereg>0)arg=atan((double)(imreg/rereg));
    else if((rereg<0)&&(imreg>0))arg= 3.1415+atan((double)(imreg/rereg));
    else if((rereg<0)&&(imreg<0))arg= -3.1415+atan((double)(imreg/rereg));
    else if(imreg>0)arg=1.5708;
    else arg=-1.5708;

    lnbel=log((double)bel);
    zzbel=exp((double)(rereg*lnbel-imreg*arg));
    zzarg=rereg*fabs((double)arg)+fabs((double)imreg)*lnbel;
    zzre=zzbel*cos((double)zzarg);
    zzim=zzbel*sin((double)zzarg);

    z6bel=bel*bel*bel*bel*bel*bel;
    z6arg=6*arg;
    z6re=z6bel*cos((double)z6arg);
    z6im=z6bel*sin((double)z6arg);

    rereg=zzre+z6re+bioreconst;
    imreg=zzim+z6im+bioimconst;

    re2reg=rereg*rereg;
    im2reg=imreg*imreg;
    if( fabs(re2reg+im2reg) > 100 )
       {
        coltmp=
           (fabs(rereg)<10)?2:0;

        if(fabs(imreg)<10)coltmp++;

        (coltmp!=0)? (counter+=coltmp):(counter=maxiter);

        return;
       };
   };
}


void bio4()
{
register float sinre,sinim,sinre2,sinim2;

float sintmp,rereg,re2reg,imreg,im2reg;
short coltmp;

imreg=imsave;
rereg=resave;
re2reg=rereg*rereg;
im2reg=imreg*imreg;
for(counter=0;counter<maxiter;counter++)
   {
    sintmp=exp(-imreg);
    sinim2=sinim=sintmp*sin(rereg);
    sinre2=sinre=sintmp*cos(rereg);
    sintmp=sinre*sinre+sinim*sinim;
    if( sintmp==0 )sintmp=0.0000000000001;
    sinre=sinre/sintmp;
    sinim=-sinim/sintmp;
    sintmp=(sinim-sinim2)/2;
    sinim=-(sinre-sinre2)/2;
    sinre=sintmp;
    sintmp=sinre-sinim;
    sinim=sinim+sinre;
    sinre=sintmp;

    sintmp=sinre+re2reg-im2reg+bioreconst;
    imreg =sinim+2*rereg*imreg+bioimconst;
    rereg=sintmp;

    re2reg=rereg*rereg;
    im2reg=imreg*imreg;
    if( fabs(re2reg+im2reg) > 100 )
       {
        coltmp=
           (fabs(rereg)<10)?2:0;

        if(fabs(imreg)<10)coltmp++;

        (coltmp!=0)? (counter+=coltmp):(counter=maxiter);

        return;
       };
   };
}

void bio5()
{
register float sinre,sinim,sinre2,sinim2;
float expre,expim;
float sintmp,rereg,re2reg,imreg,im2reg;
short coltmp;

imreg=imsave;
rereg=resave;
re2reg=rereg*rereg;
im2reg=imreg*imreg;
for(counter=0;counter<maxiter;counter++)
   {
    sintmp=exp(-imreg);
    sinim2=sinim=sintmp*sin(rereg);
    sinre2=sinre=sintmp*cos(rereg);
    sintmp=sinre*sinre+sinim*sinim;
    if( sintmp==0 )sintmp=0.0000000000001;
    sinre=sinre/sintmp;
    sinim=-sinim/sintmp;
    sintmp=(sinim-sinim2)/2;
    sinim=-(sinre-sinre2)/2;
    sinre=sintmp;
    sintmp=sinre-sinim;
    sinim=sinim+sinre;
    sinre=sintmp;

    sintmp=exp(rereg);
    expre=sintmp*cos(imreg);
    expim=sintmp*sin(imreg);

    sintmp=sinre+expre+bioreconst;
    imreg =sinim+expim+bioimconst;
    rereg=sintmp;

    re2reg=rereg*rereg;
    im2reg=imreg*imreg;
    if( fabs(re2reg+im2reg) > 100 )
       {
        coltmp=
           (fabs(rereg)<10)?2:0;

        if(fabs(imreg)<10)coltmp++;

        (coltmp!=0)? (counter+=coltmp):(counter=maxiter);

        return;
       };
   };
}


void lexpz()
{
register float rereg,imreg,tmpreg;

imreg=imsave;
rereg=resave;
for(counter=0;counter<maxiter;counter++)
   {
    tmpreg=exp(rereg)*lamexp;
    rereg=tmpreg*cos(imreg);
    imreg=tmpreg*sin(imreg);
    if( fabs(rereg) > 30 )
       return;
   };
}

void esinz()
{
register float rereg,imreg,tmpreg,lambdahalvareg;

imreg=imsave;
rereg=resave;
for(counter=0;counter<maxiter;counter++)
   {
    tmpreg=exp(-imreg);
    im2=imreg=tmpreg*sin(rereg);
    re2=rereg=tmpreg*cos(rereg);
    if((fabs(imreg) > 1e9)||(fabs(rereg) > 1e9))
      { counter=maxiter;  }
    else
       tmpreg=rereg*rereg+imreg*imreg;
    if( tmpreg==0 )tmpreg=0.0000000000001;
    rereg=rereg/tmpreg;
    imreg=-imreg/tmpreg;
    tmpreg=(imreg-im2)/2;
    imreg=-(rereg-re2)/2;
    rereg=tmpreg;
    tmpreg=rereg-imreg*epsilon;
    imreg=imreg+rereg*epsilon;
    rereg=tmpreg;
    if( fabs(imreg) > 19 )
       return;
   };
}

void mandelbrot()
{
register float rereg,imreg,tmpreg,absreg;
float tmp;

rereg=imreg=absreg=0;
for(counter=0;counter<maxiter;counter++)
   {
   tmp=imreg*imreg;
   absreg=rereg*rereg;
   tmpreg=absreg-tmp+resave;
   imreg=2*rereg*imreg+imsave;
   rereg=tmpreg;
   absreg+=tmp;
   if( fabs(absreg) > 4 )
       return;
   };
}

void julia()
{
register float rereg,imreg,tmpreg,absreg;
float tmp;

rereg=resave;
imreg=imsave;
for(counter=0;counter<maxiter;counter++)
   {
   tmp=imreg*imreg;
   absreg=rereg*rereg;
   tmpreg=absreg-tmp+reconst;
   imreg=2*rereg*imreg+imconst;
   rereg=tmpreg;
   absreg+=tmp;
   if( fabs(absreg) > 4 )
       return;
   };

}


void nr0()
{
register float rereg,imreg,re2,im2;
float re,im,reim,ret,imt,ren,imn,tmp;

imreg=imsave;
rereg=resave;
re2=rereg*rereg;
im2=imreg*imreg;
reim=3*rereg*imreg;
for(counter=0;counter<maxiter;counter++)
   {
    re=re2*rereg-reim*imreg-1;
    im=rereg*reim-im2*imreg;
    ren=3*(re2-im2);
    imn=2*reim;
    ret=re*ren+im*imn;
    imt=im*ren-re*imn;
    tmp=ren*ren+imn*imn;if(tmp==0)tmp=0.00000001;
    ret/=tmp;
    imt/=tmp;
    rereg-=ret;
    imreg-=imt;
    re2=rereg*rereg;
    im2=imreg*imreg;
    reim=3*rereg*imreg;
    if(  ((fabs(rereg-1)<0.000001)  &&(fabs(imreg)<0.000001))
      || ((fabs(rereg+0.5)<0.000001)&&(fabs(imreg-0.866025)<0.000001))
      || ((fabs(rereg+0.5)<0.000001)&&(fabs(imreg+0.866025)<0.000001))  )
         return;
   };
}


void nr1()
{
register float rereg,imreg,z3re,z3im;
float z4re,z4im;
float tmp;

imreg=imsave;
rereg=resave;
for(counter=0;counter<maxiter;counter++)
   {
    z3re=rereg*rereg-imreg*imreg;
    z3im=2*rereg*imreg;
    z4re=3*(z3re*z3re-z3im*z3im)+1;
    z4im=6*z3re*z3im;
    tmp=z3re*rereg-z3im*imreg;
    z3im=4*(z3re*imreg+z3im*rereg);
    z3re=4*tmp;

    rereg=z3re*z3re+z3im*z3im;if(rereg==0)rereg=0.0000001;
    imreg=(z4im*z3re-z4re*z3im)/rereg;
    rereg=(z4re*z3re+z4im*z3im)/rereg;

    if(fabs((double)rereg)<0.00001)
    if((fabs((double)imreg-1)<0.00001)||(fabs((double)imreg+1)<0.00001))
       return;
    if(fabs((double)imreg)<0.00001)
    if((fabs((double)rereg-1)<0.00001)||(fabs((double)rereg+1)<0.00001))
       return;
   };
}


void nr2()
{
register float rereg,imreg,z4re,z4im;
float z5re,z5im;
float tmp;

imreg=imsave;
rereg=resave;
for(counter=0;counter<maxiter;counter++)
   {
    z4re=rereg*rereg-imreg*imreg;
    z4im=2*rereg*imreg;
    tmp=z4re*z4re-z4im*z4im;
    z4im=2*z4re*z4im;
    z4re=tmp;
    z5re=4*(z4re*rereg-z4im*imreg)+1;
    z5im=4*(z4re*imreg+z4im*rereg);

    z4re=5*z4re;
    z4im=5*z4im;

    tmp=z4re*z4re+z4im*z4im;if(tmp==0)tmp=0.00000001;
    rereg=(z5re*z4re+z5im*z4im)/tmp;
    imreg=(z5im*z4re-z5re*z4im)/tmp;

    if( (fabs((double)rereg-1)<0.00001)
      &&(fabs((double)imreg)<0.00001) )
         return;
    if( (fabs((double)rereg-0.3090170)<0.00001)
      &&(fabs(fabs((double)imreg)-0.9510566)<0.00001))
         return;
    if( (fabs((double)rereg+0.8090170)<0.00001)
      &&(fabs(fabs((double)imreg)-0.5877853)<0.00001))
         return;
   };
}

void nr3()
{
register float rereg,imreg,z4re,z4im;
float z2re,z2im,z5re,z5im;
float tmp;

imreg=imsave;
rereg=resave;
for(counter=0;counter<maxiter;counter++)
   {
    z2re=z4re=rereg*rereg-imreg*imreg;
    z2im=z4im=2*rereg*imreg;
    tmp=z4re*z4re-z4im*z4im;
    z4im=2*z4re*z4im;
    z4re=tmp;
    z5re=4*(z4re*rereg-z4im*imreg)+2*(z2re*rereg-z2im*imreg)+1;
    z5im=4*(z4re*imreg+z4im*rereg)+2*(z2re*imreg+z2im*rereg);

    z4re=5*z4re+3*z2re;
    z4im=5*z4im+3*z2im;

    tmp=z4re*z4re+z4im*z4im;if(tmp==0)tmp=0.00000001;
    rereg=(z5re*z4re+z5im*z4im)/tmp;
    imreg=(z5im*z4re-z5re*z4im)/tmp;

    if( (fabs((double)rereg-0.83761977)<0.00001)
      &&(fabs((double)imreg)<0.00001))
         return;
    if( (fabs((double)rereg-0.21785321)<0.00001)
      &&(fabs(fabs((double)imreg)-1.1669512)<0.00001))
         return;
    if( (fabs((double)rereg+0.6366631)<0.00001)
      &&(fabs(fabs((double)imreg)-0.6647016)<0.00001))
         return;
   };
}


void nr4()
{
register float rereg,imreg,re2,im2;
float re,im,reim,ret,imt,ren,imn,tmp;
short coltmp;

imreg=imsave;
rereg=resave;
re2=rereg*rereg;
im2=imreg*imreg;
reim=3*rereg*imreg;
for(counter=0;counter<maxiter;counter++)
   {
    re=re2*rereg-reim*imreg-1;
    im=rereg*reim-im2*imreg;
    ren=3*(re2-im2);
    imn=2*reim;
    ret=re*ren+im*imn;
    imt=im*ren-re*imn;
    tmp=ren*ren+imn*imn;if(tmp==0)tmp=0.00000001;
    ret/=tmp;
    imt/=tmp;
    rereg-=ret;
    imreg-=imt;
    re2=rereg*rereg;
    im2=imreg*imreg;
    reim=3*rereg*imreg;
    if(  ((fabs(rereg-1)<0.000001)  &&(fabs(imreg)<0.000001))
      || ((fabs(rereg+0.5)<0.000001)&&(fabs(imreg-0.866025)<0.000001))
      || ((fabs(rereg+0.5)<0.000001)&&(fabs(imreg+0.866025)<0.000001))  )
         return;
   };
}


void nr5()
{
register float rereg,imreg,re2,im2;
float re,im,reim,ret,imt,ren,imn,tmp;
short coltmp;

imreg=imsave;
rereg=resave;
re2=rereg*rereg;
im2=imreg*imreg;
reim=3*rereg*imreg;
for(counter=0;counter<maxiter;counter++)
   {
    re=re2*rereg-reim*imreg-1;
    im=rereg*reim-im2*imreg;
    ren=3*(re2-im2);
    imn=2*reim;
    ret=re*ren+im*imn;
    imt=im*ren-re*imn;
    tmp=ren*ren+imn*imn;if(tmp==0)tmp=0.00000001;
    ret/=tmp;
    imt/=tmp;
    rereg-=ret;
    imreg-=imt;
    re2=rereg*rereg;
    im2=imreg*imreg;
    reim=3*rereg*imreg;
    if(  ((fabs(rereg-1)<0.000001)  &&(fabs(imreg)<0.000001))
      || ((fabs(rereg+0.5)<0.000001)&&(fabs(imreg-0.866025)<0.000001))
      || ((fabs(rereg+0.5)<0.000001)&&(fabs(imreg+0.866025)<0.000001))  )
         return;
   };
}


void magnet0()
{
register float rereg,imreg,tmpreg,absreg;
float tmp,ret,imt,ren,imn;

rereg=resave;
imreg=imsave;
for(counter=0;counter<maxiter;counter++)
   {
    ret=rereg*rereg-imreg*imreg+magre-1;
    imt=2*rereg*imreg+magim;
    ren=2*rereg+magre-2;
    imn=2*imreg+magim;
    tmp=ren*ren+imn*imn;
    if(fabs(tmp)<0.00001)tmp=0.00001;
    rereg=(ret*ren+imt*imn)/tmp;
    imreg=(ren*imt-ret*imn)/tmp;

    tmp=rereg*rereg-imreg*imreg;
    imreg=2*rereg*imreg;
    rereg=tmp;

    absreg=rereg*rereg+imreg*imreg;

   if( fabs(absreg) > 10000 )
     { counter&=15;return; };
   if( (fabs(rereg-1)+fabs(imreg))<0.01 )
     { counter&=15;counter|=16;return; };
   };

}


void magnet1()
{
register float rereg,imreg,tmpreg,absreg;
float tmp,ret,imt,ren,imn;

rereg=0;
imreg=0;
for(counter=0;counter<maxiter;counter++)
   {
    ret=rereg*rereg-imreg*imreg+resave-1;
    imt=2*rereg*imreg+imsave;
    ren=2*rereg+resave-2;
    imn=2*imreg+imsave;
    tmp=ren*ren+imn*imn;
    if(fabs(tmp)<0.00001)tmp=0.00001;
    rereg=(ret*ren+imt*imn)/tmp;
    imreg=(ren*imt-ret*imn)/tmp;

    tmp=rereg*rereg-imreg*imreg;
    imreg=2*rereg*imreg;
    rereg=tmp;

    absreg=rereg*rereg+imreg*imreg;

   if( fabs(absreg) > 10000 )
     { counter&=15;return; };
   if( (fabs(rereg-1)+fabs(imreg))<0.01 )
     { counter&=15;counter|=16;return; };
   };

}


void magnet2()
{
register float rereg,imreg,tmpreg,absreg;
float tmp,ret,imt,ren,imn,magr,magi,magr2,magi2;

magr=(magre-1)*(magre-2)-magim*magim;
magi=magim*(2*magre-3);
magr2=magre*magre-magim*magim-3*magre+3;
magi2=2*magre*magim-3*magim;
rereg=resave;
imreg=imsave;
for(counter=0;counter<maxiter;counter++)
   {
    ret=rereg*rereg-imreg*imreg+3*(magre-1);
    imt=2*rereg*imreg+3*magim;
    tmp=ret*rereg-imt*imreg+magr;
    imt=ret*imreg+imt*rereg+magi;
    ret=tmp;
    ren=3*(rereg*rereg-imreg*imreg)+3*((magre-2)*rereg-magim*imreg)+magr2;
    imn=6*rereg*imreg+3*(magim*rereg+(magre-2)*imreg)+magi2;
    tmp=ren*ren+imn*imn;
    if(fabs(tmp)<0.00001)tmp=0.00001;
    rereg=(ret*ren+imt*imn)/tmp;
    imreg=(ren*imt-ret*imn)/tmp;

    tmp=rereg*rereg-imreg*imreg;
    imreg=2*rereg*imreg;
    rereg=tmp;

    absreg=rereg*rereg+imreg*imreg;

   if( fabs(absreg) > 10000 )
     { counter&=15;return; };
   if( (fabs(rereg-1)+fabs(imreg))<0.01 )
     { counter&=15;counter|=16;return; };
   };

}



void magnet3()
{
register float rereg,imreg,tmpreg,absreg;
float tmp,ret,imt,ren,imn,magr,magi,magr2,magi2;

magr=(resave-1)*(resave-2)-imsave*imsave;
magi=imsave*(2*resave-3);
magr2=resave*resave-imsave*imsave-3*resave+3;
magi2=2*resave*imsave-3*imsave;
rereg=0;
imreg=0;
for(counter=0;counter<maxiter;counter++)
   {
    ret=rereg*rereg-imreg*imreg+3*(resave-1);
    imt=2*rereg*imreg+3*imsave;
    tmp=ret*rereg-imt*imreg+magr;
    imt=ret*imreg+imt*rereg+magi;
    ret=tmp;
    ren=3*(rereg*rereg-imreg*imreg)+3*((resave-2)*rereg-imsave*imreg)+magr2;
    imn=6*rereg*imreg+3*(imsave*rereg+(resave-2)*imreg)+magi2;
    tmp=ren*ren+imn*imn;
    if(fabs(tmp)<0.00001)tmp=0.00001;
    rereg=(ret*ren+imt*imn)/tmp;
    imreg=(ren*imt-ret*imn)/tmp;

    tmp=rereg*rereg-imreg*imreg;
    imreg=2*rereg*imreg;
    rereg=tmp;

    absreg=rereg*rereg+imreg*imreg;

   if( fabs(absreg) > 10000 )
     { counter&=15;return; };
   if( (fabs(rereg-1)+fabs(imreg))<0.01 )
     { counter&=15;counter|=16;return; };
   };

}


void runfractal()
{
rport=wdw->RPort;
SetAPen(rport,0L);
RectFill(rport,0L,0L,(long)(width-1),(long)(height-1));
getvalues();
SetMenuStrip(wdw,(men+3));
switch(surface)
   {
    case D2RECT:
       {
        frac1(fracalg);
        break;
       };
    case D3RECT:
       {
        frac2(fracalg);
        break;
       };
    case SPHERE:
       {
        frac3(fracalg);
        break;
       };
   };
ClearMenuStrip(wdw);
}

void frac1(frac)
void (*frac)();
{
float restep2,imstep2;
short colors;

colors=(1<<depth)-2;
restep=(remax-remin)/width;
imstep=(immax-immin)/height;
restep2=restep*4;
imstep2=imstep*4;
lambdahalva=lambda/2;
rport=wdw->RPort;

for(xoffset=0;xoffset<4;xoffset++)
 {
  for(yoffset=0;yoffset<4;yoffset++)
   {
    resave=remin+restep*xoffset;
    for(xc=xoffset;xc<width;xc+=4)
     {
      resave += restep2;
      imsave=immax-imstep*yoffset;
      for(yc=yoffset;yc<height;yc+=4)
         {
          imsave -= imstep2;
          (*frac)();
          (counter==maxiter)?
              SetAPen(rport,0L):SetAPen(rport,(counter % colors)+2L);
          WritePixel(rport,(long)xc,(long)yc);
          scanmenu();
         };
     };
   };
 };
}

void frac2(frac)
void (*frac)();
{
short heighthalf,ycplusheight,xcpluswidth,widthadjuster;
short width2,width3,colors;

colors=(1<<depth)-2;
width2=(short)(width*0.875);
width3=width-width2;
heighthalf=height/2;
restep=(remax-remin)/width2*step;
imstep=(immax-immin)/height*step;
lambdahalva=lambda/2;
rport=wdw->RPort;

imsave=immax;
for(yc=0;yc<height;yc+=step)
   {
    ycplusheight=yc/2+heighthalf;
    widthadjuster=width3*yc/height;
    imsave -= imstep;
    resave=remin;
    xcpluswidth=widthadjuster;
    for(xc=0;xc<width2;xc+=step)
       {
        resave += restep;
        (*frac)();
        if(counter!=maxiter)
           {
            SetAPen(rport,(long)(counter % colors +2 ));
            counter=(short)(log((double)counter)*heightconst);

            if(fractal!=BIOMORPH)
               {
                SetAPen(rport,(long)(counter % colors +2 ));
               };

            Move(rport,(long)xcpluswidth,(long)(ycplusheight));
            Draw(rport,(long)xcpluswidth,(long)(ycplusheight-counter));
           };
        xcpluswidth+=step;
        scanmenu();
      };
   };
}

void frac3(frac)
void (*frac)();
{
short heighthalf,widthhalf;
short xrad,yrad,colors;
float xang,yang,angxstep,angystep;
float xdir,ydir;
float re1,re2,im1,im2;

colors=(1<<depth)-2;
xrad=(width>400)?radius*2:radius;
yrad=(height>300)?radius*2:radius;
heighthalf=height/2;
widthhalf=width/2;
angxstep=3.1415/widthhalf/2*step;
angystep=3.1415/heighthalf/2*step;

restep=(remax-remin)/width*step;
imstep=(immax-immin)/height*step;
lambdahalva=lambda/2;
rport=wdw->RPort;

yang=3.1415;
im1=immax;
im2=immin;
for(yc=0;yc<heighthalf;yc+=step)
   {
    im1 -= imstep;
    im2 += imstep;
    yang-=angystep;
    xang=-3.1415;
    re1=remin;
    re2=remax;
    for(xc=0;xc<widthhalf;xc+=step)
       {
        re1 += restep;
        re2 -= restep;
        xang+=angxstep;
        xdir=sin((double)xang)*cos((double)yang);
        ydir=cos((double)xang);
        resave=re1;
        imsave=im1;
        (*frac)();
        if(counter!=maxiter)
           {
            SetAPen(rport,(long)(counter % colors +2));
            counter=(short)(log((double)counter)*heightconst);
            if(fractal!=BIOMORPH)
               {
                SetAPen(rport,(long)(counter % colors +2 ));
               };
            Move(rport,(long)(widthhalf +xrad*ydir),
                       (long)(heighthalf-yrad*xdir));
            Draw(rport,(long)(widthhalf +ydir*(xrad+counter)),
                       (long)(heighthalf-xdir*(yrad+counter)));
           };
        resave=re2;
        imsave=im1;
        (*frac)();
        if(counter!=maxiter)
           {
            SetAPen(rport,(long)(counter % colors +2));
            counter=(short)(log((double)counter)*heightconst);
            if(fractal!=BIOMORPH)
               {
                SetAPen(rport,(long)(counter % colors +2 ));
               };
            Move(rport,(long)(widthhalf -xrad*ydir),
                       (long)(heighthalf-yrad*xdir));
            Draw(rport,(long)(widthhalf -ydir*(xrad+counter)),
                       (long)(heighthalf-xdir*(yrad+counter)));
           };
        resave=re1;
        imsave=im2;
        (*frac)();
        if(counter!=maxiter)
           {
            SetAPen(rport,(long)(counter % colors+2));
            counter=(short)(log((double)counter)*heightconst);
            if(fractal!=BIOMORPH)
               {
                SetAPen(rport,(long)(counter % colors +2 ));
               };
            Move(rport,(long)(widthhalf +xrad*ydir),
                       (long)(heighthalf+yrad*xdir));
            Draw(rport,(long)(widthhalf +ydir*(xrad+counter)),
                       (long)(heighthalf+xdir*(yrad+counter)));
           };
        resave=re2;
        imsave=im2;
        (*frac)();
        if(counter!=maxiter)
           {
            SetAPen(rport,(long)(counter % colors+2));
            counter=(short)(log((double)counter)*heightconst);
            if(fractal!=BIOMORPH)
               {
                SetAPen(rport,(long)(counter % colors +2 ));
               };
            Move(rport,(long)(widthhalf -xrad*ydir),
                       (long)(heighthalf+yrad*xdir));
            Draw(rport,(long)(widthhalf -ydir*(xrad+counter)),
                       (long)(heighthalf+xdir*(yrad+counter)));
           };
        scanmenu();
      };
   };
}



void getvalues()
{

if(fractal!=ULTIMATE)
 {
   gwdw=openwindow(scr,50,10,200,180,(ULONG)ACTIVATE,
                   (USHORT)(GADGETUP),NULL);
   rport=gwdw->RPort;
   SetAPen(rport,1L);

   PutGadget("ReMin",5,10,20,(gad+0));
   PutGadget("ReMax",5,110,20,(gad+1));
   PutGadget("ImMin",5,10,42,(gad+2));
   PutGadget("ImMax",5,110,42,(gad+3));

   Move(rport,10L,10L);
   switch(fractal)
      {
       case NEWTON: { Text(rport,"Newton-R",8L);break; };
       case MANDELBROT: { Text(rport,"Mandelbrot",10L);break; };
       case BIOMORPH: { Text(rport,"Biomorph",8L);
                        PutGadget("ReConst",7,10,64,(gad+10));
                        PutGadget("ImConst",7,110,64,(gad+11));
                        break; };
       case JULIA: { Text(rport,"Julia",5L);
                     PutGadget("ReConst",7,10,64,(gad+7));
                     PutGadget("ImConst",7,110,64,(gad+8));
                     break; };
       case MAGNET2:
       case MAGNET0:{ Text(rport,"Magnet x",8L);
                      PutGadget("ReConst",7,10,64,(gad+19));
                      PutGadget("ImConst",7,110,64,(gad+20));
                      break; };
       case MAGNET3:
       case MAGNET1:{ Text(rport,"Magnet q",8L);break; };
        case LEXPZ: { Text(rport,"lEXPz",5L);
                      PutGadget("Lambda",6,10,64,(gad+9));
                      break; };
        case LCOSZ: { Text(rport,"lCOSz",5L);
                      PutGadget("Lambda",6,10,64,(gad+4));
                      break; };
        case ESINZ: { Text(rport,"(1-ei)SINz",10L);
                      PutGadget("Epsilon",7,10,64,(gad+6));
                      break; };
       };
   Move(rport,110L,10L);
   switch(surface)
    {
     case D2RECT: { Text(rport,"2D-Rect",7L);break; };
     case D3RECT: { Text(rport,"3D-Rect",7L);
                    PutGadget("HeightConst",11,10,113,(gad+12));
                    break; };
     case SPHERE: { Text(rport,"Sphere",6L);
                    PutGadget("HeightConst",11,10,113,(gad+12));
                    PutGadget("Radius",6,110,113,(gad+13));
                    break; };
    };

  PutGadget("MaxIter",7,10,135,(gad+5));
  if(surface!=D2RECT)
    PutGadget("Step",4,110,135,(gad+14));

  PutBoolGadget(110,160,(gadb+0));

Wait(1L << gwdw->UserPort->mp_SigBit);
while((imsg=(struct IntuiMessage *)GetMsg(gwdw->UserPort)))
      ReplyMsg((struct Message *)imsg);
CloseWindow(gwdw);
};


if(fractal==ULTIMATE)
{
  prepult();
};


remin=atof(reminbuf);
remax=atof(remaxbuf);
immin=atof(imminbuf);
immax=atof(immaxbuf);
reconst=atof(reconstbuf);
imconst=atof(imconstbuf);
magre=atof(magrebuf);
magim=atof(magimbuf);
lambda=atof(lambdabuf);
lamexp=atof(lamexpbuf);
epsilon=atof(epsilonbuf);
bioreconst=atof(biorebuf);
bioimconst=atof(bioimbuf);
maxiter=(short)fabs(atof(maxiterbuf));if(maxiter<10)maxiter=10;
step=(short)fabs(atof(stepbuf));if(step==0)step=1;
heightconst=atof(hcbuf);
radius=(short)atof(radiusbuf);
rulelen=strlen(rulebuf);
re0const=atof(re0constbuf);
im0const=atof(im0constbuf);
re1const=atof(re1constbuf);
im1const=atof(im1constbuf);
re2const=atof(re2constbuf);
im2const=atof(im2constbuf);
re3const=atof(re3constbuf);
im3const=atof(im3constbuf);
re4const=atof(re4constbuf);
im4const=atof(im4constbuf);
re5const=atof(re5constbuf);
im5const=atof(im5constbuf);
re6const=atof(re6constbuf);
im6const=atof(im6constbuf);
re7const=atof(re7constbuf);
im7const=atof(im7constbuf);
re8const=atof(re8constbuf);
im8const=atof(im8constbuf);
re9const=atof(re9constbuf);
im9const=atof(im9constbuf);
Ult_cre[0]=re0const;Ult_cim[0]=im0const;
Ult_cre[1]=re1const;Ult_cim[1]=im1const;
Ult_cre[2]=re2const;Ult_cim[2]=im2const;
Ult_cre[3]=re3const;Ult_cim[3]=im3const;
Ult_cre[4]=re4const;Ult_cim[4]=im4const;
Ult_cre[5]=re5const;Ult_cim[5]=im5const;
Ult_cre[6]=re6const;Ult_cim[6]=im6const;
Ult_cre[7]=re7const;Ult_cim[7]=im7const;
Ult_cre[8]=re8const;Ult_cim[8]=im8const;
Ult_cre[9]=re9const;Ult_cim[9]=im9const;
lim1=atof(lim1buf);
lim2=atof(lim2buf);
lim3=atof(lim3buf);
lim4=atof(lim4buf);
}


void getfractal()
{
  { if((itmnum!=BIOTYPE)&&(itmnum!=NEWTONTYPE))fractal=itmnum;
    switch(itmnum)
       {
        case MANDELBROT: { fracalg=mandelbrot;break; };
        case JULIA: { fracalg=julia;break; };
        case ULTIMATE:{ fracalg=ultimate;break; };
        case MAGNET0:{ fracalg=magnet0;break; };
        case MAGNET1:{ fracalg=magnet1;break; };
        case MAGNET2:{ fracalg=magnet2;break; };
        case MAGNET3:{ fracalg=magnet3;break; };
        case LCOSZ: { fracalg=lcosz;break; };
        case ESINZ: { fracalg=esinz;break; };
        case LEXPZ: { fracalg=lexpz;break; };
        case BIOTYPE: { biomorph=subnum;if(fractal!=BIOMORPH)break; };
        case BIOMORPH:
          {
           switch(biomorph)
             {
              case BIO0: { fracalg=bio0;break; };
              case BIO1: { fracalg=bio1;break; };
              case BIO2: { fracalg=bio2;break; };
              case BIO3: { fracalg=bio3;break; };
              case BIO4: { fracalg=bio4;break; };
              case BIO5: { fracalg=bio5;break; };
             };
           break;
          };
        case NEWTONTYPE: { newton=subnum;if(fractal!=NEWTON)break; };
        case NEWTON:
          {
           switch(newton)
             {
              case NR0: { fracalg=nr0;break; };
              case NR1: { fracalg=nr1;break; };
              case NR2: { fracalg=nr2;break; };
              case NR3: { fracalg=nr3;break; };
              case NR4: { fracalg=nr4;break; };
              case NR5: { fracalg=nr5;break; };
             };
           break;
          };
       };
  };
}


void prepult()
{
   ShowTitle(scr,titelflag=0L);
   gwdw=openwindow(scr,1,1,318,195,(ULONG)ACTIVATE,
                   (USHORT)(GADGETUP),NULL);
   rport=gwdw->RPort;
   SetAPen(rport,1L);
   Move(rport,10L,10L);
   Text(rport,"Ultimate Fractal Generator.",27L);

   PutGadget("Iteration Rule List",19,10,20,(gad+18));
   Move(rport,10L,42L);
   Text(rport,"A  +Z0    K  +Zn    a  +Tx    k  +Cx",36L);
   Move(rport,10L,51L);
   Text(rport,"B  -Z0    L  -Zn    b  -Tx    l  -Cx",36L);
   Move(rport,10L,60L);
   Text(rport,"C  *Z0    M  *Zn    c  *Tx    m  *Cx",36L);
   Move(rport,10L,69L);
   Text(rport,"D  /Z0    N  /Zn    d  /Tx    n  /Cx",36L);
   Move(rport,10L,78L);
   Text(rport,"E  ^Z0    O  ^Zn    e  ^Tx    o  ^Cx",36L);
   Move(rport,10L,87L);
   Text(rport,"F  ^2     P  ln     f ->Tx          ",36L);
   Move(rport,10L,96L);
   Text(rport,"G  ^3     Q  exp    g <-Tx    q  Z >",36L);
   Move(rport,10L,105L);
   Text(rport,"H  ^4     R  sin    h <>Tx    r  Re>",36L);
   Move(rport,10L,114L);
   Text(rport,"I  ^5     S  cos    i  = 0    s  Im>",36L);
   Move(rport,10L,123L);
   Text(rport,"J  ^6     T  tan    j  INV    t  dZ<",36L);

   PutGadget("ReMin",5,10,143,(gad+0));
   PutGadget("ReMax",5,110,143,(gad+1));
   PutGadget("ImMin",5,10,165,(gad+2));
   PutGadget("ImMax",5,110,165,(gad+3));

   PutBoolGadget(210,180,(gadb+0));

Wait(1L << gwdw->UserPort->mp_SigBit);
while((imsg=(struct IntuiMessage *)GetMsg(gwdw->UserPort)))
      ReplyMsg((struct Message *)imsg);
CloseWindow(gwdw);


gwdw=openwindow(scr,1,10,318,180,(ULONG)ACTIVATE,
                (USHORT)(GADGETUP),NULL);
rport=gwdw->RPort;
SetAPen(rport,1L);

   PutGadget("ReConst0",8,10,10,(gad+16));
   PutGadget("ImConst0",8,110,10,(gad+17));
   PutGadget("ReConst1",8,10,32,(gad+21));
   PutGadget("ImConst1",8,110,32,(gad+22));
   PutGadget("ReConst2",8,10,54,(gad+23));
   PutGadget("ImConst2",8,110,54,(gad+24));
   PutGadget("ReConst3",8,10,76,(gad+25));
   PutGadget("ImConst3",8,110,76,(gad+26));
   PutGadget("ReConst4",8,10,98,(gad+27));
   PutGadget("ImConst4",8,110,98,(gad+28));
   PutGadget("ReConst5",8,10,120,(gad+29));
   PutGadget("ImConst5",8,110,120,(gad+30));

PutBoolGadget(210,160,(gadb+0));

Wait(1L << gwdw->UserPort->mp_SigBit);
while((imsg=(struct IntuiMessage *)GetMsg(gwdw->UserPort)))
      ReplyMsg((struct Message *)imsg);
CloseWindow(gwdw);

gwdw=openwindow(scr,1,10,318,180,(ULONG)ACTIVATE,
                (USHORT)(GADGETUP),NULL);
rport=gwdw->RPort;
SetAPen(rport,1L);

   PutGadget("ReConst6",8,10,10,(gad+31));
   PutGadget("ImConst6",8,110,10,(gad+32));
   PutGadget("ReConst7",8,10,32,(gad+33));
   PutGadget("ImConst7",8,110,32,(gad+34));
   PutGadget("ReConst8",8,10,54,(gad+35));
   PutGadget("ImConst8",8,110,54,(gad+36));
   PutGadget("ReConst9",8,10,76,(gad+37));
   PutGadget("ImConst9",8,110,76,(gad+38));

PutBoolGadget(210,160,(gadb+0));

Wait(1L << gwdw->UserPort->mp_SigBit);
while((imsg=(struct IntuiMessage *)GetMsg(gwdw->UserPort)))
      ReplyMsg((struct Message *)imsg);
CloseWindow(gwdw);

gwdw=openwindow(scr,1,10,318,180,(ULONG)ACTIVATE,
                (USHORT)(GADGETUP),NULL);
rport=gwdw->RPort;
SetAPen(rport,1L);
Move(rport,10L,10L);
Text(rport,"Ultimate Fractal Generator.",27L);

PutGadget("Z  >",4,10,20,(gad+39));
PutGadget("dZ <",4,110,20,(gad+42));
PutGadget("Re >",4,10,42,(gad+40));
PutGadget("Im >",4,110,42,(gad+41));

   Move(rport,240L,10L);
   switch(surface)
    {
     case D2RECT: { Text(rport,"2D-Rect",7L);break; };
     case D3RECT: { Text(rport,"3D-Rect",7L);
                    PutGadget("HeightConst",11,10,113,(gad+12));
                    break; };
     case SPHERE: { Text(rport,"Sphere",6L);
                    PutGadget("HeightConst",11,10,113,(gad+12));
                    PutGadget("Radius",6,110,113,(gad+13));
                    break; };
    };


  PutGadget("MaxIter",7,10,135,(gad+5));
  if(surface!=D2RECT)
    PutGadget("Step",4,110,135,(gad+14));

PutBoolGadget(210,160,(gadb+0));

Wait(1L << gwdw->UserPort->mp_SigBit);
while((imsg=(struct IntuiMessage *)GetMsg(gwdw->UserPort)))
      ReplyMsg((struct Message *)imsg);
CloseWindow(gwdw);

ShowTitle(scr,titelflag=1L);
}


void Ult_A()
{
Ult_re+=Ult_Z0re;
Ult_im+=Ult_Z0im;
}

void Ult_B()
{
Ult_re-=Ult_Z0re;
Ult_im-=Ult_Z0im;
}

void Ult_C()
{
float tmp;
tmp=Ult_re*Ult_Z0re-Ult_im*Ult_Z0im;
Ult_im=Ult_im*Ult_Z0re+Ult_re*Ult_Z0im;
Ult_re=tmp;
}

void Ult_D()
{
Ult_swplo();
Ult_re=Ult_Z0re;
Ult_im=Ult_Z0im;
Ult_j();
Ult_mmlo();
}

void Ult_E()
{
Ult_P();
Ult_C();
Ult_Q();
}

void Ult_F()
{
float tmp;
tmp=Ult_re*Ult_re-Ult_im*Ult_im;
Ult_im=2*Ult_re*Ult_im;
Ult_re=tmp;
}

void Ult_G()
{
float tmp,tmp2,tmp3;
tmp2=Ult_re*Ult_re;
tmp3=Ult_im*Ult_im;
tmp=Ult_re*(tmp2-3*tmp3);
Ult_im=Ult_im*(3*tmp2-tmp3);
Ult_re=tmp;
}

void Ult_H()
{
Ult_F();
Ult_F();
}

void Ult_I()
{
float tmp,tmp2,tmp3,tmp4,tmp5,tmp6;
tmp2=Ult_re*Ult_re;
tmp3=Ult_im*Ult_im;
tmp4=tmp2*tmp2;
tmp5=tmp2*tmp3;
tmp6=tmp3*tmp3;
tmp   =Ult_re*(tmp4-10*tmp5+5*tmp6);
Ult_im=Ult_im*(5*tmp4-10*tmp5+tmp6);
Ult_re=tmp;
}

void Ult_J()
{
float tmp,tmp2,tmp3,tmp4,tmp5,tmp6;
tmp2=Ult_re*Ult_re;
tmp3=Ult_im*Ult_im;
tmp4=tmp2*tmp2;
tmp5=tmp2*tmp3;
tmp6=tmp3*tmp3;
tmp=tmp2*(tmp4+15*(tmp6-tmp5))-tmp6*tmp3;
Ult_im=Ult_re*Ult_im*(6*(tmp4+tmp6)-20*tmp5);
Ult_re=tmp;
}

void Ult_K()
{
Ult_re+=Ult_Znre;
Ult_im+=Ult_Znim;
}

void Ult_L()
{
Ult_re-=Ult_Znre;
Ult_im-=Ult_Znim;
}

void Ult_M()
{
float tmp;
tmp=Ult_re*Ult_Znre-Ult_im*Ult_Znim;
Ult_im=Ult_im*Ult_Znre+Ult_re*Ult_Znim;
Ult_re=tmp;
}

void Ult_N()
{
Ult_swplo();
Ult_re=Ult_Znre;
Ult_im=Ult_Znim;
Ult_j();
Ult_mmlo();

}

void Ult_O()
{
Ult_P();
Ult_M();
Ult_Q();
}

void Ult_P()
{
float bel,arg,tmp2,tmp3;

tmp2=Ult_re*Ult_re;
tmp3=Ult_im*Ult_im;

bel=sqrt((double)(tmp2+tmp3));
if(Ult_re>0)arg=atan((double)(Ult_im/Ult_re));
else if((Ult_re<0)&&(Ult_im>0))arg= 3.1415+atan((double)(Ult_im/Ult_re));
else if((Ult_re<0)&&(Ult_im<0))arg= -3.1415+atan((double)(Ult_im/Ult_re));
else if(Ult_im>0)arg=1.5708;
else arg=-1.5708;

Ult_re=log((double)bel);
Ult_im=arg;
}

void Ult_Q()
{
float tmp;
tmp=exp((double)Ult_re);
Ult_re=tmp*cos((double)Ult_im);
Ult_im=tmp*sin((double)Ult_im);
}

void Ult_R()
{
float tmp,tmp2,tmp3;
tmp2=exp(-(double)Ult_im);
tmp3=exp( (double)Ult_im);
tmp=0.5*sin((double)Ult_re)*(tmp2+tmp3);
Ult_im= -0.5*cos((double)Ult_re)*(tmp2-tmp3);
Ult_re=tmp;
}

void Ult_S()
{
float tmp,tmp2,tmp3;
tmp2=exp(-(double)Ult_im);
tmp3=exp( (double)Ult_im);
tmp=0.5*cos((double)Ult_re)*(tmp2+tmp3);
Ult_im=0.5*sin((double)Ult_re)*(tmp2-tmp3);
Ult_re=tmp;
}

void Ult_T()
{
Ult_tmpre[10]=Ult_re;Ult_tmpim[10]=Ult_im;
Ult_S();
Ult_j();
Ult_swplo();
Ult_R();
Ult_mmlo();
}

void Ult_a()
{
short pos;
Ult_cnt++;
pos=rulebuf[Ult_cnt]-'0';
Ult_re+=Ult_tmpre[pos];
Ult_im+=Ult_tmpim[pos];
}

void Ult_b()
{
short pos;
Ult_cnt++;
pos=rulebuf[Ult_cnt]-'0';
Ult_re-=Ult_tmpre[pos];
Ult_im-=Ult_tmpim[pos];
}

void Ult_c()
{
float tmp;
short pos;
Ult_cnt++;
pos=rulebuf[Ult_cnt]-'0';
tmp=Ult_re*Ult_tmpre[pos]-Ult_im*Ult_tmpim[pos];
Ult_im=Ult_im*Ult_tmpre[pos]+Ult_re*Ult_tmpim[pos];
Ult_re=tmp;
}

void Ult_d()
{
short pos;
Ult_cnt++;
pos=rulebuf[Ult_cnt]-'0';

Ult_swplo();
Ult_re=Ult_tmpre[pos];
Ult_im=Ult_tmpim[pos];
Ult_j();
Ult_mmlo();
}

void Ult_e()
{
Ult_P();
Ult_c();
Ult_Q();
}

void Ult_f()
{
short pos;
Ult_cnt++;
pos=rulebuf[Ult_cnt]-'0';
Ult_tmpre[pos]=Ult_re;
Ult_tmpim[pos]=Ult_im;
}

void Ult_g()
{
short pos;
Ult_cnt++;
pos=rulebuf[Ult_cnt]-'0';
Ult_re=Ult_tmpre[pos];
Ult_im=Ult_tmpim[pos];
}

void Ult_h()
{
short pos;
float tmp;
Ult_cnt++;
pos=rulebuf[Ult_cnt]-'0';
tmp=Ult_re;
Ult_re=Ult_tmpre[pos];
Ult_tmpre[pos]=tmp;
tmp=Ult_im;
Ult_im=Ult_tmpim[pos];
Ult_tmpim[pos]=tmp;
}

void Ult_i()
{
Ult_re=Ult_im=0;
}

void Ult_j()
{
float tmp;
tmp=Ult_re*Ult_re+Ult_im*Ult_im;
if( fabs((double)tmp)<0.0000000000001 )tmp=0.0000000000001;
Ult_re/=tmp;
Ult_im= -Ult_im/tmp;
}

void Ult_k()
{
short pos;
Ult_cnt++;
pos=rulebuf[Ult_cnt]-'0';
Ult_re+=Ult_cre[pos];
Ult_im+=Ult_cim[pos];
}

void Ult_l()
{
short pos;
Ult_cnt++;
pos=rulebuf[pos]-'0';
Ult_re-=Ult_cre[pos];
Ult_im-=Ult_cim[pos];
}

void Ult_m()
{
float tmp;
short pos;
Ult_cnt++;
pos=rulebuf[Ult_cnt]-'0';
tmp=Ult_re*Ult_cre[pos]-Ult_im*Ult_cim[pos];
Ult_im=Ult_im*Ult_cre[pos]+Ult_re*Ult_cim[pos];
Ult_re=tmp;
}

void Ult_n()
{
short pos;
Ult_cnt++;
pos=rulebuf[Ult_cnt]-'0';

Ult_swplo();
Ult_re=Ult_cre[pos];
Ult_im=Ult_cim[pos];
Ult_j();
Ult_mmlo();
}

void Ult_o()
{
Ult_P();
Ult_m();
Ult_Q();
}

void Ult_p()
{
}

void Ult_q()
{
if( (Ult_re*Ult_re+Ult_im*Ult_im)>(Ult_Zmax*Ult_Zmax) )Ult_brk=1;
}

void Ult_r()
{
if( (fabs((double)Ult_re)>Ult_Zremax) ){ Ult_brk=1;counter++;};
}

void Ult_s()
{
if( (fabs((double)Ult_im)>Ult_Zimmax) ){ Ult_brk=1;counter+=2;};
}

void Ult_t()
{
if( (fabs((double)(Ult_re-Ult_Znre))+fabs((double)(Ult_im-Ult_Znim)))<Ult_dZmin )Ult_brk=1;
}

void Ult_swplo()
{
float tmp;
tmp=Ult_re;Ult_re=Ult_tmpre[10];Ult_tmpre[10]=tmp;
tmp=Ult_im;Ult_im=Ult_tmpim[10];Ult_tmpim[10]=tmp;
}

void Ult_swphi()
{
float tmp;
tmp=Ult_re;Ult_re=Ult_tmpre[11];Ult_tmpre[11]=tmp;
tmp=Ult_im;Ult_im=Ult_tmpim[11];Ult_tmpim[11]=tmp;
}

void Ult_mmlo()
{
float tmp;
tmp   =Ult_re*Ult_tmpre[10]-Ult_im*Ult_tmpim[10];
Ult_im=Ult_re*Ult_tmpim[10]+Ult_im*Ult_tmpre[10];
Ult_re=tmp;
}

void Ult_plo()
{
Ult_re+=Ult_tmpre[10];
Ult_im+=Ult_tmpim[10];
}

void ultimate()
{
char rule;

Ult_re=0;
Ult_im=0;
Ult_Znre=Ult_Z0re=resave;
Ult_Znim=Ult_Z0im=imsave;
Ult_brk=0;
Ult_cnt=0;
rule=rulebuf[Ult_cnt];

for(counter=0;counter<10;counter++)
   {
    Ult_tmpre[counter]=Ult_tmpim[counter]=0;
   };

for(counter=0;counter<maxiter;)
   {

   if( rule < 'K' )
     {
      switch(rule)
        {
         case 'A':{Ult_A();break;};
         case 'B':{Ult_B();break;};
         case 'C':{Ult_C();break;};
         case 'D':{Ult_D();break;};
         case 'E':{Ult_E();break;};
         case 'F':{Ult_F();break;};
         case 'G':{Ult_G();break;};
         case 'H':{Ult_H();break;};
         case 'I':{Ult_I();break;};
         case 'J':{Ult_J();break;};
        };
     }
   else
   if( rule < 'a' )
     {
      switch(rule)
        {
         case 'K':{Ult_K();break;};
         case 'L':{Ult_L();break;};
         case 'M':{Ult_M();break;};
         case 'N':{Ult_N();break;};
         case 'O':{Ult_O();break;};
         case 'P':{Ult_P();break;};
         case 'Q':{Ult_Q();break;};
         case 'R':{Ult_R();break;};
         case 'S':{Ult_S();break;};
         case 'T':{Ult_T();break;};
        };
     }
   else
   if( rule < 'k' )
     {
      switch(rule)
        {
         case 'a':{Ult_a();break;};
         case 'b':{Ult_b();break;};
         case 'c':{Ult_c();break;};
         case 'd':{Ult_d();break;};
         case 'e':{Ult_e();break;};
         case 'f':{Ult_f();break;};
         case 'g':{Ult_g();break;};
         case 'h':{Ult_R();break;};
         case 'i':{Ult_i();break;};
         case 'j':{Ult_j();break;};
        };
     }
   else
   if( rule < 'u' )
     {
      switch(rule)
        {
         case 'k':{Ult_k();break;};
         case 'l':{Ult_l();break;};
         case 'm':{Ult_m();break;};
         case 'n':{Ult_n();break;};
         case 'o':{Ult_o();break;};
         case 'p':{Ult_p();break;};
         case 'q':{Ult_q();break;};
         case 'r':{Ult_r();break;};
         case 's':{Ult_s();break;};
         case 't':{Ult_t();break;};
        };
     };

    if(++Ult_cnt >= rulelen)
      {
       if(Ult_brk!=0)return;
       Ult_cnt=0;
       counter++;    /* RuleCykle finished , one iteration complete */
       Ult_Znre=Ult_re;
       Ult_Znim=Ult_im;
       Ult_re=Ult_im=0;
      };
   rule=rulebuf[Ult_cnt];

   };

}

