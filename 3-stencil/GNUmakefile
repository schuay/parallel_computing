RSYNC = rsync
SSH = ssh
SCP = scp

SFLAGS = -F "../.ssh-cfg"
RFLAGS = --recursive --progress --copy-links --exclude=build/ -e "$(SSH) $(SFLAGS)"

HOST = jupiter
REMOTEDIR = ~/project3-stencil
SOURCE = . ../common

CSV_FILE = data/bench.csv
ITERATION_COUNTS = 10 100 1000
M=1000
N=1000
SEED=5

bake:
	mkdir -p build
	cd build; cmake -DCMAKE_BUILD_TYPE="Debug" ..; make -j3

test: bake
	cd build; CK_FORK=no ctest --output-on-failure

bench: bake
	mkdir -p data
	for ITERS in $(ITERATION_COUNTS); do \
		build/stencil_ref $(CSV_FILE) $(M) $(N) $(SEED) $${ITERS}; \
		mpiexec -np 40 build/stencil_bench $(CSV_FILE) $(M) $(N) 4 10 $(SEED) $${ITERS}; \
	done

hand-in:
	$(RSYNC) $(RFLAGS) $(SOURCE) $(HOST):$(REMOTEDIR)

remote-bench: hand-in
	$(SSH) $(SFLAGS) $(HOST) 'cd $(REMOTEDIR); PATH=/opt/cmake/bin/:/opt/openmpi/bin/:${PATH} \
		MPI_HOME=/opt/openmpi make clean bench'
	mkdir -p data
	$(SCP) $(SFLAGS) $(HOST):$(REMOTEDIR)/$(CSV_FILE) $(CSV_FILE)

clean:
	rm -rf build
	rm -rf data


