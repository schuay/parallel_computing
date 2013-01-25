RSYNC = rsync
SSH = ssh
SCP = scp

SFLAGS = -F "../.ssh-cfg"
RFLAGS = --recursive --progress --copy-links --exclude=build/ -e "$(SSH) $(SFLAGS)"

HOST = jupiter
REMOTEDIR = ~/project3-bucket_sort

COMMON = $(shell [ -d ../common ] && echo -n "../common" || echo -n "common")
SOURCE = . $(COMMON)

CSV_FILE = data/bench.csv
INPUT_SIZES = 1000 10000 100000 1000000 1111111 10000000 100000000
UPPER_BOUND = 100000
PROCESS_COUNTS = 32 128 256 511 512
HOSTS=jupiter0,jupiter1,jupiter2,jupiter3,jupiter4,jupiter5,jupiter6,jupiter7,jupiter8,jupiter9,jupiter10,jupiter11,jupiter12,jupiter13,jupiter14,jupiter15,jupiter16,jupiter17,jupiter18,jupiter19,jupiter20,jupiter21,jupiter22,jupiter23,jupiter24,jupiter25,jupiter26,jupiter27,jupiter28,jupiter29,jupiter30,jupiter31,jupiter32,jupiter33,jupiter34,jupiter35

MPICC = /opt/NECmpi/gcc/inst/bin/mpicc
MPICFLAGS = -Wall -Wextra -pedantic -std=c99 -D_GNU_SOURCE -I $(COMMON)

bake:
	mkdir -p build
	cd build; cmake -DCMAKE_BUILD_TYPE="Debug" ..; make -j3

bake-nec:
	mkdir -p build
	$(MPICC) $(MPICFLAGS) $(COMMON)/csv.c bucket_ref.c bucket.c \
		-o build/bucket_ref
	$(MPICC) $(MPICFLAGS) $(COMMON)/csv.c bench.c bucket.c \
		-o build/bucket_bench

test: bake
	cd build; CK_FORK=no ctest --output-on-failure

bench: bake
	mkdir -p data
	for SIZE in $(INPUT_SIZES); do \
		build/bucket_ref $(CSV_FILE) $${SIZE} $(UPPER_BOUND); \
		for P in $(PROCESS_COUNTS); do \
			mpiexec -host $(HOSTS) -loadbalance -np $${P} \
				build/bucket_bench $(CSV_FILE) $${SIZE} $(UPPER_BOUND); \
		done \
	done

bench-nec: bake-nec
	mkdir -p data
	for SIZE in $(INPUT_SIZES); do \
		mpiexec -np 1 build/bucket_ref $(CSV_FILE) $${SIZE} $(UPPER_BOUND); \
		for P in $(PROCESS_COUNTS); do \
			MPILX_NODELIST="$(HOSTS)" mpiexec -node 0-35 -n $${P} \
				build/bucket_bench $(CSV_FILE) $${SIZE} $(UPPER_BOUND); \
		done \
	done

hand-in:
	$(RSYNC) $(RFLAGS) $(SOURCE) $(HOST):$(REMOTEDIR)

remote-bench: hand-in
	$(SSH) $(SFLAGS) $(HOST) 'cd $(REMOTEDIR); PATH=/opt/cmake/bin/:/opt/openmpi/bin/:${PATH} \
		MPI_HOME=/opt/openmpi make clean bench'
	mkdir -p data
	$(SCP) $(SFLAGS) $(HOST):$(REMOTEDIR)/$(CSV_FILE) $(CSV_FILE)

remote-bench-nec: hand-in
	$(SSH) $(SFLAGS) $(HOST) 'cd $(REMOTEDIR); PATH=/opt/NECmpi/gcc/inst/bin/:${PATH} \
		MPI_HOME=/opt/NECmpi make clean bench-nec'
	mkdir -p data
	$(SCP) $(SFLAGS) $(HOST):$(REMOTEDIR)/$(CSV_FILE) $(CSV_FILE)

clean:
	rm -rf build
