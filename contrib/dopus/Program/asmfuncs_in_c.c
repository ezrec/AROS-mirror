#include "dopus.h"
#include "view.h"

int countlines(struct ViewData *vd)
{
	int charcount, linecount;
	int size;
	
	int max_line_length, mll_p;
	
	char *buf;
	
	char *lws; /* Last whitespace character (for wordwrap) */
	
	size = vd->view_file_size;
	lws = NULL;
	
	max_line_length = vd->view_max_line_length;
	buf = vd->view_text_buffer;
	mll_p = max_line_length - 10;
	
	charcount = linecount = 0;
	
	do {
		charcount ++;
		if (charcount != max_line_length) {
			/* skip1 */
			
			/* Not max length yet */
			if (10 == *buf) {
				/* EOL */
				lws = NULL;
				linecount ++;
				charcount = 0;
				
			} else if (9 == *buf) {
				/* skip2 */
				
				/* TAB */
				*buf = 32;
			}
			
		} else {
			if (NULL != lws) {
				int diff;
				
				diff = buf - lws;
				
				if (!(mll_p >= diff)) {
				
					/* We restart from last whitespace */
					size += diff;
					buf = lws;
				}
				
				
			}
			
			/* notlastspace */
			*buf = 10;
			linecount ++;
			
			charcount = 0;
			lws = NULL;
		}
		
		/* skip3 */
		if (32 == *buf) {
			lws = buf;
		}
		
		/* notspace */
		
		buf ++;
		
	
	} while (-- size);
	
	/* endcountlines */
	if (10 != *buf) {
		linecount ++;
	}
	
	/* lastlf */
	
	return linecount;
}

int smartcountlines(struct ViewData *vd)
{
	int size, sizebak;
	
	char *buf, *lws;
	
	int linecount;
	int charcount;
	int hexcount;
	
	int max_line_length, mll_p;
	BOOL last_was_escape;
	
	size = vd->view_file_size;
	sizebak = size;
	
	buf = vd->view_text_buffer;
	
	linecount = 0;
	charcount = 0;
	hexcount = 0;
	last_was_escape = FALSE;
	
	max_line_length = vd->view_max_line_length;
	
	mll_p = max_line_length - 10;
	
	do {
		charcount ++;
		if (max_line_length != charcount) {
			BOOL is_ascii = FALSE;
			/* askip1 */
			
			if (*buf < -33) {
			    is_ascii = TRUE;
			} else if (*buf <= -1) {
			    is_ascii = FALSE;
			} else if (*buf < 9) {
			    is_ascii = FALSE;
			} else if (*buf < 14) {
			    is_ascii = TRUE;
			} else if (*buf < 32) {
			    is_ascii = FALSE;
			} else if (((unsigned char)*buf) <= 127) {
			    is_ascii = TRUE;
			} else if (((unsigned char)*buf) > 160) {
			    is_ascii = TRUE;
			}

			if (!is_ascii)
			{
			    /* aishex */

			    if (sizebak < 6) return -1;
			    hexcount++;
			    if (hexcount >= 6) return -1;
			    if (*buf == 27) last_was_escape = TRUE;			     
			} else {
			    /* aokay */
			    hexcount = 0;
			    
			    if (last_was_escape)
			    {
			        if (*buf == '[') return -2; /* isansi */
			        last_was_escape = FALSE;
			    }
			}
			
			/* aokay1 */
			if (*buf == 10)
			{
			    lws = 0;
			    linecount++;
			    charcount = 0;
			}
			
			
		} else  {
			
			if (NULL != lws) {
				int diff;
				diff = buf - lws;
				if (!(mll_p >= diff)) {
					size += diff;
					buf = lws;
				}
			}
						
			/* nolastspace1 */
			*buf = 10;
			
			lws = 0;
			linecount ++;
			charcount = 0;
			
		}

		/* askip2 */
		if (*buf == 32)
		{
		    lws = buf;
		}
		
		/* notspace1 */
		
		buf++;
		
	} while (-- size);
	
	if (*buf != 10) linecount++;
	
	return linecount;
}

int ansicountlines(struct ViewData *vd)
{
#warning Finish this

return -1;
}


void removetabs(struct ViewData *vd)
{
	int size;
	char *buf;
	
	size = vd->view_file_size;
	buf = vd->view_text_buffer;
	
	
	while (size --) {
	
		if (9 == *buf) {
			*buf = ' ';
		}
		buf ++;
	}
	
}

