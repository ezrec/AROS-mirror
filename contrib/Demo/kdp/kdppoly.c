/* KDPpoly v0.8  bij wreck/knp ydhz/knp
 
  slow :(
*/


#include "kdppoly.h"

void KDPffpoly(KDPvertex *v1,KDPvertex *v2,KDPvertex *v3,UBYTE col,UBYTE *vmem)
{
  KDPvertex *temp;
  int dx1, dx2, dx3;
  int xp1, xp2, xp3;
  int xx;
  int rx1,rx2;
  int x13pos;
  UBYTE *vm;
  int j;
  float l;
  int i;
  float d;

  if (v1->y>v2->y)
        {
    temp=v1;v1=v2;v2=temp;
    }
  if (v1->y>v3->y)
        {
    temp=v1;v1=v3;v3=temp;
    }
  if (v2->y>v3->y)
        {
    temp=v2;v2=v3;v3=temp;
    }
  if((v1->y<0) && (v3->y<0)) return;
  if((v1->y>199) && (v3->y>199)) return;

  if((v1->x<0) && (v2->x<0) && (v3->x<0)) return;
  if((v1->x>319) && (v2->x>319) && (v3->x>319)) return;



  d=v2->y-v1->y;
  if (d!=0)
    dx1=(int)((float)((v2->x-v1->x)*65536)/d);
  else
    dx1=0;
  d=v3->y-v1->y;
  if (d!=0)
    dx2=(int)((float)((v3->x-v1->x)*65536)/d);
  else
    dx2=0;
  d=v3->y-v2->y;
  if (d!=0)
    dx3=(int)((float)((v3->x-v2->x)*65536)/d);
  else
    dx3=0;


  xp1=(int)(v1->x*65536);
  xp2=(int)(v1->x*65536);
  xp3=(int)(v2->x*65536);
  xx=xp3;

  d=(v2->y-v1->y);
  x13pos=(int)(xp1+(dx2*d));


  l=v1->x+((float)((float)dx2/65536)*(v2->y-v1->y));
  if (v2->x<l)
    {
    for (i=(int)v1->y; i<(int)v2->y; i++)
      {
      if((i>=0) && (i<200))
        {
        rx1=xp1>>16;
        rx2=xp2>>16;
        if(rx1<0)   rx1=0;
        if(rx2>319) rx2=319;
        vm=vmem+(320*i+rx1);
        for (j=0; j<=(rx2-rx1); j++)
          *(vm++)=col;
        }
      xp1+=dx1; xp2+=dx2;
      }
    for (i=(int)v2->y; i<(int)v3->y; i++)
      {
      if((i>=0) && (i<200))
        {
        rx1=xp3>>16;
        rx2=xp2>>16;
        if(rx1<0)   rx1=0;
        if(rx2>319) rx2=319;
        vm=vmem+(320*i+rx1);
        for (j=0; j<=(rx2-rx1); j++)
          *(vm++)=col;
        }
      xp3+=dx3; xp2+=dx2;
      }
    }
  else
    {
    for (i=(int)v1->y; i<(int)v2->y; i++)
      {
      if((i>=0) && (i<200))
        {
        rx1=xp2>>16;
        rx2=xp1>>16;
        if(rx1<0)   rx1=0;
        if(rx2>319) rx2=319;
        vm=vmem+(320*i+rx1);
        for (j=0; j<=(rx2-rx1); j++)
          *(vm++)=col;
        }
      xp2+=dx2; xp1+=dx1;
      }
    for (i=(int)v2->y; i<(int)v3->y; i++)
      {
      if((i>=0) && (i<200))
        {
        rx1=xp2>>16;
        rx2=xp3>>16;
        if(rx1<0)   rx1=0;
        if(rx2>319) rx2=319;
        vm=vmem+(320*i+rx1);
        for (j=0; j<=(rx2-rx1); j++)
          *(vm++)=col;
        }
      xp3+=dx3; xp2+=dx2;
      }
    }
}


