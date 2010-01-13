#include <stdio.h>

int stcl_o (char * out,long lvalue)
{
    return sprintf(out, "%lo", lvalue);
}

char * strupr (char * str)
{
	unsigned int i = 0;
	while (str[i]) {
		if (str[i] >= 'a' && str[i] <= 'z')
			str[i] += ('A'-'a');
		i++;
	}

	return str;
}
