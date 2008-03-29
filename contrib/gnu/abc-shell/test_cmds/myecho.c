#include <stdio.h>

int main(int argc, char** argv)
{
	int i;
	for(i = 1; i < argc; i++)
	{	
		if(i > 1) printf(" ");
		printf("%s", argv[i]);
	}
	if(argc > 1) printf("\n");
	system("echo 1 >>ram:echo.out");
	return 0;
}
