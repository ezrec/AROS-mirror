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

#warning Finish the unwritten asm funcs
return -1;
#if 0
	int size, sizebak;
	
	char *buf, lws;
	
	int linecount;
	int charcount;
	
	int max_line_length, mll_p;
	
	
	size = vd->view_file_size;
	sizebak = size;
	
	buf = vd->view_text_buffer;
	
	linecount = 0;
	charcount = 0;
	
	max_line_length = vd->view_max_line_length;
	
	mll_p = max_line_length - 10;
	
	do {
		charcount ++;
		if (max_line_length != charcount) {
			/* askip1 */
			
			if (*buf < - 33) {
			} else if (*buf <= -1) {
			
			} else if (*buf < 9) {
			
			} else if (*buf < 14) {
			
			} else if (*buf < 32) {
			
			} else if (*buf < 128) {
			
			} else if (*buf > 160) {
			
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
		
	} (while);
#endif
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

