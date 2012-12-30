RSYNC = rsync
SSH = ssh

SFLAGS = -F "../.ssh-cfg"
RFLAGS = --recursive --progress --copy-links --exclude=build/ -e "$(SSH) $(SFLAGS)"

HOST = saturn
REMOTEDIR = ~/project1
SOURCE = .

ALGORITHMS = recursive iterative hillis-steele
INPUT_SIZES = 10 100 1000000 100000000

bake:
	mkdir -p build
	cd build; cmake -DCMAKE_BUILD_TYPE="Debug" ..; make -j3

test: bake
	cd build; ctest --output-on-failure

bench: bake
	for alg in $(ALGORITHMS); do build/$${alg}_bench $(INPUT_SIZES); done

hand-in:
	$(RSYNC) $(RFLAGS) $(SOURCE) $(HOST):$(REMOTEDIR)

remote-bench: hand-in
	$(SSH) $(SFLAGS) $(HOST) 'cd $(REMOTEDIR); make clean bench'

clean:
	rm -rf build


