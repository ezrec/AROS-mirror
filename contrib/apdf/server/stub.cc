#include <string.h>

#ifdef BUGGY_DIVDI3

extern "C" long long PPCDivu64(long long x,long long y);

extern "C" long long __divdi3(long long x,long long y) {
    /* PPCDivs64 seems buggy too... */
    bool neg=false;
    if(x<0) {
	x=-x;
	neg=true;
    }
    if(y<0) {
	y=-y;
	neg=!neg;
    }
    x=PPCDivu64(x,y);
    return neg?-x:x;
}

#endif

#ifdef BUGGY_ATOI

extern "C" long strtol(const char *,const char **,int);

extern "C" int atoi(const char *s) {
    return (int)strtol(s,NULL,10);
}

#endif

#ifdef BUGGY_STRNCPY

extern "C" char *strncpy(char *p0,const char *q,size_t len) {
    char *p=p0;
    if(len) {
	do {
	    if(!(*p++=*q++)) {
		if(--len)
		    memset(p,0,len);
		break;
	    }
	} while(--len);
    }
    return p0;
}

#endif

