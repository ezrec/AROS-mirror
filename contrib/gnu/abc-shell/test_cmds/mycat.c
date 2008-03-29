#include <stdio.h>

int main(int argc, char** argv)
{
	char c='a';
	do{
		scanf("%c", &c);
		printf("%c", c==10 ? c : c+1);
	}while(c!=10);
	return 0;
}
