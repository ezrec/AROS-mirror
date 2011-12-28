/*		Access Manager					HTAccess.c
**		==============
**
** Authors
**	TBL	Tim Berners-Lee timbl@info.cern.ch
**	JFG	Jean-Francois Groff jfg@dxcern.cern.ch
**	DD	Denis DeLaRoca (310) 825-4580  <CSP1DWD@mvs.oac.ucla.edu>
** History
**	 8 Jun 92 Telnet hopping prohibited as telnet is not secure TBL
**	26 Jun 92 When over DECnet, suppressed FTP, Gopher and News. JFG
**	 6 Oct 92 Moved HTClientHost and logfile into here. TBL
**	17 Dec 92 Tn3270 added, bug fix. DD
**	 4 Feb 93 Access registration, Search escapes bad chars TBL
**		  PARAMETERS TO HTSEARCH AND HTLOADRELATIVE CHANGED
**	28 May 93 WAIS gateway explicit if no WAIS library linked in.
**
** Bugs
**	This module assumes that that the graphic object is hypertext, as it
**	needs to select it when it has been loaded.  A superclass needs to be
**	defined which accepts select and select_anchor.
*/

#ifndef DEFAULT_WAIS_GATEWAY
#define DEFAULT_WAIS_GATEWAY "http://www.ncsa.uiuc.edu:8001/"
#endif

/* Implements:
*/
#include "HTAccess.h"

/* Uses:
*/

#include "HTParse.h"
#include "HTUtils.h"
#include "HTML.h"               /* SCW */

#include <stdio.h>

#include "HTList.h"
#include "HText.h"      /* See bugs above */
#include "HTAlert.h"

/* #define TRACE 1 */


#if !defined(_AMIGA) && !defined(__AROS__)
 /* in the mean time, looks ar X resources, may want to make
    it look in prefs file later */
extern char *mo_check_for_proxy (char *);
#endif



/*	These flags may be set to modify the operation of this module
*/
PUBLIC char * HTClientHost = 0; /* Name of remote login host if any */

/*	To generate other things, play with these:
*/

PUBLIC HTFormat HTOutputFormat = NULL;
PUBLIC HTStream* HTOutputStream = NULL; /* For non-interactive, set this */

PUBLIC BOOL using_gateway = NO; /* are we using a gateway? */
PUBLIC BOOL using_proxy = NO; /* are we using a proxy gateway? */


PRIVATE HTList * protocols = NULL;   /* List of registered protocol descriptors */


/*	Register a Protocol				HTRegisterProtocol
**	-------------------
*/

PUBLIC BOOL HTRegisterProtocol(protocol)
	HTProtocol * protocol;
{
    if (!protocols) protocols = HTList_new();
    HTList_addObject(protocols, protocol);
    return YES;
}


/*	Register all known protocols
**	----------------------------
**
**	Add to or subtract from this list if you add or remove protocol modules.
**	This routine is called the first time the protocol list is needed,
**	unless any protocols are already registered, in which case it is not called.
**	Therefore the application can override this list.
*/
PRIVATE void HTAccessInit NOARGS			/* Call me once */
{
extern HTProtocol HTTP, HTFile, HTTelnet, HTTn3270, HTRlogin;
extern HTProtocol HTFTP, HTNews, HTGopher, HTMail;
#ifdef DIRECT_WAIS
extern HTProtocol HTWAIS;
#endif
    HTRegisterProtocol(&HTFTP);
    HTRegisterProtocol(&HTNews);
    HTRegisterProtocol(&HTGopher);
    HTRegisterProtocol(&HTMail);
#ifdef DIRECT_WAIS
    HTRegisterProtocol(&HTWAIS);
#endif

    HTRegisterProtocol(&HTTP);
    HTRegisterProtocol(&HTFile);
    HTRegisterProtocol(&HTTelnet);
    HTRegisterProtocol(&HTTn3270);
    HTRegisterProtocol(&HTRlogin);
}


