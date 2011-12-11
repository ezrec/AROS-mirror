/*
 *      Returns true if string s matches pattern p.
 */

#include <ctype.h>

/* p == pattern             */
/* s == string to match     */
/* f == flag for case force */

int patmatch(char *p, char *s, int f)
{
    char pc;			/* a single character from pattern */
    char c;

    c = *p++;
    while ((pc = ((f && islower(c)) ? toupper(c) : c)))
    {
	if (pc == '*')
	{
	    do {			/* look for match till s exhausted */
		if (patmatch (p, s, f))
		    return (1);
	    } while (*s++);
	    return (0);
	}
	else
	{
	    if (*s == 0)
		return (0);		/* s exhausted, p not */
	    else if (pc == '?')
	    	s++;			/* matches all, just bump */
            else
	    {
		c = *s++;
		if (pc != ((f && islower(c)) ? toupper(c) : c))
		    return (0);
	    }
	}
    }
    return (!*s);			/* p exhausted, ret true if s exhausted */
}