void KDPfgpoly(KDPvertex *v1,KDPvertex *v2,KDPvertex *v3,UBYTE *vmem)
{
  KDPvertex *temp;
  int dx1, dx2, dx3;
  int dc1,dc2,dc3;
  int xp1, xp2, xp3;
  int cp1,cp2,cp3;
  int xx,cc;
  int rx1,rx2;
  int x13pos;
  int c13pos;
  int hc1;
  UBYTE *vm;
  int dc;
  int j;
  float l;
  int i;
  float d;
  float len;

  if (v1->y>v2->y)
        {
    temp=v1;v1=v2;v2=temp;
    }
  if (v1->y>v3->y)
        {
    temp=v1;v1=v3;v3=temp;
    }
  if (v2->y>v3->y)
        {
    temp=v2;v2=v3;v3=temp;
    }
  if((v1->y<0) && (v3->y<0)) return;
  if((v1->y>199) && (v3->y>199)) return;

  if((v1->x<0) && (v2->x<0) && (v3->x<0)) return;
  if((v1->x>319) && (v2->x>319) && (v3->x>319)) return;



  d=v2->y-v1->y;
  if (d!=0)
    {
    dx1=(int)((float)((v2->x-v1->x)*65536)/d);
    dc1=(int)((float)((v2->c-v1->c)*65536)/d);
    }
  else
    {
    dx1=0;
    dc1=0;
    }
  d=v3->y-v1->y;
  if (d!=0)
    {
    dx2=(int)((float)((v3->x-v1->x)*65536)/d);
    dc2=(int)((float)((v3->c-v1->c)*65536)/d);
    }
  else
    {
    dx2=0;
    dc2=0;
    }
  d=v3->y-v2->y;
  if (d!=0)
    {
    dx3=(int)((float)((v3->x-v2->x)*65536)/d);
    dc3=(int)((float)((v3->c-v2->c)*65536)/d);
    }
  else
    {
    dx3=0;
    dc3=0;
    }

  xp1=(int)(v1->x*65536);
  xp2=(int)(v1->x*65536);
  xp3=(int)(v2->x*65536);
  xx=xp3;
  cp1=(int)(v1->c*65536);
  cp2=(int)(v1->c*65536);
  cp3=(int)(v2->c*65536);
  cc=cp3;

  d=(v2->y-v1->y);
  x13pos=(int)(xp1+(dx2*d));
  c13pos=(int)(cp1+(dc2*d));

  l=v1->x+((float)((float)dx2/65536)*(v2->y-v1->y));
  if (v2->x<l)
    {
    d=(int)((float)(xp3-x13pos)/65536);
    if(d!=0)
      dc=(int)((float)(cp3-c13pos)/d);
    else
      dc=0;
    len=v2->y-v1->y;
    for (i=(int)v1->y; i<(int)v2->y; i++)
      {
      hc1=cp1;
      if((i>=0) && (i<200))
        {
        rx1=xp1>>16;
        rx2=xp2>>16;
        if(rx1<0)   {hc1+=(-rx1)*dc;rx1=0;}
        if(rx2>319) rx2=319;
        vm=vmem+(320*i+rx1);
        for (j=0; j<=(rx2-rx1); j++)
          {
          *(vm++)=hc1>>16;
          hc1+=dc;
          }
        }
      xp1+=dx1; xp2+=dx2;
      cp1+=dc1;
      }
    cp2+=(dc2*len);
    for (i=(int)v2->y; i<(int)v3->y; i++)
      {
      hc1=cp3;
      if((i>=0) && (i<200))
        {
        rx1=xp3>>16;
        rx2=xp2>>16;
        if(rx1<0)   {hc1+=(-rx1)*dc;rx1=0;}
        if(rx2>319) rx2=319;
        vm=vmem+(320*i+rx1);
        for (j=0; j<=(rx2-rx1); j++)
          {
          *(vm++)=hc1>>16;
          hc1+=dc;
          }
        }
      xp3+=dx3; xp2+=dx2;
      cp3+=dc3;
      }
    }
  else
    {
    d=(int)((float)(x13pos-(v2->x*65536))/65536);
    if (d!=0)
      dc=(int)((float)(c13pos-(v2->c*65536))/d);
    else
      dc=0;
    len=v1->y-v2->y;
    for (i=(int)v1->y; i<(int)v2->y; i++)
      {
      hc1=cp2;
      if((i>=0) && (i<200))
        {
        rx1=xp2>>16;
        rx2=xp1>>16;
        if(rx1<0)   {hc1+=(-rx1)*dc;rx1=0;}
        if(rx2>319) rx2=319;
        vm=vmem+(320*i+rx1);
        for (j=0; j<=(rx2-rx1); j++)
          {
          *(vm++)=hc1>>16;
          hc1+=dc;
          }
        }
      xp2+=dx2; xp1+=dx1;
      cp2+=dc2;
      }
    cp1+=(dc1*len);
    for (i=(int)v2->y; i<(int)v3->y; i++)
      {
      hc1=cp2;
      if((i>=0) && (i<200))
        {
        rx1=xp2>>16;
        rx2=xp3>>16;
        if(rx1<0)   {hc1+=(-rx1)*dc;rx1=0;}
        if(rx2>319) rx2=319;
        vm=vmem+(320*i+rx1);
        for (j=0; j<=(rx2-rx1); j++)
          {
          *(vm++)=hc1>>16;
          hc1+=dc;
          }
        }
      xp3+=dx3; xp2+=dx2;
      cp2+=dc2;
      }
    }
}


