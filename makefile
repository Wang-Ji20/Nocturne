.PHONY: cross
.PHONY: build
.PHONY: clean
CMAKE_TOOLCHAIN_DIR=../build_support/cmake
CORTEX_A7_TOOLCHAIN=$(CMAKE_TOOLCHAIN_DIR)/cortexA7.cmake

build:
	mkdir -p build && cd build && cmake .. && make

cross:
	mkdir -p crossbuild && cd crossbuild && cmake -DCMAKE_TOOLCHAIN_FILE=$(CORTEX_A7_TOOLCHAIN) .. && make

clean:
	rm -rf build
	rm -rf crossbuild