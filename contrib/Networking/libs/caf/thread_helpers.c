/*
 * THREAD_HELPERS.C, Copyright (C) Stefano Crosara 2006
 *
 * Implements some functions that can be safely called from threads in Amiga programs
 *
*/

#include <caf/caf_types.h>
#include <caf/thread_helpers.h>

/* Standard lib replacements */
void caf_memset(void *in_ptr, char c, int len)
{
	char *ptr = in_ptr;
	int i;
	
	for (i = 0; i < len; i++)
		*ptr++ = c;
}

int caf_strlen(char *buf)
{
	int i;
	
	i = 0;
	while (*buf++ != 0)
		i++;

	return i;
}

void caf_strncat(char *in_dest, char *in_src, int maxlen)
{
	int len = caf_strlen(in_dest), i;
	char *dest, *src = in_src;
	
	// 
	dest = &in_dest[len];
	for (i = 0; (i < maxlen-1) && (*src != 0); i++)
		*dest++ = *src++;
	
	// Add terminator
	*dest++ = 0;
}

int caf_strncpy(char *in_dest, char *in_src, int maxlen)
{
	char *dest = in_dest, *src = in_src;
	int i, cnt = 0;
	
	for (i = 0; (i < maxlen-1) && (*src != 0); i++) {
		*dest++ = *src++;
		cnt++;
	}
	
	// Add terminator
	*dest++ = 0;
	
	return cnt;
}

int caf_strcpy(char *in_dest, char *in_src)
{
	char *dest = in_dest, *src = in_src;
	int i, cnt = 0;
	
	for (i = 0; *src != 0; i++) {
		*dest++ = *src++;
		cnt++;
	}
	
	// Add terminator
	*dest++ = 0;
	
	return cnt;
}

int caf_strscanfor(char *string, char *scanforwhat)
{
	int slen, slen2, i, k, eb;
	int i_pos = -1;
	
	slen = caf_strlen(string);
	slen2= caf_strlen(scanforwhat); 
	slen -= slen2;
	
	// Se la stringa passata è + lunga della stringa input esce con false
	if (slen < 1)
		return -1;
	
	for (i = 0; i < slen; i++) {
		eb = 0;
		for (k = 0; k < slen2; k++) {
			if (string[i+k] == scanforwhat[k])
				eb++;
			else
				break;
		}
		
		if (eb == slen2) {
			i_pos = i;
			break;
		}
	}
	
	return i_pos;
}

int caf_atol(char *str)
{
	int multiplier = 1, i, numba = 0, base, ndigits = 0, iter_start = 0;
	#define CAF_ATOL_TEMPLEN 64
	char digits[CAF_ATOL_TEMPLEN];
	
	if (str[0] == '-')
		iter_start = 1;
	
	// Copia i digit nella stringa digit
	for (i = iter_start; (str[i] != 0)&&(i<CAF_ATOL_TEMPLEN); i++) {
		if (str[i] < '0' || str[i] > '9')
			break;
		
		digits[ndigits++] = str[i];
	}
	
	// Legge i digit al rovescio, 1234 dev'essere 4321 per partire da 10^1, 10^2, 10^3 etc
	for (i = 0; i < ndigits; i++) {
		base = (digits[ndigits-i-1]-'0');
		numba += base*multiplier;
		multiplier *= 10;
	}
	
	if (str[0] == '-')
		numba = -numba;
	
	return numba;
}

void caf_IToA(char *dest, int number, int maxlen)
{
	int i = 0, numba = number;
	char *dst = dest;
	
	do {
		*dst++ = (numba % 10)+'0';
		numba /= 10;
		i++;
	} while ((numba != 0) && (i < maxlen));
	
	// Add terminator
	*dst++ = 0;
}

void caf_IToACat(char *dest, int number, int maxlen)
{
	int len;
	
	len = caf_strlen(dest);
	if (len >= maxlen-2)
		return;
	
	caf_IToA(&dest[len], number, maxlen-len);
}

