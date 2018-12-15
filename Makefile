
RM := rm -f
CFLAGS := -g -pg -mfentry -mnop-mcount -fno-pic -no-pie

extract_funcs:
	gcc -g extract_funcs.c -o $@ 

OPTIMIZE = 0 1 2 3 s
test_rbtree:
	$(foreach var, $(OPTIMIZE), gcc $(CFLAGS) -O$(var) rbtree.c rbtree_main.c -o $@_O$(var);)

test_hashmap:
	$(foreach var, $(OPTIMIZE), gcc $(CFLAGS) -O$(var) hashmap.c hashmap_main.c -o $@_O$(var);)

rbtree_bench: test_rbtree
	$(foreach var,$(OPTIMIZE), \
		uftrace record --no-libcall -P start_bmt -d $@_O$(var) test_rbtree_O$(var) ;)

hashmap_bench: test_hashmap
	$(foreach var,$(OPTIMIZE), \
		uftrace record --no-libcall -P start_bmt -d $@_O$(var) test_hashmap_O$(var) ;)

report: rbtree_bench hashmap_bench
	$(shell echo "Benchmark Results of rbtree......." >> report.log)
	$(foreach var,$(OPTIMIZE), \
		$(shell echo "OPTIMIZE $(var)" >> report.log) \
		$(shell uftrace report -d rbtree_bench_O$(var) >> report.log) )

	$(shell echo "Benchmark Results of hashmap......." >> report.log)
	$(foreach var,$(OPTIMIZE), \
		$(shell echo "OPTIMIZE $(var)" >> report.log) \
		$(shell uftrace report -d hashmap_bench_O$(var) >> report.log) )

all:
	$(MAKE) extract_funcs
	$(MAKE) test_rbtree
	$(MAKE) test_hashmap
	$(MAKE) rbtree_bench
	$(MAKE) hashmap_bench
	$(MAKE) report

clean:
	$(RM) test_rbtree_O*
	$(RM) test_hashmap_O*
	$(RM) extract_funcs
	$(RM) -r hashmap_bench_O*
	$(RM) -r rbtree_bench_O*
	$(RM) report.log

.DEFAULT_GOAL := all
