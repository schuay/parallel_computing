RSYNC = rsync
SSH = ssh
SCP = scp

SFLAGS = -F "../.ssh-cfg"
RFLAGS = --recursive --progress --copy-links --exclude=build/ -e "$(SSH) $(SFLAGS)"

HOST = saturn
REMOTEDIR = ~/project1-prefix_sums
SOURCE = . ../common

CSV_FILE = bench.csv
WORK_FILE = work.csv

ALGORITHMS = recursive iterative hillis-steele reduction
INPUT_SIZES = 10 100 1000000 1111111 2000000 5000000 6666666 100000000
NUM_THREADS = 1 2 4 0 # 0 means use the default
NUM_THREADS_REMOTE = 8 16 32 0

bake:
	mkdir -p build
	cd build; cmake -DCMAKE_BUILD_TYPE="Debug" ..; make -j3

test: bake
	cd build; ctest --output-on-failure

bench: bake
	mkdir -p data
	for inSize in $(INPUT_SIZES); do \
		build/reference_bench data/$(CSV_FILE) $${inSize} 1; \
		for alg in $(ALGORITHMS); do \
			build/$${alg}_bench data/$(CSV_FILE) $${inSize} $(NUM_THREADS); \
		done; \
	done

work: bake
	mkdir -p data
	rm -f data/$(WORK_FILE)
	for alg in $(ALGORITHMS); do \
		build/$${alg}_work data/$(WORK_FILE) $(INPUT_SIZES); \
	done


hand-in:
	$(RSYNC) $(RFLAGS) $(SOURCE) $(HOST):$(REMOTEDIR)

remote-bench: hand-in
	$(SSH) $(SFLAGS) $(HOST) 'cd $(REMOTEDIR); make NUM_THREADS="$(NUM_THREADS_REMOTE)" clean bench'
	mkdir -p data
	$(SCP) $(SFLAGS) $(HOST):$(REMOTEDIR)/data/$(CSV_FILE) data/$(CSV_FILE)

clean:
	rm -rf build