// Integer StrStr Last occurrence - ASDASD, should work from right to left
int caf_istrstrl(char *substr, char *str )
{
	int len_str, len_substr, i, j, curfind = -1;
	
	len_substr	= caf_strlen(substr);
	if (len_substr == 0)
		// No substring or string, blah
		return -1;
	
	for (i = 0; i < str[i] != 0; i++) {
		for (j = 0; j < len_substr && i+j < len_str; j++)
			if (str[i+j] != substr[j])
				break;
				
		if (j >= len_substr)
			// J is bigger or equal than len_substr, means the for above went ok for the whole comparison
			curfind = i;
	}
	
	// String not found
	return curfind;
}

// Integer StrStr First occurrence
int caf_istrstrf(char *substr, char *str)
{
	int len_str, len_substr, i, j;
	
	len_substr	= caf_strlen(substr);
	if (len_substr == 0)
		// No substring or string, blah
		return -1;
	
	for (i = 0; str[i] != 0; i++) {
		for (j = 0; j < len_substr && i+j < len_str; j++)
			if (str[i+j] != substr[j])
				break;
		
		if (j >= len_substr)
			// J is bigger or equal than len_substr, means the for above went ok for the whole comparison
			return i;
	}
	
	// String not found
	return -1;
}

// Finds string N start in buffer, N begins at 1
int caf_istrstrnum(int idx, char *str)
{
	int pos = -1, i, curstr = 0, start = 0;
	bool was_space = true;
	
	// First string begins at zero
	if (str[0] != 0)
		curstr = 0;
	
	/*
	0123456789
	 a       q
	oppp q www
	*/
	
	if (str[0] == ' ')
		start++;
	
	for (i = 0; str[i] != 0; i++) {
		if (str[i] != ' ' && was_space == true) {
			// If current char is not a space and previous was, a new string has been found
			curstr++;
			if (curstr == idx)
				return i;
		}
		
		was_space = str[i] == ' ';
	}
	
	return pos;
}

// Returns the number of substrings present in strin str
int caf_substrcnt(char *str)
{
	bool was_space = true;
	int i, curstr = 0;
	
	// First string begins at zero
	if (str[0] != 0)
		curstr = 0;
	
	/*
	0123456789
	 a       q
	oppp q www
	*/
	
	for (i = 0; str[i] != 0; i++) {
		if (str[i] != ' ' && was_space == true) {
			// If current char is not a space and previous was, a new string has been found
			curstr++;
		}
		
		was_space = str[i] == ' ';
	}
	
	return curstr;
}

// Terminates the substrings in "str" and returns the substring count, and strings start index 
int caf_substrset(char *str, int *strarray, int strmax)
{
	bool was_space = true;
	int i, curstr = 0;
	
	for (i = 0; str[i] != 0; i++) {
		if (str[i] != ' ' && was_space == true) {
			// If current char is not a space and previous was, a new string has been found
			strarray[curstr] = i;
			str[i-1] = 0;
			curstr++;
			if (curstr >= strmax)
				break;
		}
		
		was_space = str[i] == ' ';
	}
	
	return curstr;
}

// Adds N characters to the string up to len chars
void caf_strcatpad(char *str, int up_to, char padchar)
{
	bool pad_it = false;
	int i = 0;
	
	/*
	 0123456789
	"ABC"
	"ABCzzzzzzz"
	*/
	
	for (i = 0; i < up_to; i++) {
		if (str[i] == 0)
			pad_it = true;
		
		if (pad_it)
			str[i] = padchar;
	}
	
	// Terminate the baby
	if (pad_it)
		str[i] = 0;
}

int caf_getlastchar(char *str)
{
	int c = 0, i;
	
	for (i = 0; str[i] != 0; i++)
		c = str[i];
	
	return c;
}

void BSwapDWORD(int *dest, int *srce)
{
	char *c_dest = (char *) dest, *c_srce = (char *) srce, buf[4];
	
	buf[0] = c_srce[3];
	buf[1] = c_srce[2];
	buf[2] = c_srce[1];
	buf[3] = c_srce[0];
	
	c_dest[0] = buf[0];
	c_dest[1] = buf[1];
	c_dest[2] = buf[2];
	c_dest[3] = buf[3];
}

/* End Standard lib replacements */
