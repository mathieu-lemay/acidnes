default_rom := "tests/nestest.nes"

run rom=default_rom: cmake
	cd build && make acidnes
	./build/acidnes "{{rom}}"

test: cmake
	cd build && make tests
	./tests/nestest.sh

cmake:
	[[ ! -f build/Makefile || build/Makefile -ot CMakeLists.txt ]] && ( mkdir -p build && cd build && cmake .. ) || true

clean:
	rm -f build

# vim: ft=make