/*		Find physical name and access protocol
**		--------------------------------------
**
**
** On entry,
**	addr		must point to the fully qualified hypertext reference.
**	anchor		a pareent anchor with whose address is addr
**
** On exit,
**	returns 	HT_NO_ACCESS		Error has occured.
**			HT_OK			Success
**
*/
PRIVATE int get_physical ARGS2(
	CONST char *, 	addr,
	HTParentAnchor *,	anchor)
{
    char * access=0;	/* Name of access method */

    HTAnchor_setPhysical(anchor, addr);

    access =  HTParse(HTAnchor_physical(anchor),
		"file:", PARSE_ACCESS);

/*	Check whether gateway access has been set up for this
*/
#define USE_GATEWAYS
#ifdef USE_GATEWAYS
    {
	char *gateway_parameter, *gateway, *proxy;

	/* search for gateways */
	gateway_parameter = (char *)malloc(strlen(access)+20);
	if (gateway_parameter == NULL) outofmem(__FILE__, "HTLoad");
	strcpy(gateway_parameter, "WWW_");
	strcat(gateway_parameter, access);
	strcat(gateway_parameter, "_GATEWAY");
	gateway = (char *)getenv(gateway_parameter); /* coerce for decstation */

	/* search for proxy servers */
	strcpy(gateway_parameter, access);
	strcat(gateway_parameter, "_proxy");
	proxy = (char *)getenv(gateway_parameter);
	free(gateway_parameter);

	/*
	 * The environment variables have precedence over the X resources,
	 * But if we got no proxy from them, check the X resources now.
	 */
#if !defined(_AMIGA) && !defined(__AROS__)
/* see above */
	if ((proxy == NULL)||(proxy[0] == '\0'))
	{
		proxy = mo_check_for_proxy(access);
	}
#endif

#ifndef DIRECT_WAIS
	if (!gateway && 0==strcmp(access, "wais")) {
	    gateway = DEFAULT_WAIS_GATEWAY;
	}
#endif

	/* make sure the using_proxy variable is false */
	using_proxy = NO;

	/* proxy servers have precedence over gateway servers */
	if (proxy) {
		char * gatewayed;

		gatewayed = NULL;
		StrAllocCopy(gatewayed,proxy);
		StrAllocCat(gatewayed,addr);
		using_proxy = YES;
		HTAnchor_setPhysical(anchor, gatewayed);
		free(gatewayed);
		free(access);

		access =  HTParse(HTAnchor_physical(anchor),
			"http:", PARSE_ACCESS);
	} else if (gateway) {
		char * gatewayed;

		gatewayed = NULL;
		StrAllocCopy(gatewayed,gateway);
		StrAllocCat(gatewayed,addr);
		using_gateway = YES;
		HTAnchor_setPhysical(anchor, gatewayed);
		free(gatewayed);
		free(access);

		access =  HTParse(HTAnchor_physical(anchor),
			"http:", PARSE_ACCESS);
	} else {
		using_proxy = NO;
		using_gateway = NO;
	}
    }
#endif



/*	Search registered protocols to find suitable one
*/
    {
	int i, n;
	if (!protocols) HTAccessInit();
	n = HTList_count(protocols);
	for (i=0; i<n; i++) {
	    HTProtocol *p = HTList_objectAt(protocols, i);
	    if (strcmp(p->name, access)==0) {
		HTAnchor_setProtocol(anchor, p);
		free(access);
		return (HT_OK);
	    }
	}
    }

    free(access);
    return HT_NO_ACCESS;
}


/*		Load a document
**		---------------
**
**	This is an internal routine, which has an address AND a matching
**	anchor.  (The public routines are called with one OR the other.)
**
** On entry,
**	addr		must point to the fully qualified hypertext reference.
**	anchor		a pareent anchor with whose address is addr
**
** On exit,
**	returns 	<0		Error has occured.
**			HT_LOADED	Success
**			HT_NO_DATA	Success, but no document loaded.
**					(telnet sesssion started etc)
**
*/
PRIVATE int HTLoad ARGS4(
	CONST char *,		addr,
	HTParentAnchor *,	anchor,
	HTFormat,		format_out,
	HTStream *,		sink)
{
    HTProtocol* p;
    int status = get_physical(addr, anchor);
    if (status < 0) return status;	/* Can't resolve or forbidden */

    p = HTAnchor_protocol(anchor);
    return (*(p->load))(HTAnchor_physical(anchor),
			anchor, format_out, sink);
}


/*		Get a save stream for a document
**		--------------------------------
*/
PUBLIC HTStream *HTSaveStream ARGS1(HTParentAnchor *, anchor)
{
    HTProtocol * p = HTAnchor_protocol(anchor);
    if (!p) return NULL;

    return (*p->saveStream)(anchor);

}


/*		Load a document - with logging etc
**		----------------------------------
**
**	- Checks or documents already loaded
**	- Logs the access
**	- Allows stdin filter option
**	- Trace ouput and error messages
**
**    On Entry,
**	  anchor	    is the node_anchor for the document
**	  full_address	    The address of the document to be accessed.
**	  filter	    if YES, treat stdin as HTML
**
**    On Exit,
**	  returns    1	   Success in opening document
**		     0	   Failure
**		     -1    Interrupted
**
*/

