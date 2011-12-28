/*			MIME Message Parse			HTMIME.c
**			==================
**
**	This is RFC 1341-specific code.
**	The input stream pushed into this parser is assumed to be
**	stripped on CRs, ie lines end with LF, not CR LF.
**	(It is easy to change this except for the body part where
**	conversion can be slow.)
**
** History:
**	   Feb 92	Written Tim Berners-Lee, CERN
**
*/
#include "HTMIME.h"             /* Implemented here */
#include "HTAlert.h"
#include "HTFile.h"
#include "tcp.h"

/* This is UGLY. */
char *redirecting_url = NULL;

/* This is almost as ugly. */
extern int loading_length;

/* #define TRACE 1 */

/*		MIME Object
**		-----------
*/
typedef enum _MIME_state
{
  BEGINNING_OF_LINE,
  CONTENT_,
  CONTENT_T,
  CONTENT_TRANSFER_ENCODING,
  CONTENT_TYPE,
  CONTENT_ENCODING,
  CONTENT_LENGTH,
  LOCATION,
  SKIP_GET_VALUE,		/* Skip space then get value */
  GET_VALUE,			/* Get value till white space */
  JUNK_LINE,			/* Ignore the rest of this folded line */
  NEWLINE,			/* Just found a LF .. maybe continuation */
  CHECK,			/* check against check_pointer */
  MIME_TRANSPARENT,		/* put straight through to target ASAP! */
  MIME_IGNORE,			/* ignore entire file */
  /* TRANSPARENT and IGNORE are defined as stg else in _WINDOWS */
} MIME_state;

#define VALUE_SIZE 256		/* @@@@@@@ Arbitrary? */
struct _HTStream
{
  CONST HTStreamClass * isa;

  MIME_state		state;		/* current state */
  MIME_state		if_ok;		/* got this state if match */
  MIME_state		field;		/* remember which field */
  MIME_state		fold_state;	/* state on a fold */
  CONST char *		check_pointer;	/* checking input */

  char *		value_pointer;	/* storing values */
  char			value[VALUE_SIZE];

  HTParentAnchor *	anchor; 	/* Given on creation */
  HTStream *		sink;		/* Given on creation */

  char *		boundary;	/* For multipart */

  HTFormat		encoding;	/* Content-Transfer-Encoding */
  char *		compression_encoding;
  int			content_length;
  HTFormat		format; 	/* Content-Type */
  HTStream *		target; 	/* While writing out */
  HTStreamClass 	targetClass;

  HTAtom *		targetRep;	/* Converting into? */

  char *		location;
  int interrupted;
};


/*_________________________________________________________________________
**
**			A C T I O N	R O U T I N E S
*/

/*	Character handling
**	------------------
**
**	This is a FSM parser which is tolerant as it can be of all
**	syntax errors.	It ignores field names it does not understand,
**	and resynchronises on line beginnings.
*/

