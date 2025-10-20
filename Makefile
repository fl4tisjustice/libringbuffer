.PHONY: all release debug clean

release:
	cmake --preset Release . && cmake --build . --preset Release

debug:
	cmake --preset Debug . && cmake --build . --preset Debug

all: release debug

clean:
	cmake --build . --preset Debug --target clean
	cmake --build . --preset Release --target clean