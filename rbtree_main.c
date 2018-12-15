#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "rbtree.h"
#include "common.h"

int test();

struct mcount_mem_regions {
	struct rb_root root;
	unsigned long  heap;
	unsigned long  brk;
};

struct mem_region {
	struct rb_node          node;
	unsigned long           start;
	unsigned long           end;
	char			*name;
};

struct mcount_mem_regions g_regions;

void *find_mem_region(struct rb_root *root,
		      unsigned long addr)
{
	struct rb_node *parent = NULL;
	struct rb_node **p = &root->rb_node;
	struct mem_region *iter;

	while (*p) {
		parent = *p;
		iter = rb_entry(parent, struct mem_region, node);

		if (iter->start <= addr && addr < iter->end)
			return iter;

		if (iter->start > addr)
			p = &parent->rb_left;
		else
			p = &parent->rb_right;
	}
	return 0;
}

void add_mem_region(struct rb_root *root,
		    unsigned long start,
		    unsigned long end,
		    char *name)
{
	struct rb_node *parent = NULL;
	struct rb_node **p = &root->rb_node;
	struct mem_region *iter, *entry;

	while (*p) {
		parent = *p;
		iter = rb_entry(parent, struct mem_region, node);
		if (iter->start > start)
			p = &parent->rb_left;
		else
			p = &parent->rb_right;
	}

	entry = malloc(sizeof(*entry));
	entry->start = start;
	entry->end = end;
	entry->name = strdup(name);

	rb_link_node(&entry->node, parent, p);
	rb_insert_color(&entry->node, root);
}


void keep_addr(long address)
{
	func_addr[func_count++] = address;
}

void start_bmt()
{
	int count = 0;
	struct mem_region* res;

	// printf("total funcs : %lu\n", func_count);

	for (;count < func_count;count++) {
		long addr = func_addr[count];
		res = find_mem_region(&g_regions.root, addr);
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

		// save data to entry
		add_mem_region(&g_regions.root, address, address + size, name);
		// printf("%lx %x %s %s %s\n", address, size, type, bind, name);

		keep_addr(address);
	}
	fclose(fp);

	// test();
	start_bmt();
}

int test()
{
	struct mem_region* res;
	add_mem_region(&g_regions.root, 0x400000, 0x401000, "main");
	res = find_mem_region(&g_regions.root, 0x400000);

	if (res != NULL)
		printf("%lx %lx %s\n", res->start, res->end, res->name);
}