/* This is exported all the way to gui-documents.c at the moment,
   to tell mo_load_window_text when to use a redirected URL instead. */
CONST char *use_this_url_instead;

PRIVATE int HTLoadDocument ARGS4(
	CONST char *,		full_address,
	HTParentAnchor *,	anchor,
	HTFormat,		format_out,
	HTStream*,		sink)
{
    int 	status;

    use_this_url_instead = NULL;

    /* We LOVE goto's! */
  try_again:
    if (TRACE) fprintf (stderr,
      "HTAccess: loading document %s\n", full_address);

    status = HTLoad(full_address, anchor, format_out, sink);

    if (status == HT_LOADED) {
	if (TRACE) {
	    fprintf(stderr, "HTAccess: `%s' has been accessed.\n",
	    full_address);
	}
	return 1;
    }

    if (status == HT_REDIRECTING)
      {
	/* Exported from HTMIME.c, of all places. */
	extern char *redirecting_url;
	if (TRACE)
	  {
	    fprintf (stderr, "HTAccess: '%s' is a redirection URL.\n",
		     full_address);
	    fprintf (stderr, "HTAccess: Redirecting to '%s'\n",
		     redirecting_url);
	  }
	full_address = redirecting_url;
	use_this_url_instead = full_address;
	goto try_again;
      }

    if (status == HT_INTERRUPTED)
      {
	if (TRACE)
	  fprintf (stderr,
		   "HTAccess: We were interrupted.\n");
	return -1;
      }

    if (status == HT_NO_DATA) {
	if (TRACE) {
	    fprintf(stderr,
	    "HTAccess: `%s' has been accessed, No data left.\n",
	    full_address);
	}
	return 0;
    }

    if (status<0) {		      /* Failure in accessing a document */
	if (TRACE) fprintf(stderr,
			   "HTAccess: Can't access `%s'\n", full_address);
	return 0;
    }

    /* If you get this, then please find which routine is returning
       a positive unrecognised error code! */

    if (TRACE)
      fprintf(stderr,
	      "**** HTAccess: socket or file number %d returned by obsolete load routine!\n", status);
    return 0;

} /* HTLoadDocument */



/*		Load a document from absolute name
**		---------------
**
**    On Entry,
**	  addr	   The absolute address of the document to be accessed.
**	  filter   if YES, treat document as HTML
**
**    On Exit,
**	  returns    1	   Success in opening document
**		     0	    Failure
**		     -1      Interrupted
**
**
*/

PUBLIC int HTLoadAbsolute ARGS1(CONST char *,addr)
{
   return HTLoadDocument( addr,
		HTAnchor_parent(HTAnchor_findAddress(addr)),
			HTOutputFormat ? HTOutputFormat : WWW_PRESENT,
			HTOutputStream);
}


/*		Load a document from absolute name to stream
**		--------------------------------------------
**
**    On Entry,
**	  addr	   The absolute address of the document to be accessed.
**	  sink	   if non-NULL, send data down this stream
**
**    On Exit,
**	  returns    YES     Success in opening document
**		     NO      Failure
**
**
*/

PUBLIC BOOL HTLoadToStream ARGS3(
		CONST char *,	addr,
		BOOL,		filter,
		HTStream *,	sink)
{
   return HTLoadDocument(addr,
		HTAnchor_parent(HTAnchor_findAddress(addr)),
			HTOutputFormat ? HTOutputFormat : WWW_PRESENT,
			sink);
}




/*		Load a document from relative name
**		---------------
**
**    On Entry,
**	  relative_name     The relative address of the document
**			    to be accessed.
**
**    On Exit,
**	  returns    YES     Success in opening document
**		     NO      Failure
**
**
*/

PUBLIC BOOL HTLoadRelative ARGS2(
		CONST char *,		relative_name,
		HTParentAnchor *,	here)
{
    char *		full_address = 0;
    BOOL		result;
    char *		mycopy = 0;
    char *		stripped = 0;
    char *		current_address =
				HTAnchor_address((HTAnchor*)here);

    StrAllocCopy(mycopy, relative_name);

    stripped = HTStrip(mycopy);
    full_address = HTParse(stripped,
		   current_address,
		   PARSE_ACCESS|PARSE_HOST|PARSE_PATH|PARSE_PUNCTUATION);
    result = HTLoadAbsolute(full_address);
    free(full_address);
    free(current_address);
    free(mycopy);
    return result;
}