void KDPfgcpoly(KDPvertex *v1,KDPvertex *v2,KDPvertex *v3,UBYTE col,UBYTE *vmem)
{
  KDPvertex *temp;
  int o=256*col;
  int dx1, dx2, dx3;
  int dc1,dc2,dc3;
  int xp1, xp2, xp3;
  int cp1,cp2,cp3;
  int xx,cc;
  int rx1,rx2;
  int x13pos;
  int c13pos;
  int hc1;
  UBYTE *vm;
  int dc;
  int j;
  float l;
  int i;
  float d;
  float len;

  if (v1->y>v2->y)
        {
    temp=v1;v1=v2;v2=temp;
    }
  if (v1->y>v3->y)
        {
    temp=v1;v1=v3;v3=temp;
    }
  if (v2->y>v3->y)
        {
    temp=v2;v2=v3;v3=temp;
    }
  if((v1->y<0) && (v3->y<0)) return;
  if((v1->y>199) && (v3->y>199)) return;

  if((v1->x<0) && (v2->x<0) && (v3->x<0)) return;
  if((v1->x>319) && (v2->x>319) && (v3->x>319)) return;



  d=v2->y-v1->y;
  if (d!=0)
    {
    dx1=(int)((float)((v2->x-v1->x)*65536)/d);
    dc1=(int)((float)((v2->c-v1->c)*65536)/d);
    }
  else
    {
    dx1=0;
    dc1=0;
    }
  d=v3->y-v1->y;
  if (d!=0)
    {
    dx2=(int)((float)((v3->x-v1->x)*65536)/d);
    dc2=(int)((float)((v3->c-v1->c)*65536)/d);
    }
  else
    {
    dx2=0;
    dc2=0;
    }
  d=v3->y-v2->y;
  if (d!=0)
    {
    dx3=(int)((float)((v3->x-v2->x)*65536)/d);
    dc3=(int)((float)((v3->c-v2->c)*65536)/d);
    }
  else
    {
    dx3=0;
    dc3=0;
    }

  xp1=(int)(v1->x*65536);
  xp2=(int)(v1->x*65536);
  xp3=(int)(v2->x*65536);
  xx=xp3;
  cp1=(int)(v1->c*65536);
  cp2=(int)(v1->c*65536);
  cp3=(int)(v2->c*65536);
  cc=cp3;

  d=(v2->y-v1->y);
  x13pos=(int)(xp1+(dx2*d));
  c13pos=(int)(cp1+(dc2*d));

  l=v1->x+((float)((float)dx2/65536)*(v2->y-v1->y));
  if (v2->x<l)
    {
    d=(int)((float)(xp3-x13pos)/65536);
    if(d!=0)
      dc=(int)((float)(cp3-c13pos)/d);
    else
      dc=0;
    len=v2->y-v1->y;
    for (i=(int)v1->y; i<(int)v2->y; i++)
      {
      hc1=cp1;
      if((i>=0) && (i<200))
        {
        rx1=xp1>>16;
        rx2=xp2>>16;
        if(rx1<0)   {hc1+=(-rx1)*dc;rx1=0;}
        if(rx2>319) rx2=319;
        vm=vmem+(320*i+rx1);
        for (j=0; j<=(rx2-rx1); j++)
          {
          *(vm++)=KDPcgcoltab[o+(hc1>>16)];
          hc1+=dc;
          }
        }
      xp1+=dx1; xp2+=dx2;
      cp1+=dc1;
      }
    cp2+=(dc2*len);
    for (i=(int)v2->y; i<(int)v3->y; i++)
      {
      hc1=cp3;
      if((i>=0) && (i<200))
        {
        rx1=xp3>>16;
        rx2=xp2>>16;
        if(rx1<0)   {hc1+=(-rx1)*dc;rx1=0;}
        if(rx2>319) rx2=319;
        vm=vmem+(320*i+rx1);
        for (j=0; j<=(rx2-rx1); j++)
          {
          *(vm++)=KDPcgcoltab[o+(hc1>>16)];
          hc1+=dc;
          }
        }
      xp3+=dx3; xp2+=dx2;
      cp3+=dc3;
      }
    }
  else
    {
    d=(int)((float)(x13pos-(v2->x*65536))/65536);
    if (d!=0)
      dc=(int)((float)(c13pos-(v2->c*65536))/d);
    else
      dc=0;
    len=v1->y-v2->y;
    for (i=(int)v1->y; i<(int)v2->y; i++)
      {
      hc1=cp2;
      if((i>=0) && (i<200))
        {
        rx1=xp2>>16;
        rx2=xp1>>16;
        if(rx1<0)   {hc1+=(-rx1)*dc;rx1=0;}
        if(rx2>319) rx2=319;
        vm=vmem+(320*i+rx1);
        for (j=0; j<=(rx2-rx1); j++)
          {
          *(vm++)=KDPcgcoltab[o+(hc1>>16)];
          hc1+=dc;
          }
        }
      xp2+=dx2; xp1+=dx1;
      cp2+=dc2;
      }
    cp1+=(dc1*len);
    for (i=(int)v2->y; i<(int)v3->y; i++)
      {
      hc1=cp2;
      if((i>=0) && (i<200))
        {
        rx1=xp2>>16;
        rx2=xp3>>16;
        if(rx1<0)   {hc1+=(-rx1)*dc;rx1=0;}
        if(rx2>319) rx2=319;
        vm=vmem+(320*i+rx1);
        for (j=0; j<=(rx2-rx1); j++)
          {
          *(vm++)=KDPcgcoltab[o+(hc1>>16)];
          hc1+=dc;
          }
        }
      xp3+=dx3; xp2+=dx2;
      cp2+=dc2;
      }
    }
}


