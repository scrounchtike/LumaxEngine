
all: new

clean:
	rm -rf *.o
	rm -rf *.out
	rm -rf *.obj
#	rm -r build/CMakeCache.txt
	rm -rf build/CMakeFiles

new:
	cd build;\
	cmake .;\
  make;\
	dsymutil LumaxEngine

run:
	./build/LumaxEngine

debug:
	lldb build/LumaxEngine
#	/usr/local/Cellar/gdb/8.3/bin/gdb /build/LumaxEngine

rebuild: clean new

cloc:
	cloc --force-lang="c++",tpp --force-lang="GLSL",vs --force-lang="GLSL",fs --force-lang="HLSL",ps --exclude-list-file="cloc_excluded.txt" .
