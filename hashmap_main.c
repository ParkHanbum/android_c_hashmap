#include <stdio.h>
#include <assert.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/types.h>
#include <stdint.h>
#include "threads.h"
#include "hashmap.h"
#include "common.h"

int test();

struct Entry {
	long start;
	long end;
	char *name;
};

bool iterator(void* key, void* value, void* context)
{
	struct Entry *ent = (struct Entry *)value;
	printf("KEY : %lx, VALUE : %lx %lx %s\n",
		*(unsigned long *)key,
		ent->start, ent->end, ent->name);
	return true;
}


Hashmap *map;

void keep_addr(long address)
{
	func_addr[func_count++] = address;
}

void start_bmt()
{
	int count = 0;
	struct Entry *res;

	// printf("total funcs : %lu\n", func_count);

	for (;count < func_count;count++) {
		long addr = func_addr[count];
		res = (struct Entry *)hashmapGet(map, &addr);
		// printf("%lx %lx %s\n", res->start, res->end, res->name);

		if (res ==  NULL) {
			printf("something wrong %lx\n", addr);
			return;
		}
	}
}

int main()
{
	char buf[1024];
	FILE* fp;
	long address;
	uint32_t size;
	char type[1024];
	char bind[1024];
	char name[1024];

	map = hashmapCreate(50000, hashmapDefaultHash, hashmapDefaultEquals);

	fp = fopen(DATA_FILE_NAME, "r");
/*
Num:    Value          Size Type    Bind   Vis      Ndx Name
0: 0000000000000000     0 NOTYPE  LOCAL  DEFAULT  UND
36: 00000000004e7410    32 FUNC    LOCAL  DEFAULT   14 _ZN2v88internalL16f64_aco
*/
	if (fp == NULL)
		printf("There is no file node_dbg.syms\n");

	while (fgets(buf, sizeof(buf), fp) != NULL) {
		sscanf(buf, "%*s %lx %x %s %s %*s %*s %s",
			&address, &size, type, bind, name);
		if (strncmp(type, READELF_TYPE_FUNC, sizeof(READELF_TYPE_FUNC))
				|| address < 1 || size < 1)
			continue;
		struct Entry* ent = malloc(sizeof(struct Entry));
		ent->start = address;
		ent->end = address + size;
		ent->name = strdup(name);
		// check the key has already exist in hashmap before putting it.
		if (hashmapContainsKey(map, ent)) {
			// printf("This address already exist\n");
			hashmapGet(map, ent);
		}
		void *oldvalue = hashmapPut(map, &ent->start, ent);
		if (oldvalue != NULL) {
			struct Entry* _ent = (struct Entry *)oldvalue;
			// printf("[replaced] %lx %lx %s\n", _ent->start, _ent->end, _ent->name);
			// printf("[replacer] %lx %lx %s\n", ent->start, ent->end, ent->name);
		}
		// printf("%lx %x %s %s %s\n", address, size, type, bind, name);
		keep_addr(address);
	}
	fclose(fp);

	// test();
	start_bmt();
}
