RSYNC = rsync
SSH = ssh
SCP = scp

SFLAGS = -F "../.ssh-cfg"
RFLAGS = --recursive --progress --copy-links -e "$(SSH) $(SFLAGS)"
CFLAGS = -D_XOPEN_SOURCE=600 -I ../common -I /usr/include/cilk

HOST = saturn
REMOTEDIR = ~/project2-merge
SOURCE = . ../common

CSV_FILE = bench.csv
INPUT_SIZES = 10 100 1000000 100000000
NUM_THREADS = 1 2 4 0 # 0 means use the default
NUM_THREADS_REMOTE = 8 16 32 0

bake:
	mkdir -p build
	cd build
	cilkc ${CFLAGS} -lcheck recursive.cilk merge.c test.c -o build/recursive_test
	cilkc ${CFLAGS} -static recursive.cilk merge.c bench.c ../common/perf.c ../common/csv.c \
	    -o build/recursive_bench
	cilkc ${CFLAGS} -static reference.cilk merge.c bench.c ../common/perf.c ../common/csv.c \
	    -o build/reference_bench

test: bake
	./build/*_test

bench: build/reference_bench build/recursive_bench
	mkdir -p data
	for inSize in $(INPUT_SIZES); do \
		build/reference_bench data/$(CSV_FILE) $${inSize} 1; \
		for nThreads in $(NUM_THREADS); do \
			build/recursive_bench data/$(CSV_FILE) $${inSize} $${nThreads}; \
		done \
	done

hand-in:
	$(RSYNC) $(RFLAGS) $(SOURCE) $(HOST):$(REMOTEDIR)

remote-bench: bake hand-in
	$(SSH) $(SFLAGS) $(HOST) 'cd $(REMOTEDIR); make NUM_THREADS="$(NUM_THREADS_REMOTE)" bench'
	mkdir -p data
	$(SCP) $(SFLAGS) $(HOST):$(REMOTEDIR)/data/$(CSV_FILE) data/$(CSV_FILE)

clean:
	rm -rf build
