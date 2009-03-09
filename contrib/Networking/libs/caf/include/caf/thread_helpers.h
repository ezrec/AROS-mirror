/*
 * THREAD_HELPERS.H, Copyright (C) Stefano Crosara 2006
 *
 * Implements some functions that can be safely called from threads in Amiga programs
 *
*/

#ifndef THREAD_HELPERS_H
#define THREAD_HELPERS_H

/* Standard lib replacements */
void caf_memset(void *in_ptr, char c, int len);
int caf_strlen(char *buf);
int caf_strncpy(char *dest, char *src, int maxlen);
int caf_strcpy(char *in_dest, char *in_src);
int caf_strscanfor(char *string, char *scanforwhat);	// Searc
void caf_strncat(char *in_dest, char *in_src, int maxlen);
int caf_atol(char *str);
void caf_IToA(char *dest, int number, int maxlen);
void caf_IToACat(char *dest, int number, int maxlen);
int caf_istrstrl(char *substr, char *str);				// Finds last occurrence of substr in str
int caf_istrstrf(char *substr, char *str);				// Finds first occurrence of substr in str
int caf_istrstrnum(int idx, char *str);					// Finds where substr i starts in str
int caf_substrcnt(char *str);							// Counts the number of substrings in str
int caf_retsubstr(int *subarray, char *str);			// Returns the offset of each substring
														// found in str
														// NOTE: the substrings in str are
														// 		 zeroed, original string is
														//		 thus modified
int caf_substrset(char *str, int *strarray, int strmax); // // Terminates the substrings in "str" and returns the substring count, and strings start index 
void caf_strcatpad(char *str, int up_to, char padchar);	// Pads a string (RIGHT)
int caf_getlastchar(char *str);							// Gets last string's char
void BSwapDWORD(int *dest, int *srce);

/* End Standard lib replacements */


#endif
