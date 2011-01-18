#ifndef CONFIG_H
#define CONFIG_H 1

#include <exec/types.h>

struct Config0001
{
    Column wwcol;	    /*	word wrap column		    */

    UWORD winx;	    /*	save state of non-icon window	    */
    UWORD winy;
    UWORD winwidth;
    UWORD winheight;

    UWORD iwinx, iwiny;    /*	save state of icon window	    */

    UWORD aslleft;	    /* pos of FileReq */
    UWORD asltop;
    UWORD aslwidth;
    UWORD aslheight;

    Column tabstop;	    /* size of tabs */
    Column margin;

    int   fgpen;	    /* Pens */
    int   bgpen;
    int   hgpen;
    int   tpen;
    int   bbpen;

    char insertmode;	/* insert or overwrite */
    char ignorecase;	/* should SEARCH ignore the case ? */
    char wordwrap;	/* do wordwrap ? */
    char autosplit;	/* Split line automatically ? */
    char autoindent;	/* Autoindent cursor after <RETURN> ? */
    char autounblock;	/* Automatic Unblock on new BLOCK-command ? */
};

#define CONFIG0001	    "XDMECNFG0001"
#define CONFIG0001_SIZE     sizeof(struct Config0001)

struct Config0002
{
    Column wwcol;	    /*	word wrap column		    */

    UWORD winx; 	    /*	save state of non-icon window	    */
    UWORD winy;
    UWORD winwidth;
    UWORD winheight;

    UWORD iwinx, iwiny;     /*	save state of icon window	    */

    UWORD aslleft;	    /* pos of FileReq */
    UWORD asltop;
    UWORD aslwidth;
    UWORD aslheight;

    Column tabstop;	    /* size of tabs */
    Column margin;

    int   fgpen;	    /* Pens */
    int   bgpen;
    int   hgpen;
    int   tpen;
    int   bbpen;

    ULONG insertmode : 1;   /* insert or overwrite */
    ULONG ignorecase : 1;   /* should SEARCH ignore the case ? */
    ULONG wordwrap : 1;     /* do wordwrap ? */
    ULONG autosplit : 1;    /* Split line automatically ? */
    ULONG autoindent : 1;   /* Autoindent cursor after <RETURN> ? */
    ULONG autounblock : 1;  /* Automatic Unblock on new BLOCK-command ? */
};

#define CONFIG0002	    "XDMECNFG0002"
#define CONFIG0002_SIZE     sizeof(struct Config0002)

struct Config0003
{
    Column wwcol;	    /*	word wrap column		    */

    UWORD winx; 	    /*	save state of non-icon window	    */
    UWORD winy;
    UWORD winwidth;
    UWORD winheight;

    UWORD iwinx, iwiny;     /*	save state of icon window	    */

    UWORD aslleft;	    /* pos of FileReq */
    UWORD asltop;
    UWORD aslwidth;
    UWORD aslheight;

    Column tabstop;	    /* size of tabs */
    Column margin;

    UBYTE fgpen;	    /* Pens */
    UBYTE bgpen;
    UBYTE hgpen;
    UBYTE tpen;
    UBYTE bbpen;

    ULONG insertmode : 1;   /* insert or overwrite */
    ULONG ignorecase : 1;   /* should SEARCH ignore the case ? */
    ULONG wordwrap : 1;     /* do wordwrap ? */
    ULONG autosplit : 1;    /* Split line automatically ? */
    ULONG autoindent : 1;   /* Autoindent cursor after <RETURN> ? */
    ULONG autounblock : 1;  /* Automatic Unblock on new BLOCK-command ? */
};

#define CONFIG0003	    "XDMECNFG0003"
#define CONFIG0003_SIZE     sizeof(struct Config0003)

struct Config0004
{
    Column wwcol;	    /*	word wrap column		    */

    UWORD winx; 	    /*	save state of non-icon window	    */
    UWORD winy;
    UWORD winwidth;
    UWORD winheight;

    UWORD iwinx, iwiny;     /*	save state of icon window	    */

    UWORD aslleft;	    /* pos of FileReq */
    UWORD asltop;
    UWORD aslwidth;
    UWORD aslheight;

    Column tabstop;	    /* size of tabs */
    Column margin;

    UBYTE fgpen;	    /* Pens */
    UBYTE bgpen;
    UBYTE hgpen;
    UBYTE bbpen;
    UBYTE tfpen;
    UBYTE tbpen;

    ULONG _insertmode  : 1;  /* insert or overwrite */
    ULONG _ignorecase  : 1;  /* should SEARCH ignore the case ? */
    ULONG _wordwrap    : 1;  /* do wordwrap ? */
    ULONG _autosplit   : 1;  /* Split line automatically ? */
    ULONG _autoindent  : 1;  /* Autoindent cursor after <RETURN> ? */

    ULONG _autounblock : 1;  /* Automatic Unblock on new BLOCK-command ? */
    ULONG _doback      : 1;  /* create backup on save */
    ULONG _nicepaging  : 1;  /* don't do paging as the styleguide says */
    ULONG _iconactive  : 1;  /* should the iconified window be activated ? */
    ULONG _blockendsfloat : 1; /* should bstart & bend sort themselves ? */

/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
!!!!!!!!!!!!! DON'T CHANGE ANYTHING ABOVE HERE !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

    If you need to add a new flag, count dummy_bf down by one and insert it
    before dummy_bf. If you need something else, count down dummy_pad and
    insert it before dummy_pad. */

    ULONG dummy_bf    : 22; /* more flags here (default: 0) */
    UBYTE dummy_pad[76];    /* more space for more stuff */
};

#define CONFIG0004	    "XDMECNFG0004"
#define CONFIG0004_SIZE     sizeof(struct Config0004)

#define Config		    Config0004
#define CONFIG_VERSION	    CONFIG0004

#endif /* CONFIG_H */