void KDPftpoly(KDPvertex *v1,KDPvertex *v2,KDPvertex *v3,UBYTE *tex,UBYTE *vmem)
{
  KDPvertex *temp;
  int dx1, dx2, dx3;
  int du1,du2,du3;
  int dv1,dv2,dv3;
  int xp1, xp2, xp3;
  int up1,up2,up3;
  int vp1,vp2,vp3;
  int xx,uu,vv;
  int rx1,rx2;
  int x13pos;
  int u13pos,v13pos;
  int hu1,hv1;
  UBYTE *vm;
  int du,dv;
  int j;
  float l;
  int i;
  float d;
  float len;

  if (v1->y>v2->y)
        {
    temp=v1;v1=v2;v2=temp;
    }
  if (v1->y>v3->y)
        {
    temp=v1;v1=v3;v3=temp;
    }
  if (v2->y>v3->y)
        {
    temp=v2;v2=v3;v3=temp;
    }
  if((v1->y<0) && (v3->y<0)) return;
  if((v1->y>199) && (v3->y>199)) return;

  if((v1->x<0) && (v2->x<0) && (v3->x<0)) return;
  if((v1->x>319) && (v2->x>319) && (v3->x>319)) return;



  d=v2->y-v1->y;
  if (d!=0)
    {
    dx1=(int)((float)((v2->x-v1->x)*65536)/d);
    du1=(int)((float)((v2->u-v1->u)*65536)/d);
    dv1=(int)((float)((v2->v-v1->v)*65536)/d);
    }
  else
    {
    dx1=0;
    du1=0;
    dv1=0;
    }
  d=v3->y-v1->y;
  if (d!=0)
    {
    dx2=(int)((float)((v3->x-v1->x)*65536)/d);
    du2=(int)((float)((v3->u-v1->u)*65536)/d);
    dv2=(int)((float)((v3->v-v1->v)*65536)/d);
    }
  else
    {
    dx2=0;
    du2=0;
    dv2=0;
    }
  d=v3->y-v2->y;
  if (d!=0)
    {
    dx3=(int)((float)((v3->x-v2->x)*65536)/d);
    du3=(int)((float)((v3->u-v2->u)*65536)/d);
    dv3=(int)((float)((v3->v-v2->v)*65536)/d);
    }
  else
    {
    dx3=0;
    du3=0;
    dv3=0;
    }

  xp1=(int)(v1->x*65536);
  xp2=(int)(v1->x*65536);
  xp3=(int)(v2->x*65536);
  xx=xp3;
  up1=(int)(v1->u*65536);
  up2=(int)(v1->u*65536);
  up3=(int)(v2->u*65536);
  uu=up3;
  vp1=(int)(v1->v*65536);
  vp2=(int)(v1->v*65536);
  vp3=(int)(v2->v*65536);
  vv=vp3;



  d=(v2->y-v1->y);
  x13pos=(int)(xp1+(dx2*d));
  u13pos=(int)(up1+(du2*d));
  v13pos=(int)(vp1+(dv2*d));



  l=v1->x+((float)((float)dx2/65536)*(v2->y-v1->y));
  if (v2->x<l)
    {
    d=(int)((float)(xp3-x13pos)/65536);
    if(d!=0)
      {
      du=(int)((float)(up3-u13pos)/d);
      dv=(int)((float)(vp3-v13pos)/d);
      }
    else
      {
      du=0;
      dv=0;
      }
    len=v2->y-v1->y;
    for (i=(int)v1->y; i<(int)v2->y; i++)
      {
      hv1=vp1;hu1=up1;
      if((i>=0) && (i<200))
        {
        rx1=xp1>>16;
        rx2=xp2>>16;
        if(rx1<0)   {hu1+=(-rx1)*du;hv1+=(-rx1)*dv;rx1=0;}
        if(rx2>319) rx2=319;
        vm=vmem+(320*i+rx1);
        for (j=0; j<=(rx2-rx1); j++)
          {
          *(vm++)=tex[(((hv1>>8)&0xff00)+(hu1>>16))&0xffff];
          hu1+=du;hv1+=dv;
          }
        }
      xp1+=dx1; xp2+=dx2;
      up1+=du1; vp1+=dv1;
      }
    up2+=(du2*len);vp2+=(dv2*len);
    for (i=(int)v2->y; i<(int)v3->y; i++)
      {
      hv1=vp3;hu1=up3;
      if((i>=0) && (i<200))
        {
        rx1=xp3>>16;
        rx2=xp2>>16;
        if(rx1<0)   {hu1+=(-rx1)*du;hv1+=(-rx1)*dv;rx1=0;}
        if(rx2>319) rx2=319;
        vm=vmem+(320*i+rx1);
        for (j=0; j<=(rx2-rx1); j++)
          {
          *(vm++)=tex[(((hv1>>8)&0xff00)+(hu1>>16))&0xffff];
          hu1+=du;hv1+=dv;
          }
        }
      xp3+=dx3; xp2+=dx2;
      up3+=du3; vp3+=dv3;
      }
    }
  else
    {
    d=(int)((float)(x13pos-(v2->x*65536))/65536);
    if (d!=0)
      {
      du=(int)((float)(u13pos-(v2->u*65536))/d);
      dv=(int)((float)(v13pos-(v2->v*65536))/d);
      }
    else
      {
      du=0;dv=0;
      }
    len=v1->y-v2->y;
    for (i=(int)v1->y; i<(int)v2->y; i++)
      {
      hv1=vp2;hu1=up2;
      if((i>=0) && (i<200))
        {
        rx1=xp2>>16;
        rx2=xp1>>16;
        if(rx1<0)   {hu1+=(-rx1)*du;hv1+=(-rx1)*dv;rx1=0;}
        if(rx2>319) rx2=319;
        vm=vmem+(320*i+rx1);
        for (j=0; j<=(rx2-rx1); j++)
          {
          *(vm++)=tex[(((hv1>>8)&0xff00)+(hu1>>16))&0xffff];
          hu1+=du;hv1+=dv;
          }
        }
      xp2+=dx2; xp1+=dx1;
      up2+=du2; vp2+=dv2;
      }
    up1+=(du1*len);vp1+=(dv1*len);
    for (i=(int)v2->y; i<(int)v3->y; i++)
      {
      hv1=vp2;hu1=up2;
      if((i>=0) && (i<200))
        {
        rx1=xp2>>16;
        rx2=xp3>>16;
        if(rx1<0)   {hu1+=(-rx1)*du;hv1+=(-rx1)*dv;rx1=0;}
        if(rx2>319) rx2=319;
        vm=vmem+(320*i+rx1);
        for (j=0; j<=(rx2-rx1); j++)
          {
          *(vm++)=tex[(((hv1>>8)&0xff00)+(hu1>>16))&0xffff];
          hu1+=du;hv1+=dv;
          }
        }
      xp3+=dx3; xp2+=dx2;
      up2+=du2; vp2+=dv2;
      }
    }
}

