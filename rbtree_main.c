#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "rbtree.h"

struct mcount_mem_regions {
	struct rb_root root;
	unsigned long  heap;
	unsigned long  brk;
};

struct mem_region {
	struct rb_node          node;
	unsigned long           start;
	unsigned long           end;
};

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
		    unsigned long end)
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

	rb_link_node(&entry->node, parent, p);
	rb_insert_color(&entry->node, root);
}

struct mcount_mem_regions regions;

int main()
{
	struct mem_region* res;
	add_mem_region(&regions.root, 0x400000, 0x401000);
	res = find_mem_region(&regions.root, 0x400000);

	if (res != NULL)
		printf("%lx %lx\n", res->start, res->end);
}
