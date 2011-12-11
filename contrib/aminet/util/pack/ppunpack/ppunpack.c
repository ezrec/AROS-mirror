/* ppunpack.c */
  
#include <stdio.h>
#include <stdlib.h>
#include "depack.h"

#define VERSION "1.0"

int main(argc, argv)
int argc;
char **argv;
	{
	FILE *file;
	ulong plen, unplen;

	ubyte *packed, *unpacked;

	if (argc != 3)
		{
		printf("ppunpack %s\nUsage: %s infile outfile\n", VERSION, argv[0]);
		exit(1);
		}

	file = fopen(argv[1],"rb");
	if (!file)
		{
		fprintf(stderr, "%s: can't open input file\n", argv[0]);
		exit(10);
		}
	fseek(file, 0, SEEK_END);
	plen = ftell(file);
	rewind(file);
	packed = (ubyte *)malloc(plen);
	if (!packed)
		{
		fprintf(stderr, "%s: can't allocate source memory\n", argv[0]);
		exit(10);
		}

	fread(packed, 1, plen, file);
	fclose(file);

	unplen = depackedlen(packed, plen);
	if (!unplen)
		{
		fprintf(stderr, "%s: not a powerpacked file\n", argv[0]);
		exit(10);
		}
	unpacked = (ubyte *)malloc(unplen);
	if (!unpacked)
		{
		fprintf(stderr, "%s: can't allocate destination memory\n", argv[0]);
		exit(10);
		}

	ppdepack(packed, unpacked, plen, unplen);

	file = fopen(argv[2],"wb");
	if (!file)
		{
		fprintf(stderr, "%s: can't write unpacked file\n", argv[0]);
		exit(10);
		}

	fwrite(unpacked, 1, unplen, file);
	fclose(file);
	return 0;
	}
