bake:
	mkdir -p build
	cd build; cmake -DCMAKE_BUILD_TYPE="Debug" ..; make -j3

test: bake
	cd build; ctest --output-on-failure

clean:
	rm -r build


