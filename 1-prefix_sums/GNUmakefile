RSYNC = rsync
SSH = ssh

SFLAGS = -F "../.ssh-cfg"
RFLAGS = --recursive --progress --copy-links --exclude=build/ -e "$(SSH) $(SFLAGS)"

HOST = saturn
REMOTEDIR = ~/project1
SOURCE = .

ALGORITHMS = recursive iterative hillis-steele
INPUT_SIZES = 10 100 1000000 100000000
NUM_THREADS = 1 2 4 0 # 0 means use the default
NUM_THREADS_REMOTE = 8 16 32 0

bake:
	mkdir -p build
	cd build; cmake -DCMAKE_BUILD_TYPE="Debug" ..; make -j3

test: bake
	cd build; ctest --output-on-failure

bench: bake
	for nThreads in $(NUM_THREADS); do \
		if [ $${nThreads} -eq 0 ]; then \
			for alg in $(ALGORITHMS); do \
				build/$${alg}_bench $(INPUT_SIZES); \
			done; \
			continue; \
		fi; \
		for alg in $(ALGORITHMS); do \
			OMP_NUM_THREADS=$${nThreads} build/$${alg}_bench $(INPUT_SIZES); \
		done; \
	done

hand-in:
	$(RSYNC) $(RFLAGS) $(SOURCE) $(HOST):$(REMOTEDIR)

remote-bench: hand-in
	$(SSH) $(SFLAGS) $(HOST) 'cd $(REMOTEDIR); make NUM_THREADS="$(NUM_THREADS_REMOTE)" clean bench'

clean:
	rm -rf build


