
#include <stdio.h>
#include <stdint.h>
#include <string.h>


#define READELF_TYPE_FUNC "FUNC"

int main(int argc, char **argv)
{
	char buf[1024];
	FILE* fp;
	long address;
	uint32_t size;
	char type[1024];
	char bind[1024];
	char name[1024];

	fp = fopen(argv[1], "r");
	/*
Num:    Value          Size Type    Bind   Vis      Ndx Name
0: 0000000000000000     0 NOTYPE  LOCAL  DEFAULT  UND 
36: 00000000004e7410    32 FUNC    LOCAL  DEFAULT   14 _ZN2v88internalL16f64_aco

  */
	if (fp == NULL)
		printf("There is no file node_dbg.syms\n");


	while (fgets(buf, sizeof(buf), fp) != NULL) {
		sscanf(buf, "%*s %lx %d %s %s %*s %*s %s",
			&address, &size, type, bind, name);
		if (strncmp(type, READELF_TYPE_FUNC, sizeof(READELF_TYPE_FUNC))
				|| address < 1 || size < 1)
			continue;
		printf("%lx %d %s %s %s\n", address, size, type, bind, name);
	}
	fclose(fp);
}