PRIVATE void HTMIME_put_character ARGS2(HTStream *, me, char, c)
{
  switch(me->state)
    {
    case MIME_IGNORE:
      if (TRACE)
	fprintf (stderr, "[HTMIME_put_character] Got MIME_IGNORE; returning...\n");
      return;

    case MIME_TRANSPARENT:
      (*me->targetClass.put_character)(me->target, c);	/* MUST BE FAST */
      return;

    case NEWLINE:
      if (c != '\n' && WHITE(c))
	{
	  /* Folded line */
	  me->state = me->fold_state;	/* pop state before newline */
	  break;
	}
      /* else Falls through */

    case BEGINNING_OF_LINE:
      switch(c)
	{
	case 'c':
	case 'C':
	  me->check_pointer = "ontent-";
	  me->if_ok = CONTENT_;
	  me->state = CHECK;
	  if (TRACE)
	    fprintf (stderr,
		     "[MIME] Got C at beginning of line; checking for 'ontent-'\n");
	  break;
	case 'l':
	case 'L':
	  me->check_pointer = "ocation:";
	  me->if_ok = LOCATION;
	  me->state = CHECK;
	  if (TRACE)
	    fprintf (stderr,
		     "[MIME] Got L at beginning of line\n");
	  break;
	case '\n':                      /* Blank line: End of Header! */
	  {
	    int compressed = COMPRESSED_NOT;
	    if (TRACE)
	      fprintf (stderr,
		       "HTMIME: DOING STREAMSTACK: MIME content type is %s, converting to %s\n",
		       HTAtom_name(me->format), HTAtom_name(me->targetRep));
	    if (TRACE)
	      fprintf (stderr,
		       "                           Compression encoding '%s'\n",
		       me->compression_encoding ? me->compression_encoding : "<none>");
	    if (me->compression_encoding)
	      {
		if (strcmp (me->compression_encoding, "x-compress") == 0)
		  {
		    compressed = COMPRESSED_BIGZ;
		  }
		else if (strcmp (me->compression_encoding, "x-gzip") == 0)
		  {
		    compressed = COMPRESSED_GNUZIP;
		  }
		else
		  {
		    if (TRACE)
		      fprintf (stderr, "HTMIME: Unknown compression_encoding '%s'\n",
			       me->compression_encoding);
		  }
	      }

	    if (TRACE)
	      fprintf (stderr, "HTMIME: compressed == %d\n", compressed);
	    me->target = HTStreamStack(me->format, me->targetRep, compressed,
				       me->sink, me->anchor);
	    if (!me->target)
	      {
		if (TRACE)
		  {
		    fprintf(stderr, "MIME: Can't translate! ** \n");
		    fprintf(stderr, "HTMIME: Defaulting to HTML.\n");
		  }
		/* Default to HTML. */
		me->target = HTStreamStack(HTAtom_for("text/html"),
					   me->targetRep,
					   compressed,
					   me->sink,
					   me->anchor);
	      }
	    if (me->target)
	      {
		me->targetClass = *me->target->isa;
		/* Check for encoding and select state from there @@ */
		/* From now push straigh through */
		if (TRACE)
		  fprintf (stderr, "[MIME] Entering MIME_TRANSPARENT\n");
		me->state = MIME_TRANSPARENT;
	      }
	    else
	      {
		/* This is HIGHLY EVIL -- the browser WILL BREAK
		   if it ever reaches here.  Thus the default to
		   HTML above, which should always happen... */
		if (TRACE)
		  fprintf (stderr, "MIME: HIT HIGHLY EVIL!!! ***\n");
		me->state = MIME_IGNORE;		/* What else to do? */
	      }
	  }
	  break;

	default:
	  if (TRACE)
	    fprintf (stderr, "[MIME] Got nothing at beginning of line; bleah.\n");
	  goto bad_field_name;
	  break;

	} /* switch on character */
      break;

    case CHECK: 			/* Check against string */
      if (TOLOWER(c) == *(me->check_pointer)++)
	{
	  if (!*me->check_pointer)
	    me->state = me->if_ok;
	}
      else
	{		/* Error */
	  if (TRACE)
	    fprintf(stderr,
		    "HTMIME: Bad character `%c' found where `%s' expected\n",
		    c, me->check_pointer - 1);
	  goto bad_field_name;
	}
      break;

    case CONTENT_:
      if (TRACE)
	fprintf (stderr,
		 "[MIME] in case CONTENT_\n");
      switch(c)
	{
	case 't':
	case 'T':
	  me->state = CONTENT_T;
	  if (TRACE)
	    fprintf (stderr,
		     "[MIME] Was CONTENT_, found T, state now CONTENT_T\n");
	  break;

	case 'e':
	case 'E':
	  me->check_pointer = "ncoding:";
	  me->if_ok = CONTENT_ENCODING;
	  me->state = CHECK;
	  if (TRACE)
	    fprintf (stderr,
		     "[MIME] Was CONTENT_, found E, checking for 'ncoding:'\n");
	  break;

	case 'l':
	case 'L':
	  me->check_pointer = "ength:";
	  me->if_ok = CONTENT_LENGTH;
	  me->state = CHECK;
	  if (TRACE)
	    fprintf (stderr,
		     "[MIME] Was CONTENT_, found L, checking for 'ength:'\n");
	  break;

	default:
	  if (TRACE)
	    fprintf (stderr,
		     "[MIME] Was CONTENT_, found nothing; bleah\n");
	  goto bad_field_name;

	} /* switch on character */
      break;

    case CONTENT_T:
      if (TRACE)
	fprintf (stderr,
		 "[MIME] in case CONTENT_T\n");
      switch(c)
	{
	case 'r':
	case 'R':
	  me->check_pointer = "ansfer-encoding:";
	  me->if_ok = CONTENT_TRANSFER_ENCODING;
	  me->state = CHECK;
	  if (TRACE)
	    fprintf (stderr,
		     "[MIME] Was CONTENT_T; going to check for ansfer-encoding:\n");
	  break;

	case 'y':
	case 'Y':
	  me->check_pointer = "pe:";
	  me->if_ok = CONTENT_TYPE;
	  me->state = CHECK;
	  if (TRACE)
	    fprintf (stderr, "[MIME] Was CONTENT_T; going to check for pe:\n");
	  break;

	default:
	  if (TRACE)
	    fprintf (stderr,
		     "[MIME] Was CONTENT_T; found nothing; bleah\n");
	  goto bad_field_name;
	} /* switch on character */
      break;

    case CONTENT_TYPE:
    case CONTENT_TRANSFER_ENCODING:
    case CONTENT_ENCODING:
    case CONTENT_LENGTH:
      me->field = me->state;		/* remember it */
      me->state = SKIP_GET_VALUE;
      /* Fall through! */
      goto fall_through;
    case LOCATION:
      me->field = me->state;
      me->state = SKIP_GET_VALUE;
      /* Fall through! */
    fall_through:
    case SKIP_GET_VALUE:
      if (c == '\n')
	{
	  me->fold_state = me->state;
	  me->state = NEWLINE;
	  break;
	}
      if (WHITE(c))
	break;	/* Skip white space */

      me->value_pointer = me->value;
      me->state = GET_VALUE;
      /* Fall through to store first character */

    case GET_VALUE:
      if (WHITE(c))
	{
	  /* End of field */
	  *me->value_pointer = 0;
	  switch (me->field)
	    {
	    case CONTENT_TYPE:
	      if (TRACE)
		fprintf (stderr, "[MIME_put_char] Got content-type value '%s'\n", me->value);
	      /* Lowercase it. */
	      {
		char *tmp;
		for (tmp = me->value; *tmp; tmp++)
		  *tmp = TOLOWER (*tmp);
	      }
	      if (TRACE)
		fprintf (stderr, "[MIME_put_char] Lowercased to '%s'\n", me->value);
	      me->format = HTAtom_for(me->value);
	      if (TRACE)
		fprintf (stderr, "[MIME_put_char] Got content-type value atom '%s'\n",
			 me->format->name);
	      break;
	    case CONTENT_TRANSFER_ENCODING:
	      me->encoding = HTAtom_for(me->value);
	      if (TRACE)
		fprintf (stderr,
			 "[MIME_put_char] Picked up transfer_encoding '%s'\n",
			 me->encoding->name);
	      break;
	    case CONTENT_ENCODING:
	      me->compression_encoding = strdup (me->value);
	      if (TRACE)
		fprintf (stderr,
			 "[MIME_put_char] Picked up compression encoding '%s'\n",
			 me->compression_encoding);
	      break;
	    case CONTENT_LENGTH:
	      me->content_length = atoi (me->value);
	      /* This is TEMPORARY. */
	      loading_length = me->content_length;
	      if (TRACE)
		fprintf (stderr,
			 "[MIME_put_char] Picked up content length '%d'\n",
			 me->content_length);
	      break;
	    case LOCATION:
	      me->location = me->value;
	      redirecting_url = strdup (me->location);
	      if (TRACE)
		fprintf
		  (stderr,
		   "[MIME_put_char] Picked up location '%s'\n", me->location);
	      break;
	    default:		/* Should never get here */
	      break;
	    }
	}
      else
	{
	  if (me->value_pointer < me->value + VALUE_SIZE - 1)
	    {
	      *me->value_pointer++ = c;
	      break;
	    }
	  else
	    {
	      goto value_too_long;
	    }
	}
      /* Fall through */

    case JUNK_LINE:
      if (c == '\n')
	{
	  me->state = NEWLINE;
	  me->fold_state = me->state;
	}
      break;

    } /* switch on state*/

  return;

 value_too_long:
  if (TRACE) fprintf(stderr,
		     "HTMIME: *** Syntax error. (string too long)\n");

 bad_field_name:				/* Ignore it */
  me->state = JUNK_LINE;
  return;
}