void KDPfftpoly(KDPvertex *v1,KDPvertex *v2,KDPvertex *v3,UBYTE col,UBYTE *tex,UBYTE *vmem)
{
  KDPvertex *temp;
  int dx1, dx2, dx3;
  int du1,du2,du3;
  int dv1,dv2,dv3;
  int xp1, xp2, xp3;
  int up1,up2,up3;
  int vp1,vp2,vp3;
  int xx,uu,vv;
  int rx1,rx2;
  int x13pos;
  int u13pos,v13pos;
  int hu1,hv1;
  UBYTE *vm;
  int du,dv;
  int j;
  float l;
  int i;
  float d;
  float len;

  if (v1->y>v2->y)
        {
    temp=v1;v1=v2;v2=temp;
    }
  if (v1->y>v3->y)
        {
    temp=v1;v1=v3;v3=temp;
    }
  if (v2->y>v3->y)
        {
    temp=v2;v2=v3;v3=temp;
    }
  if((v1->y<0) && (v3->y<0)) return;
  if((v1->y>199) && (v3->y>199)) return;

  if((v1->x<0) && (v2->x<0) && (v3->x<0)) return;
  if((v1->x>319) && (v2->x>319) && (v3->x>319)) return;



  d=v2->y-v1->y;
  if (d!=0)
    {
    dx1=(int)((float)((v2->x-v1->x)*65536)/d);
    du1=(int)((float)((v2->u-v1->u)*65536)/d);
    dv1=(int)((float)((v2->v-v1->v)*65536)/d);
    }
  else
    {
    dx1=0;
    du1=0;
    dv1=0;
    }
  d=v3->y-v1->y;
  if (d!=0)
    {
    dx2=(int)((float)((v3->x-v1->x)*65536)/d);
    du2=(int)((float)((v3->u-v1->u)*65536)/d);
    dv2=(int)((float)((v3->v-v1->v)*65536)/d);
    }
  else
    {
    dx2=0;
    du2=0;
    dv2=0;
    }
  d=v3->y-v2->y;
  if (d!=0)
    {
    dx3=(int)((float)((v3->x-v2->x)*65536)/d);
    du3=(int)((float)((v3->u-v2->u)*65536)/d);
    dv3=(int)((float)((v3->v-v2->v)*65536)/d);
    }
  else
    {
    dx3=0;
    du3=0;
    dv3=0;
    }

  xp1=(int)(v1->x*65536);
  xp2=(int)(v1->x*65536);
  xp3=(int)(v2->x*65536);
  xx=xp3;
  up1=(int)(v1->u*65536);
  up2=(int)(v1->u*65536);
  up3=(int)(v2->u*65536);
  uu=up3;
  vp1=(int)(v1->v*65536);
  vp2=(int)(v1->v*65536);
  vp3=(int)(v2->v*65536);
  vv=vp3;



  d=(v2->y-v1->y);
  x13pos=(int)(xp1+(dx2*d));
  u13pos=(int)(up1+(du2*d));
  v13pos=(int)(vp1+(dv2*d));



  l=v1->x+((float)((float)dx2/65536)*(v2->y-v1->y));
  if (v2->x<l)
    {
    d=(int)((float)(xp3-x13pos)/65536);
    if(d!=0)
      {
      du=(int)((float)(up3-u13pos)/d);
      dv=(int)((float)(vp3-v13pos)/d);
      }
    else
      {
      du=0;
      dv=0;
      }
    len=v2->y-v1->y;
    for (i=(int)v1->y; i<(int)v2->y; i++)
      {
      hv1=vp1;hu1=up1;
      if((i>=0) && (i<200))
        {
        rx1=xp1>>16;
        rx2=xp2>>16;
        if(rx1<0)   {hu1+=(-rx1)*du;hv1+=(-rx1)*dv;rx1=0;}
        if(rx2>319) rx2=319;
        vm=vmem+(320*i+rx1);
        for (j=0; j<=(rx2-rx1); j++)
          {
          *(vm++)=KDPcgcoltab[(tex[(((hv1>>8)&0xff00)+(hu1>>16))&0xffff]<<8)+col];
          hu1+=du;hv1+=dv;
          }
        }
      xp1+=dx1; xp2+=dx2;
      up1+=du1; vp1+=dv1;
      }
    up2+=(du2*len);vp2+=(dv2*len);
    for (i=(int)v2->y; i<(int)v3->y; i++)
      {
      hv1=vp3;hu1=up3;
      if((i>=0) && (i<200))
        {
        rx1=xp3>>16;
        rx2=xp2>>16;
        if(rx1<0)   {hu1+=(-rx1)*du;hv1+=(-rx1)*dv;rx1=0;}
        if(rx2>319) rx2=319;
        vm=vmem+(320*i+rx1);
        for (j=0; j<=(rx2-rx1); j++)
          {
          *(vm++)=KDPcgcoltab[(tex[(((hv1>>8)&0xff00)+(hu1>>16))&0xffff]<<8)+col];
          hu1+=du;hv1+=dv;
          }
        }
      xp3+=dx3; xp2+=dx2;
      up3+=du3; vp3+=dv3;
      }
    }
  else
    {
    d=(int)((float)(x13pos-(v2->x*65536))/65536);
    if (d!=0)
      {
      du=(int)((float)(u13pos-(v2->u*65536))/d);
      dv=(int)((float)(v13pos-(v2->v*65536))/d);
      }
    else
      {
      du=0;dv=0;
      }
    len=v1->y-v2->y;
    for (i=(int)v1->y; i<(int)v2->y; i++)
      {
      hv1=vp2;hu1=up2;
      if((i>=0) && (i<200))
        {
        rx1=xp2>>16;
        rx2=xp1>>16;
        if(rx1<0)   {hu1+=(-rx1)*du;hv1+=(-rx1)*dv;rx1=0;}
        if(rx2>319) rx2=319;
        vm=vmem+(320*i+rx1);
        for (j=0; j<=(rx2-rx1); j++)
          {
          *(vm++)=KDPcgcoltab[(tex[(((hv1>>8)&0xff00)+(hu1>>16))&0xffff]<<8)+col];
          hu1+=du;hv1+=dv;
          }
        }
      xp2+=dx2; xp1+=dx1;
      up2+=du2; vp2+=dv2;
      }
    up1+=(du1*len);vp1+=(dv1*len);
    for (i=(int)v2->y; i<(int)v3->y; i++)
      {
      hv1=vp2;hu1=up2;
      if((i>=0) && (i<200))
        {
        rx1=xp2>>16;
        rx2=xp3>>16;
        if(rx1<0)   {hu1+=(-rx1)*du;hv1+=(-rx1)*dv;rx1=0;}
        if(rx2>319) rx2=319;
        vm=vmem+(320*i+rx1);
        for (j=0; j<=(rx2-rx1); j++)
          {
          *(vm++)=KDPcgcoltab[(tex[(((hv1>>8)&0xff00)+(hu1>>16))&0xffff]<<8)+col];

          hu1+=du;hv1+=dv;
          }
        }
      xp3+=dx3; xp2+=dx2;
      up2+=du2; vp2+=dv2;
      }
    }
}


