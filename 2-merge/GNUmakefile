RSYNC = rsync
SSH = ssh
SCP = scp

CC = $(shell [ -x /opt/cilk-5.4.6/bin/cilkc ] && echo -n "/opt/cilk-5.4.6/bin/cilkc" \
     || echo -n "cilkc")

COMMON = $(shell [ -d ../common ] && echo -n "../common" || echo -n "common")

CILK_INCLUDE = $(shell [ -d /opt/cilk-5.4.6/include/cilk ] && \
	       echo -n "/opt/cilk-5.4.6/include/cilk" || echo -n "/usr/include/cilk")

SFLAGS = -F "../.ssh-cfg"
RFLAGS = --recursive --progress --copy-links -e "$(SSH) $(SFLAGS)"
CFLAGS = -D_XOPEN_SOURCE=600 -I ${COMMON} -I ${CILK_INCLUDE}

HOST = saturn
REMOTEDIR = ~/project2-merge
SOURCE = . ${COMMON}

CSV_FILE = bench.csv
INPUT_SIZES = 10 100 1000000 100000000
NUM_THREADS = 1 2 4 0 # 0 means use the default
NUM_THREADS_REMOTE = 8 16 32 0

bake: test_build bench_build

test_build:
	mkdir -p build
	cd build
	${CC} ${CFLAGS} -lcheck recursive.cilk merge.c test.c -o build/recursive_test
	${CC} ${CFLAGS} -lcheck binsearch.cilk merge.c test.c -o build/binsearch_test

bench_build:
	mkdir -p build
	cd build
	${CC} ${CFLAGS} recursive.cilk merge.c bench.c ${COMMON}/perf.c ${COMMON}/csv.c \
	    -o build/recursive_bench
	${CC} ${CFLAGS} binsearch.cilk merge.c bench.c ${COMMON}/perf.c ${COMMON}/csv.c \
	    -o build/binsearch_bench
	${CC} ${CFLAGS} reference.cilk merge.c bench.c ${COMMON}/perf.c ${COMMON}/csv.c \
	    -o build/reference_bench

test: test_build
	./build/recursive_test
	./build/binsearch_test

bench: bench_build
	mkdir -p data
	for inSize in $(INPUT_SIZES); do \
		build/reference_bench data/$(CSV_FILE) $${inSize} 1; \
		for nThreads in $(NUM_THREADS); do \
			build/recursive_bench data/$(CSV_FILE) $${inSize} $${nThreads}; \
			build/binsearch_bench data/$(CSV_FILE) $${inSize} $${nThreads}; \
		done \
	done

hand-in:
	$(RSYNC) $(RFLAGS) $(SOURCE) $(HOST):$(REMOTEDIR)

remote-bench: hand-in
	$(SSH) $(SFLAGS) $(HOST) 'cd $(REMOTEDIR); make NUM_THREADS="$(NUM_THREADS_REMOTE)" clean bench'
	mkdir -p data
	$(SCP) $(SFLAGS) $(HOST):$(REMOTEDIR)/data/$(CSV_FILE) data/$(CSV_FILE)

clean:
	rm -rf build
