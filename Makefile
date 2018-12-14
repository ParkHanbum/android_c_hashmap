
extract_funcs:
	gcc -g extract_funcs.c -o $@ 

test_rbtree:
	gcc -g rbtree.c rbtree_main.c -o $@

test_hashmap:
	gcc -g hashmap.c hashmap_main.c -o $@

all:
	$(MAKE) extract_funcs
	$(MAKE) test_rbtree
	$(MAKE) test_hashmap

clean:
	rm test_rbtree
	rm test_hashmap
	rm extract_funcs

.DEFAULT_GOAL := all
