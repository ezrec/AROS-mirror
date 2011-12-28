/*	HyperText Tranfer Protocol	- Client implementation		HTTP.c
**	==========================
*/

#include "HTTP.h"

#define HTTP_VERSION	"HTTP/1.0"

#define INIT_LINE_SIZE		1024	/* Start with line buffer this big */
#define LINE_EXTEND_THRESH	256	/* Minimum read size */
#define VERSION_LENGTH 		20	/* for returned protocol version */

#include "HTParse.h"
#include "HTUtils.h"
#include "tcp.h"
#include "HTTCP.h"
#include "HTFormat.h"
#include "HTFile.h"
#include <ctype.h>
#include "HTAlert.h"
#include "HTMIME.h"
#include "HTML.h"
#include "HTInit.h"
#include "HTAABrow.h"

/* #define TRACE 1 */

struct _HTStream 
{
  HTStreamClass * isa;
};

extern char * HTAppName;	/* Application name: please supply */
extern char * HTAppVersion;	/* Application version: please supply */

/* Variables that control whether we do a POST or a GET,
   and if a POST, what and how we POST. */
int do_post = 0;
char *post_content_type = NULL;
char *post_data = NULL;
extern BOOL using_gateway;    /* are we using an HTTP gateway? */
extern BOOL using_proxy;      /* are we using an HTTP proxy gateway? */