void KDPfgtpoly(KDPvertex *v1,KDPvertex *v2,KDPvertex *v3,UBYTE *tex,UBYTE *vmem)
{
  KDPvertex *temp;
  int dx1, dx2, dx3;
  int du1,du2,du3;
  int dv1,dv2,dv3;
  int dc1,dc2,dc3;
  int xp1, xp2, xp3;
  int up1,up2,up3;
  int vp1,vp2,vp3;
  int cp1,cp2,cp3;
  int xx,uu,vv,cc;
  int rx1,rx2;
  int x13pos;
  int u13pos,v13pos,c13pos;
  int hu1,hv1,hc1;
  UBYTE *vm;
  int du,dv,dc;
  int j;
  float l;
  int i;
  float d;
  float len;

  if (v1->y>v2->y)
        {
    temp=v1;v1=v2;v2=temp;
    }
  if (v1->y>v3->y)
        {
    temp=v1;v1=v3;v3=temp;
    }
  if (v2->y>v3->y)
        {
    temp=v2;v2=v3;v3=temp;
    }
  if((v1->y<0) && (v3->y<0)) return;
  if((v1->y>199) && (v3->y>199)) return;

  if((v1->x<0) && (v2->x<0) && (v3->x<0)) return;
  if((v1->x>319) && (v2->x>319) && (v3->x>319)) return;



  d=v2->y-v1->y;
  if (d!=0)
    {
    dx1=(int)((float)((v2->x-v1->x)*65536)/d);
    du1=(int)((float)((v2->u-v1->u)*65536)/d);
    dv1=(int)((float)((v2->v-v1->v)*65536)/d);
    dc1=(int)((float)((v2->c-v1->c)*65536)/d);
    }
  else
    {
    dx1=0;
    du1=0;
    dv1=0;
    dc1=0;
    }
  d=v3->y-v1->y;
  if (d!=0)
    {
    dx2=(int)((float)((v3->x-v1->x)*65536)/d);
    du2=(int)((float)((v3->u-v1->u)*65536)/d);
    dv2=(int)((float)((v3->v-v1->v)*65536)/d);
    dc2=(int)((float)((v3->c-v1->c)*65536)/d);
    }
  else
    {
    dx2=0;
    du2=0;
    dv2=0;
    dc2=0;
    }
  d=v3->y-v2->y;
  if (d!=0)
    {
    dx3=(int)((float)((v3->x-v2->x)*65536)/d);
    du3=(int)((float)((v3->u-v2->u)*65536)/d);
    dv3=(int)((float)((v3->v-v2->v)*65536)/d);
    dc3=(int)((float)((v3->c-v2->c)*65536)/d);
    }
  else
    {
    dx3=0;
    du3=0;
    dv3=0;
    dc3=0;
    }

  xp1=(int)(v1->x*65536);
  xp2=(int)(v1->x*65536);
  xp3=(int)(v2->x*65536);
  xx=xp3;
  up1=(int)(v1->u*65536);
  up2=(int)(v1->u*65536);
  up3=(int)(v2->u*65536);
  uu=up3;
  vp1=(int)(v1->v*65536);
  vp2=(int)(v1->v*65536);
  vp3=(int)(v2->v*65536);
  vv=vp3;
  cp1=(int)(v1->c*65536);
  cp2=(int)(v1->c*65536);
  cp3=(int)(v2->c*65536);
  cc=cp3;


  d=(v2->y-v1->y);
  x13pos=(int)(xp1+(dx2*d));
  u13pos=(int)(up1+(du2*d));
  v13pos=(int)(vp1+(dv2*d));
  c13pos=(int)(cp1+(dc2*d));


  l=v1->x+((float)((float)dx2/65536)*(v2->y-v1->y));
  if (v2->x<l)
    {
    d=(int)((float)(xp3-x13pos)/65536);
    if(d!=0)
      {
      du=(int)((float)(up3-u13pos)/d);
      dv=(int)((float)(vp3-v13pos)/d);
      dc=(int)((float)(cp3-c13pos)/d);
      }
    else
      {
      du=0;
      dv=0;
      dc=0;
      }
    len=v2->y-v1->y;
    for (i=(int)v1->y; i<(int)v2->y; i++)
      {
      hv1=vp1;hu1=up1;hc1=cp1;
      if((i>=0) && (i<200))
        {
        rx1=xp1>>16;
        rx2=xp2>>16;
        if(rx1<0)   {hu1+=(-rx1)*du;hv1+=(-rx1)*dv;hc1+=(-rx1)*dc;rx1=0;}
        if(rx2>319) rx2=319;
        vm=vmem+(320*i+rx1);
        for (j=0; j<=(rx2-rx1); j++)
          {
          *(vm++)=KDPcgcoltab[(tex[(((hv1>>8)&0xff00)+(hu1>>16))&0xffff]<<8)+(hc1>>16)];
          hu1+=du;hv1+=dv;hc1+=dc;
          }
        }
      xp1+=dx1; xp2+=dx2;
      up1+=du1; vp1+=dv1; cp1+=dc1;
      }
    up2+=(du2*len);vp2+=(dv2*len); cp2+=(dc2*len);
    for (i=(int)v2->y; i<(int)v3->y; i++)
      {
      hv1=vp3;hu1=up3;hc1=cp3;
      if((i>=0) && (i<200))
        {
        rx1=xp3>>16;
        rx2=xp2>>16;
        if(rx1<0)   {hu1+=(-rx1)*du;hv1+=(-rx1)*dv;hc1+=(-rx1)*dc;rx1=0;}
        if(rx2>319) rx2=319;
        vm=vmem+(320*i+rx1);
        for (j=0; j<=(rx2-rx1); j++)
          {
          *(vm++)=KDPcgcoltab[(tex[(((hv1>>8)&0xff00)+(hu1>>16))&0xffff]<<8)+(hc1>>16)];
          hu1+=du;hv1+=dv;hc1+=dc;
          }
        }
      xp3+=dx3; xp2+=dx2;
      up3+=du3; vp3+=dv3; cp3+=dc3;
      }
    }
  else
    {
    d=(int)((float)(x13pos-(v2->x*65536))/65536);
    if (d!=0)
      {
      du=(int)((float)(u13pos-(v2->u*65536))/d);
      dv=(int)((float)(v13pos-(v2->v*65536))/d);
      dc=(int)((float)(c13pos-(v2->c*65536))/d);
      }
    else
      {
      du=0;dv=0;dc=0;
      }
    len=v1->y-v2->y;
    for (i=(int)v1->y; i<(int)v2->y; i++)
      {
      hv1=vp2;hu1=up2;hc1=cp2;
      if((i>=0) && (i<200))
        {
        rx1=xp2>>16;
        rx2=xp1>>16;
        if(rx1<0)   {hu1+=(-rx1)*du;hv1+=(-rx1)*dv;hc1+=(-rx1)*dc;rx1=0;}
        if(rx2>319) rx2=319;
        vm=vmem+(320*i+rx1);
        for (j=0; j<=(rx2-rx1); j++)
          {
          *(vm++)=KDPcgcoltab[(tex[(((hv1>>8)&0xff00)+(hu1>>16))&0xffff]<<8)+(hc1>>16)];
          hu1+=du;hv1+=dv;hc1+=dc;
          }
        }
      xp2+=dx2; xp1+=dx1;
      up2+=du2; vp2+=dv2;cp2+=dc2;
      }
    up1+=(du1*len);vp1+=(dv1*len); cp1+=(dc1*len);
    for (i=(int)v2->y; i<(int)v3->y; i++)
      {
      hv1=vp2;hu1=up2;hc1=cp2;
      if((i>=0) && (i<200))
        {
        rx1=xp2>>16;
        rx2=xp3>>16;
        if(rx1<0)   {hu1+=(-rx1)*du;hv1+=(-rx1)*dv;hc1+=(-rx1)*dc;rx1=0;}
        if(rx2>319) rx2=319;
        vm=vmem+(320*i+rx1);
        for (j=0; j<=(rx2-rx1); j++)
          {
          *(vm++)=KDPcgcoltab[(tex[(((hv1>>8)&0xff00)+(hu1>>16))&0xffff]<<8)+(hc1>>16)];
          hu1+=du;hv1+=dv;hc1+=dc;
          }
        }
      xp3+=dx3; xp2+=dx2;
      up2+=du2; vp2+=dv2; cp2+=dc2;
      }
    }
}


