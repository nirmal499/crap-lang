project-configure-fresh:
	rm -fr build
	mkdir build
	cmake -B build -S .

project-configure:
	cmake -B build -S .

project-build:
	cmake --build build

project-build-verbose:
	cmake --build build --clean-first -- VERBOSE=ON

project-run:
	./build/lang/executable lang/main.cpl

project-run-ll:
	lli-14 out.ll