/*		Load Document from HTTP Server			HTLoadHTTP()
**		==============================
**
**	Given a hypertext address, this routine loads a document.
**
**
** On entry,
**	arg	is the hypertext reference of the article to be loaded.
**
** On exit,
**	returns	>=0	If no error, a good socket number
**		<0	Error.
**
**	The socket must be closed by the caller after the document has been
**	read.
**
*/
PUBLIC int HTLoadHTTP ARGS4 (
	CONST char *, 		arg,
	HTParentAnchor *,	anAnchor,
	HTFormat,		format_out,
	HTStream*,		sink)
{
  int s;				/* Socket number for returned data */
  char *command;			/* The whole command */
  char *eol;			/* End of line if found */
  char *start_of_data;		/* Start of body of reply */
  int status;				/* tcp return */
  int bytes_already_read;
  char crlf[3];			/* A CR LF equivalent string */
  HTStream *target;		/* Unconverted data */
  HTFormat format_in;			/* Format arriving in the message */
  
  char *line_buffer;
  char *line_kept_clean;
  BOOL extensions;		/* Assume good HTTP server */
  int compressed;
  char line[256];
  
  int length, doing_redirect, rv;
  int already_retrying = 0;
  int return_nothing;

#ifdef PEM_AUTH
  BOOL doing_pem;
  char *body;
  int body_size;
#endif /* PEM_AUTH */
  
  if (!arg)
    {
      status = -3;
      HTProgress ("Bad request.");
      goto done;
    }
  if (!*arg) 
    {
      status = -2;
      HTProgress ("Bad request.");
      goto done;
    }
  
  sprintf(crlf, "%c%c", CR, LF);

  /* At this point, we're talking HTTP/1.0. */
  extensions = YES;

 try_again:
  /* All initializations are moved down here from up above,
     so we can start over here... */
  eol = 0;
  bytes_already_read = 0;
  length = 0;
  doing_redirect = 0;
  compressed = 0;
  target = NULL;
  line_buffer = NULL;
  line_kept_clean = NULL;
  return_nothing = 0;
#ifdef PEM_AUTH
  doing_pem = NO;
#endif /* PEM_AUTH */

  status = HTDoConnect (arg, "HTTP", TCP_PORT, &s);
  if (status == HT_INTERRUPTED)
    {
      /* Interrupt cleanly. */
      if (TRACE)
        fprintf (stderr,
                 "HTTP: Interrupted on connect; recovering cleanly.\n");
      HTProgress ("Connection interrupted.");
      /* status already == HT_INTERRUPTED */
      goto done;
    }
  if (status < 0) 
    {
      if (TRACE) 
        fprintf(stderr, 
                "HTTP: Unable to connect to remote host for `%s' (errno = %d).\n", arg, errno);
      HTProgress ("Unable to connect to remote host.");
      status = HT_NO_DATA;
      goto done;
    }
  
  /*	Ask that node for the document,
   **	omitting the host name & anchor
   */        
  {
    char * p1 = HTParse(arg, "", PARSE_PATH|PARSE_PUNCTUATION);
    command = malloc(5 + strlen(p1)+ 2 + 31);

    if (do_post)
      strcpy(command, "POST ");
    else
      strcpy(command, "GET ");

    /*
     * For a gateway, the beginning '/' on the request must
     * be stripped before appending to the gateway address.
     */
    if ((using_gateway)||(using_proxy))
        strcat(command, p1+1);
    else
        strcat(command, p1);
    free(p1);
  }
  if (extensions) 
    {
      strcat(command, " ");
      strcat(command, HTTP_VERSION);
    }
  
  strcat(command, crlf);	/* CR LF, as in rfc 977 */
  
  if (extensions) 
    {
#if	0
      int n, i;
      
      if (!HTPresentations) HTFormatInit();
      n = HTList_count(HTPresentations);
      
      for(i=0; i<n; i++) 
        {
          HTPresentation * pres = HTList_objectAt(HTPresentations, i);
          if (pres->rep_out == WWW_PRESENT) 
            {
              sprintf(line, "Accept: %s%c%c",
                      HTAtom_name(pres->rep), CR, LF);
              StrAllocCat(command, line);
            }
        }
      
#endif
      sprintf(line, "User-Agent:  %s/%s  libwww/%s%c%c",
              HTAppName ? HTAppName : "unknown",
              HTAppVersion ? HTAppVersion : "0.0",
              HTLibraryVersion, CR, LF);
      StrAllocCat(command, line);
      
      {
        char *docname;
        char *hostname;
        char *colon;
        int portnumber;
        char *auth;
        
        docname = HTParse(arg, "", PARSE_PATH);
        hostname = HTParse(arg, "", PARSE_HOST);
        if (hostname &&
            NULL != (colon = strchr(hostname, ':'))) 
          {
            *(colon++) = '\0';	/* Chop off port number */
            portnumber = atoi(colon);
          }
        else portnumber = 80;
        
        if (NULL!=(auth=HTAA_composeAuth(hostname, portnumber, docname))) 
          {
            sprintf(line, "%s%c%c", auth, CR, LF);
            StrAllocCat(command, line);
          }
        if (TRACE) 
          {
            if (auth)
              fprintf(stderr, "HTTP: Sending authorization: %s\n", auth);
            else
              fprintf(stderr, "HTTP: Not sending authorization (yet)\n");
          }
        FREE(hostname);
        FREE(docname);
      }
    }

  if (do_post)
    {
      if (TRACE)
        fprintf (stderr, "HTTP: Doing post, content-type '%s'\n",
                 post_content_type);
      sprintf (line, "Content-type: %s%c%c",
               post_content_type ? post_content_type : "lose", CR, LF);
      StrAllocCat(command, line);
      {
        int content_length;
        if (!post_data)
          content_length = 4; /* 4 == "lose" :-) */
        else
          content_length = strlen (post_data);
        sprintf (line, "Content-length: %d%c%c",
                 content_length, CR, LF);
        StrAllocCat(command, line);
      }
      
      StrAllocCat(command, crlf);	/* Blank line means "end" */
      
      if (post_data)
        StrAllocCat(command, post_data);
      else
        StrAllocCat(command, "lose");
    }

  StrAllocCat(command, crlf);	/* Blank line means "end" */

#ifdef PEM_AUTH
/*
 * HTAA_makecommand will check if we're doing PEM authentication, and if
 * so, will return a new command which is actually the command which was
 * just built, encrypted, and placed inside a dummy HTTP request. Body
 * is kept separate from the command because in the case of PGP data, the
 * message body can be binary.
 */

  body = NULL;
  body_size = -1;
  command = HTAA_makecommand(command,&body,&body_size);
#endif /* PEM_AUTH */
  
  if (TRACE)
    fprintf (stderr, "Writing:\n%s----------------------------------\n",
             command);
  
  HTProgress ("Sending HTTP request.");
  status = NETWRITE(s, command, (int)strlen(command));
#ifdef PEM_AUTH
  if(body) {
      status = NETWRITE(s, body, body_size);
      free(body);
  }
#endif /* PEM_AUTH */
  free (command);
  if (status <= 0) 
    {
      if (status == 0)
        {
          if (TRACE)
            fprintf (stderr, "HTTP: Got status 0 in initial write\n");
          /* Do nothing. */
        }
      else if 
        ((errno == ENOTCONN || errno == ECONNRESET || errno == EPIPE) &&
         !already_retrying &&
         /* Don't retry if we're posting. */ !do_post)
          {
            /* Arrrrgh, HTTP 0/1 compability problem, maybe. */
            if (TRACE)
              fprintf 
                (stderr, 
                 "HTTP: BONZO ON WRITE Trying again with HTTP0 request.\n");
            HTProgress ("Retrying as HTTP0 request.");
            NETCLOSE(s);
            extensions = NO;
            already_retrying = 1;
            goto try_again;
          }
      else
        {
          if (TRACE)
            fprintf (stderr, "HTTP: Hit unexpected network WRITE error; aborting connection.\n");
          NETCLOSE (s);
          status = -1;
          HTProgress ("Unexpected network write error; connection aborted.");
          goto done;
        }
    }
  
  if (TRACE)
    fprintf (stderr, "HTTP: WRITE delivered OK\n");
  HTProgress ("Done sending HTTP request; waiting for response.");

  /*	Read the first line of the response
   **	-----------------------------------
   */

#ifdef PEM_AUTH
/* 
 * reparse: We got an encapsulated response from the server, and have 
 * decrypted it. Now we restart the whole process with the decrypted 
 * response.
 */
 reparse:
  length=0;
  bytes_already_read=0;
  eol = 0;
#endif /* PEM_AUTH */ 
  {
    /* Get numeric status etc */
    BOOL end_of_file = NO;
    int buffer_length = INIT_LINE_SIZE;
    
    line_buffer = (char *) malloc(buffer_length * sizeof(char));
    
    do 
      {	/* Loop to read in the first line */
        /* Extend line buffer if necessary for those crazy WAIS URLs ;-) */
        if (buffer_length - length < LINE_EXTEND_THRESH) 
          {
            buffer_length = buffer_length + buffer_length;
            line_buffer = 
              (char *) realloc(line_buffer, buffer_length * sizeof(char));
          }
        if (TRACE)
          fprintf (stderr, "HTTP: Trying to read %d\n",
                   buffer_length - length - 1);
        status = NETREAD(s, line_buffer + length,
                         buffer_length - length - 1);
        if (TRACE)
          fprintf (stderr, "HTTP: Read %d\n", status);
        if (status <= 0) 
          {
            /* Retry if we get nothing back too; 
               bomb out if we get nothing twice. */
            if (status == HT_INTERRUPTED)
              {
                if (TRACE)
                  fprintf (stderr, "HTTP: Interrupted initial read.\n");
                HTProgress ("Connection interrupted.");
                status = HT_INTERRUPTED;
		NETCLOSE (s);
                goto clean_up;
              }
            else if 
              (status < 0 &&
               (errno == ENOTCONN || errno == ECONNRESET || errno == EPIPE) &&
               !already_retrying &&
               !do_post)
              {
                /* Arrrrgh, HTTP 0/1 compability problem, maybe. */
                if (TRACE)
                  fprintf (stderr, "HTTP: BONZO Trying again with HTTP0 request.\n");
                NETCLOSE(s);
                if (line_buffer) 
                  free(line_buffer);
                if (line_kept_clean) 
                  free(line_kept_clean);
                
                extensions = NO;
                already_retrying = 1;
                HTProgress ("Retrying as HTTP0 request.");
                goto try_again;
              }
            else
              {
                if (TRACE)
                  fprintf (stderr, "HTTP: Hit unexpected network read error; aborting connection; status %d.\n", status);
                HTProgress 
                  ("Unexpected network read error; connection aborted.");

                NETCLOSE (s);
                status = -1;
                goto clean_up;
              }
          }

        bytes_already_read += status;
        {
          char line[256];
          sprintf (line, "Read %d bytes of data.", bytes_already_read);
          HTProgress (line);
        }
        
        if (status == 0) 
          {
            end_of_file = YES;
            break;
          }
        line_buffer[length+status] = 0;
        
        if (line_buffer)
          {
            if (line_kept_clean)
              free (line_kept_clean);
            line_kept_clean = (char *)malloc (buffer_length * sizeof (char));
            bcopy (line_buffer, line_kept_clean, buffer_length);
          }
        
        eol = strchr(line_buffer + length, LF);
        /* Do we *really* want to do this? */
        if (eol && eol != line_buffer && *(eol-1) == CR) 
          *(eol-1) = ' '; 
        
        length = length + status;

        /* Do we really want to do *this*? */
        if (eol) 
          *eol = 0;		/* Terminate the line */
      }
    /* All we need is the first line of the response.  If it's a HTTP/1.0
       response, then the first line will be absurdly short and therefore
       we can safely gate the number of bytes read through this code
       (as opposed to below) to ~1000. */
    /* Well, let's try 100. */
    while (!eol && !end_of_file && bytes_already_read < 100);
  } /* Scope of loop variables */
    
    
  /*	We now have a terminated unfolded line. Parse it.
   **	-------------------------------------------------
   */
  if (TRACE)
    fprintf(stderr, "HTTP: Rx: %s\n", line_buffer);
  
  {
    int fields;
    char server_version[VERSION_LENGTH+1];
    int server_status;

    server_version[0] = 0;
    
    fields = sscanf(line_buffer, "%20s %d",
                    server_version,
                    &server_status);
    
    if (TRACE)
      fprintf (stderr, "HTTP: Scanned %d fields from line_buffer\n", fields);
    if (TRACE)
      fprintf (stderr, "HTTP: line_buffer '%s'\n", line_buffer);
    
    /* Rule out HTTP/1.0 reply as best we can. */
    if (fields < 2 || !server_version[0] || server_version[0] != 'H' ||
        server_version[1] != 'T' || server_version[2] != 'T' ||
        server_version[3] != 'P' || server_version[4] != '/' ||
        server_version[6] != '.') 
      {	
        /* HTTP0 reply */
        HTAtom * encoding;

        if (TRACE)
          fprintf (stderr, "--- Talking HTTP0.\n");
        
        format_in = HTFileFormat(arg, &encoding, WWW_HTML, &compressed);
        start_of_data = line_kept_clean;
      } 
    else 
      {
        /* Decode full HTTP response */
        format_in = HTAtom_for("www/mime");
        /* We set start_of_data to "" when !eol here because there
           will be a put_block done below; we do *not* use the value
           of start_of_data (as a pointer) in the computation of
           length or anything else in this situation. */
        start_of_data = eol ? eol + 1 : "";
        length = eol ? length - (start_of_data - line_buffer) : 0;
        
        if (TRACE)
          fprintf (stderr, "--- Talking HTTP1.\n");
        
        switch (server_status / 100) 
          {
          case 3:		/* Various forms of redirection */
            /* We now support this in the parser, at least. */
            doing_redirect = 1;
            break;
            
          case 4:		/* "I think I goofed" */
            switch (server_status) 
              {
              case 403:
                /* 403 is "forbidden"; display returned text. */
                break;

              case 401:
                /* length -= start_of_data - text_buffer; */
                if (HTAA_shouldRetryWithAuth(start_of_data, length, s)) 
                  {
                    (void)NETCLOSE(s);
                    if (line_buffer) 
                      free(line_buffer);
                    if (line_kept_clean) 
                      free(line_kept_clean);

                    if (TRACE) 
                      fprintf(stderr, "%s %d %s\n",
                              "HTTP: close socket", s,
                              "to retry with Access Authorization");
                    
                    HTProgress ("Retrying with access authorization information.");
                    goto try_again;
                    break;
                  }
                else 
                  {
                    /* Fall through. */
                  }

              default:
                break;
              } /* case 4 switch */
            break;

          case 5:		/* I think you goofed */
            break;
            
          case 2:		/* Good: Got MIME object */
            switch (server_status)
              {
              case 204:
                return_nothing = 1;
                format_in = HTAtom_for("text/html");
                break;
              default:
#ifdef PEM_AUTH
/*
 * HTAA_switchPEMresponse will return us a new file descriptor if we should
 * re-parse the response, will return -1 if some error occured, and will 
 * return -2 if there was no PEM going on at the time.
 */
                {
                    int s2;
                    if(doing_pem == NO) {
                        if((s2 = HTAA_switchPEMresponse(start_of_data, length, s)) == -1) {
                            HTProgress("An error occured while decrypting.");
                            status = -1;
			    NETCLOSE (s);
                            goto clean_up;
                        }

			if (s2 == -3)
			{
			    HTAA_ClearAuth();
			    (void)NETCLOSE(s);
			    if (line_buffer) 
			      free(line_buffer);
			    if (line_kept_clean) 
			      free(line_kept_clean);

			    if (TRACE) 
			      fprintf(stderr, "%s %d %s\n",
				      "HTTP: close socket", s,
				      "to retry without Access Authorization");
                    
			    HTProgress ("Retrying without access authorization information.");
			    goto try_again;
			}

                        if(s2 != -2) {
                            HTProgress("Got encrypted response, decrypting.");
                            doing_pem = YES;
                            s = s2;
                            goto reparse;
                        }
                    }
                }
#endif /* PEM_AUTH */
                break;
              }
            break;
            
          default:		/* bad number */
            HTAlert("Unknown status reply from server!");
            break;
          } /* Switch on server_status/100 */
        
      }	/* Full HTTP reply */
  } /* scope of fields */

  /* Set up the stream stack to handle the body of the message */
  target = HTStreamStack(format_in,
                         format_out,
                         compressed,
                         sink, anAnchor);
  
  if (!target) 
    {
      char buffer[1024];	/* @@@@@@@@ */
      sprintf(buffer, "Sorry, no known way of converting %s to %s.",
              HTAtom_name(format_in), HTAtom_name(format_out));
      HTProgress (buffer);
      status = -1;
      NETCLOSE (s);
      goto clean_up;
    }

  if (!return_nothing)
    {
      if (TRACE)
        fprintf (stderr, "HTTP: Doing put_block, '%s'\n", start_of_data);
      /* Recycle the first chunk of data, in all cases. */
      (*target->isa->put_block)(target, start_of_data, length);
      
      /* Go pull the bulk of the data down. */
      rv = HTCopy(s, target, bytes_already_read);
      if (rv == -1)
        {
          (*target->isa->handle_interrupt) (target);
          status = HT_INTERRUPTED;
	  NETCLOSE (s);
          goto clean_up;
        }
      if (rv == -2 && !already_retrying && !do_post)
        {
          /* Aw hell. */
          if (TRACE)
            fprintf (stderr, "HTTP: Trying again with HTTP0 request.\n");
          /* May as well consider it an interrupt -- right? */
          (*target->isa->handle_interrupt) (target);
          NETCLOSE(s);
          if (line_buffer) 
            free(line_buffer);
          if (line_kept_clean) 
            free(line_kept_clean);
          
          extensions = NO;
          already_retrying = 1;
          HTProgress ("Retrying as HTTP0 request.");
          goto try_again;
        }
    }
  else
    {
      /* return_nothing is high. */
      (*target->isa->put_string) (target, "<mosaic-access-override>\n");
      HTProgress ("And silence filled the night.");
    }

  (*target->isa->end_document)(target);

#ifdef PEM_AUTH
  /* If we're processing a PEM decrypted response, do extra cleanup */
  if(!HTAA_PEMclose(s))
#endif /* PEM_AUTH */
  /* Close socket before doing free. */
  NETCLOSE(s);
  (*target->isa->free)(target);

  if (doing_redirect)
    {
      /* OK, now we've got the redirection URL temporarily stored
         in external variable redirecting_url, exported from HTMIME.c,
         since there's no straightforward way to do this in the library
         currently.  Do the right thing. */
      status = HT_REDIRECTING;
    }
  else
    {
      status = HT_LOADED;
    }

  /*	Clean up
   */
  
 clean_up: 
  if (line_buffer) 
    free(line_buffer);
  if (line_kept_clean) 
    free(line_kept_clean);

 done:
  /* Clear out on exit, just in case. */
  do_post = 0;
  return status;
}


/*	Protocol descriptor
*/

PUBLIC HTProtocol HTTP = { "http", HTLoadHTTP, 0 };
