RSYNC = rsync
SSH = ssh
SCP = scp

SFLAGS = -F "../.ssh-cfg"
RFLAGS = --recursive --progress --copy-links --exclude=build/ -e "$(SSH) $(SFLAGS)"

HOST = saturn
REMOTEDIR = ~/project2-merge
SOURCE = . ../common

CSV_FILE = bench.csv
INPUT_SIZES = 10 100 1000000 100000000
NUM_THREADS = 1 2 4 0 # 0 means use the default
NUM_THREADS_REMOTE = 8 16 32 0

bake:
	mkdir -p build
	cd build; cilkc -O3 ../main.cilk -o merge;

test: bake
	cd build; ctest --output-on-failure

bench: bake
	mkdir -p data
	for inSize in $(INPUT_SIZES); do \
		build/merge_seq_bench data/$(CSV_FILE) $${inSize} 1; \
		for nThreads in $(NUM_THREADS); do \
			build/merge_bench data/$(CSV_FILE) $${inSize} $${nThreads}; \
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
