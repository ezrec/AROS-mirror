#include "clib_stuff.h"

#if 0
char *StrCpy(char *dst, char *src) {
char *back;

	back=dst;
	while ((*dst++ = *src++));
	return back;
}

char *StrCat(char *dst, char *src) {
char *back;

	back=dst;
	while (*dst++);
	dst--;
	while ((*dst++ = *src++));
	return back;
}

int StrLen(char *str) {
int count=0;

	while (*str++)
		count++;
	return count;
}

char *StrChr(char *str, int c) {

	while (*str)
	{
		if (*str == c)
			return str;
		str++;
	}
	return 0;
}

char *StrStr(char *src, char *dst) {
char *back;
char *buf;

	while (*src)
	{
		while ((*src++ != *dst) && (*src));
		if (*src == 0)
			return 0;
		buf = dst+1;
		back = src;
		while ((*buf) && (*back) && (*buf++ == *back++));
		if (*buf==0)
			return (src-1);
	}
	return 0;
}

int StrCmp(char *src, char *dst) {

	do
	{
		if ((*src==0) && (*dst == 0))
			return 0;
	} while (*src++ == *dst++);
	return *src == 0 ? -1 : 1;
}

int StrNCmp(char *src, char *dst, int len) {

	while (*src == *dst)
	{
		if (len == 0)
			return 0;
		if (*src++ == 0)
			return -1;
		if (*dst++ == 0)
			return 1;
		len--;
	}
	return len;
}

void *MemSet(void *s, int c, int n) {
char *str = (char *)s;

	while (n)
	{
		*str++=c;
		n--;
	}
	return s;
}

int MemCmp(void *src, void *dst, int len) {
char *b1;
char *b2;

	while (*b1++ == *b2++)
	{
		if (len == 0)
			return 0;
		len--;
	}
	return len;
}
#endif
