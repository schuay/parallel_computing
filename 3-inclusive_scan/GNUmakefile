RSYNC = rsync
SSH = ssh
SCP = scp

SFLAGS = -F "../.ssh-cfg"
RFLAGS = --recursive --progress --copy-links --exclude=build/ -e "$(SSH) $(SFLAGS)"

HOST = jupiter
REMOTEDIR = ~/project3-inclusive_scan
SOURCE = . ../common

CSV_FILE = data/bench.csv
INPUT_SIZES = 10 100 1000000 100000000
NUM_THREADS = 1 2 4
NUM_THREADS_REMOTE = 8 16 32 48
SEED = 42

bake:
	mkdir -p build
	cd build; cmake -DCMAKE_BUILD_TYPE="Debug" ..; make -j3

test: bake
	cd build; CK_FORK=no ctest --output-on-failure

bench: bake
	mkdir -p data
	for inSize in $(INPUT_SIZES); do \
		mpiexec -np 1 build/arrayscan_ref_bench $(CSV_FILE) $${inSize} $(SEED); \
		for nThreads in $(NUM_THREADS); do \
			mpiexec -np $${nThreads} build/arrayscan_par_mpi_bench $(CSV_FILE) $${inSize} $(SEED); \
		done; \
	done

hand-in:
	$(RSYNC) $(RFLAGS) $(SOURCE) $(HOST):$(REMOTEDIR)

remote-bench: hand-in
	$(SSH) $(SFLAGS) $(HOST) 'cd $(REMOTEDIR); PATH=/opt/cmake/bin/:/opt/openmpi/bin/:${PATH} \
		MPI_HOME=/opt/openmpi make NUM_THREADS="$(NUM_THREADS_REMOTE)" clean bench'
	mkdir -p data
	$(SCP) $(SFLAGS) $(HOST):$(REMOTEDIR)/$(CSV_FILE) $(CSV_FILE)

clean:
	rm -rf build
	rm -rf data