/*	String handling
**	---------------
**
**	Strings must be smaller than this buffer size.
*/
PRIVATE void HTMIME_put_string ARGS2(HTStream *, me, CONST char*, s)
{
  CONST char * p;
  if (TRACE)
    fprintf (stderr, "[HTMIME_put_string] Putting '%s'\n", s);
  if (me->state == MIME_TRANSPARENT)		/* Optimisation */
    {
      if (TRACE)
	fprintf (stderr, "[HTMIME_put_string] Doing transparent put_string\n");
      (*me->targetClass.put_string)(me->target,s);
    }
  else if (me->state != MIME_IGNORE)
    {
      if (TRACE)
	fprintf (stderr, "[HTMIME_put_string] Doing char-by-char put_character\n");
      for (p=s; *p; p++)
	HTMIME_put_character(me, *p);
    }
  else
    {
      if (TRACE)
	fprintf (stderr, "[HTMIME_put_string] DOING NOTHING!\n");
    }
  return;
}


/*	Buffer write.  Buffers can (and should!) be big.
**	------------
*/
PRIVATE void HTMIME_write ARGS3(HTStream *, me, CONST char*, s, int, l)
{
  CONST char * p;
  if (TRACE)
    fprintf (stderr, "[HTMIME_write] Putting %d bytes\n", l);
  if (me->state == MIME_TRANSPARENT)		/* Optimisation */
    {
      if (TRACE)
	fprintf (stderr, "[HTMIME_write] Doing transparent put_block\n");
      (*me->targetClass.put_block)(me->target, s, l);
    }
  else if (me->state != MIME_IGNORE)
    {
      if (TRACE)
	fprintf (stderr, "[HTMIME_write] Doing char-by-char put_character\n");
      for (p=s; p < s+l; p++)
	HTMIME_put_character(me, *p);
    }
  else
    {
      if (TRACE)
	fprintf (stderr, "[HTMIME_write] DOING NOTHING!\n");
    }
  return;
}