void KDPdrawPoly(KDPface *fac,KDPvertex *vert,UBYTE *vmem)
        {
        switch (fac->type)
                {
                case 0: // point
                  KDPpset((int)vert[fac->a].x,(int)vert[fac->a].y,(UBYTE)vert[fac->a].c,vmem);
                  KDPpset((int)vert[fac->b].x,(int)vert[fac->b].y,(UBYTE)vert[fac->a].c,vmem);
                  KDPpset((int)vert[fac->c].x,(int)vert[fac->c].y,(UBYTE)vert[fac->a].c,vmem);
                  break;
                case 1:
                  KDPline((int)vert[fac->a].x,(int)vert[fac->a].y,
                           (int)vert[fac->b].x,(int)vert[fac->b].y,(UBYTE)fac->color,vmem);
                  KDPline((int)vert[fac->b].x,(int)vert[fac->b].y,
                           (int)vert[fac->c].x,(int)vert[fac->c].y,(UBYTE)fac->color,vmem);
                  KDPline((int)vert[fac->c].x,(int)vert[fac->c].y,
                           (int)vert[fac->a].x,(int)vert[fac->a].y,(UBYTE)fac->color,vmem);
                  break;
                case 2:
                  KDPline2((int)vert[fac->a].x,(int)vert[fac->a].y,
                           (int)vert[fac->b].x,(int)vert[fac->b].y,(UBYTE)fac->color,vmem);
                  KDPline2((int)vert[fac->b].x,(int)vert[fac->b].y,
                           (int)vert[fac->c].x,(int)vert[fac->c].y,(UBYTE)fac->color,vmem);
                  KDPline2((int)vert[fac->c].x,(int)vert[fac->c].y,
                           (int)vert[fac->a].x,(int)vert[fac->a].y,(UBYTE)fac->color,vmem);
                  break;
                case 3:
                  KDPffpoly(&vert[fac->a],&vert[fac->b],&vert[fac->c],(UBYTE)fac->color,vmem);
                  break;
                case 4:
                  KDPfgpoly(&vert[fac->a],&vert[fac->b],&vert[fac->c],vmem);
                  break;
                case 5:
                  KDPfgcpoly(&vert[fac->a],&vert[fac->b],&vert[fac->c],(UBYTE)fac->color,vmem);
                  break;
                case 6:
                  KDPftpoly(&vert[fac->a],&vert[fac->b],&vert[fac->c],fac->texture,vmem);
                  break;
                case 7:
                  KDPfftpoly(&vert[fac->a],&vert[fac->b],&vert[fac->c],(UBYTE)fac->color,fac->texture,vmem);
                  break;
                case 8:
                  KDPfgtpoly(&vert[fac->a],&vert[fac->b],&vert[fac->c],fac->texture,vmem);
                  break;
                }
        }

