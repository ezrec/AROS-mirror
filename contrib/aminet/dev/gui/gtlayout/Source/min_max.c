#include <exec/types.h>

LONG min(LONG a, LONG b)
{
	return (a < b) ? a : b;
}

LONG max(LONG a, LONG b)
{
	return (a > b) ? a : b;
}