/*	Free an HTML object
**	-------------------
**
*/
PRIVATE void HTMIME_free ARGS1(HTStream *, me)
{
  if (!me->target)
    {
      if (TRACE)
	fprintf (stderr, "[HTMIME_free] Caught case where we didn't get a target.\n");
      if (TRACE)
	fprintf (stderr, "  me %p, me->target %p\n", me, me->target);
      me->format = HTAtom_for ("text/html");
      me->target = HTStreamStack(me->format, me->targetRep, 0,
				 me->sink, me->anchor);
      if (TRACE)
	fprintf (stderr, "  me->target->isa %p\n", me->target->isa);
      if (TRACE)
	fprintf (stderr, "  *me->target->isa '%s'\n", me->target->isa->name);
      me->targetClass = *me->target->isa;
      (*me->targetClass.put_string) (me->target, "<H1>ERROR IN HTTP/1.0 RESPONSE</H1> The remote server returned a HTTP/1.0 response that Mosaic's MIME parser could not understand.  Please contact the server maintainer.<P> Sorry for the inconvenience,"
"<P><ADDRESS>The Management</ADDRESS>");
    }
  if (me->target)
    (*me->targetClass.free)(me->target);

  free(me);

  return;
}

/*	End writing
*/

PRIVATE void HTMIME_end_document ARGS1(HTStream *, me)
{
  if (me->target)
    (*me->targetClass.end_document)(me->target);
}

PRIVATE void HTMIME_handle_interrupt ARGS1(HTStream *, me)
{
  me->interrupted = 1;

  /* Propagate interrupt message down. */
  if (me->target)
    (*me->targetClass.handle_interrupt)(me->target);

  return;
}



/*	Structured Object Class
**	-----------------------
*/
PUBLIC CONST HTStreamClass HTMIME =
{
  "MIMEParser",
  HTMIME_free,
  HTMIME_end_document,
  HTMIME_put_character, 	HTMIME_put_string,
  HTMIME_write,
  HTMIME_handle_interrupt
  };


/*	Subclass-specific Methods
**	-------------------------
*/

PUBLIC HTStream* HTMIMEConvert ARGS5(
	HTPresentation *,	pres,
	HTParentAnchor *,	anchor,
	HTStream *,		sink,
	HTFormat,		format_in,
	int,			compressed)
{
    HTStream* me;

    me = malloc(sizeof(*me));
    me->isa = &HTMIME;

    if (TRACE)
      fprintf (stderr, "[HTMIMEConvert] HELLO!\n");

    me->sink = sink;
    me->anchor = anchor;
    me->target = NULL;
    me->state = BEGINNING_OF_LINE;
    me->format = WWW_PLAINTEXT;
    me->targetRep = pres->rep_out;
    me->boundary = 0;		/* Not set yet */
    me->location = 0;
    me->interrupted = 0;
    me->encoding = 0;
    me->compression_encoding = 0;
    me->content_length = -1;
    return me;
}
