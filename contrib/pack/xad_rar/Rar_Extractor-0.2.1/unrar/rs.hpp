
// Rar_Extractor 0.2.1. See unrar_license.txt.

#ifndef _RAR_RS_
#define _RAR_RS_

#define MAXPAR 255
#define MAXPOL 512

class RSCoder
{
  private:
    void gfInit();
    int gfMult(int a,int b);
    void pnInit();
    void pnMult(int *p1,int *p2,int *r);

    int gfExp[MAXPOL];
    int gfLog[MAXPAR+1];

    int GXPol[MAXPOL*2];

    int ErrorLocs[MAXPAR+1],ErrCount;
    int Dn[MAXPAR+1];

    int ParSize;
    int PolB[MAXPOL];
    bool FirstBlockDone;
  public:
    RSCoder(int ParSize);
    bool Decode(byte *Data,int DataSize,int *EraLoc,int EraSize);
};

#endif
