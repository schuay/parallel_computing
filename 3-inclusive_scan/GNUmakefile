RSYNC = rsync
SSH = ssh
SCP = scp

SFLAGS = -F "../.ssh-cfg"
RFLAGS = --recursive --progress --copy-links --exclude=build/ -e "$(SSH) $(SFLAGS)"

HOST = jupiter
REMOTEDIR = ~/project3-inclusive_scan

COMMON = $(shell [ -d ../common ] && echo -n "../common" || echo -n "common")
SOURCE = . $(COMMON)

CSV_FILE = data/bench.csv
INPUT_SIZES = 10 100 1000000 100000000 111111111 1000000000
NUM_THREADS = 1 2 4
NUM_THREADS_REMOTE = 8 16 32 64 128 256 511 512
SEED = 42
HOSTS=jupiter0,jupiter1,jupiter2,jupiter3,jupiter4,jupiter5,jupiter6,jupiter7,jupiter8,jupiter9,jupiter10,jupiter11,jupiter12,jupiter13,jupiter14,jupiter15,jupiter16,jupiter17,jupiter18,jupiter19,jupiter20,jupiter21,jupiter22,jupiter23,jupiter24,jupiter25,jupiter26,jupiter27,jupiter28,jupiter29,jupiter30,jupiter31,jupiter32,jupiter33,jupiter34,jupiter35

MPICC = /opt/NECmpi/gcc/inst/bin/mpicc
MPICFLAGS = -Wall -Wextra -pedantic -std=c99 -D_GNU_SOURCE -I $(COMMON)

bake:
	mkdir -p build
	cd build; cmake -DCMAKE_BUILD_TYPE="Debug" ..; make -j3

bake: bake-nec

bake-nec:
	mkdir -p build
	$(MPICC) $(MPICFLAGS) $(COMMON)/csv.c bench.c arrayscan.c arrayscan_ref.c \
		-o build/arrayscan_ref_bench
	$(MPICC) $(MPICFLAGS) $(COMMON)/csv.c bench.c arrayscan.c arrayscan_par.c \
		exscan_mpi.c -o build/arrayscan_par_mpi_bench
	$(MPICC) $(MPICFLAGS) $(COMMON)/csv.c bench.c arrayscan.c arrayscan_par.c \
		exscan_hillis-steele.c -o build/arrayscan_par_hillis-steele_bench

test: bake
	cd build; CK_FORK=no ctest --output-on-failure

bench: bake
	mkdir -p data
	for inSize in $(INPUT_SIZES); do \
		mpiexec -np 1 build/arrayscan_ref_bench $(CSV_FILE) $${inSize} $(SEED); \
		for nThreads in $(NUM_THREADS); do \
			mpiexec -host $(HOSTS) -np $${nThreads} -loadbalance \
				build/arrayscan_par_mpi_bench $(CSV_FILE) \
				$${inSize} $(SEED); \
			mpiexec -host $(HOSTS) -np $${nThreads} -loadbalance \
				-np $${nThreads} build/arrayscan_par_hillis-steele_bench $(CSV_FILE) $${inSize} $(SEED); \
		done; \
	done

bench-nec: bake-nec
	mkdir -p data
	for inSize in $(INPUT_SIZES); do \
		mpiexec -np 1 build/arrayscan_ref_bench $(CSV_FILE) $${inSize} $(SEED); \
		for nThreads in $(NUM_THREADS); do \
			MPILX_NODELIST="$(HOSTS)" mpiexec -node 0-35 -n $${nThreads} \
				build/arrayscan_par_mpi_bench $(CSV_FILE) \
				$${inSize} $(SEED); \
			MPILX_NODELIST="$(HOSTS)" mpiexec -node 0-35 -n $${nThreads} \
				build/arrayscan_par_hillis-steele_bench $(CSV_FILE) $${inSize} $(SEED); \
		done; \
	done

hand-in:
	$(RSYNC) $(RFLAGS) $(SOURCE) $(HOST):$(REMOTEDIR)

remote-bench: hand-in
	$(SSH) $(SFLAGS) $(HOST) 'cd $(REMOTEDIR); PATH=/opt/cmake/bin/:/opt/openmpi/bin/:${PATH} \
		MPI_HOME=/opt/openmpi make NUM_THREADS="$(NUM_THREADS_REMOTE)" clean bench'
	mkdir -p data
	$(SCP) $(SFLAGS) $(HOST):$(REMOTEDIR)/$(CSV_FILE) $(CSV_FILE)

remote-bench-nec: hand-in
	$(SSH) $(SFLAGS) $(HOST) 'cd $(REMOTEDIR); PATH=/opt/NECmpi/gcc/inst/bin/:${PATH} \
		MPI_HOME=/opt/NECmpi make NUM_THREADS="$(NUM_THREADS_REMOTE)" clean bench-nec'
	mkdir -p data
	$(SCP) $(SFLAGS) $(HOST):$(REMOTEDIR)/$(CSV_FILE) $(CSV_FILE)

clean:
	rm -rf build
