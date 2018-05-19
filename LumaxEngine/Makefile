
SOURCES = $(wildcard *.cpp GL/*.cpp PL/*.cpp RAL/*.cpp RL/*.cpp RL/lights/*.cpp RML/*.cpp core/*.cpp levels/*.cpp loaders/*.cpp utils/*.cpp math/*.cpp)

HEADERS = $(wildcart *.hpp *.h GL/*.hpp PL/*.hpp RAL/*.hpp RL/*.hpp RL/lights/*.hpp RML/*.hpp core/*.hpp levels/*.hpp loaders/*.hpp math/*.hpp utils/*.hpp utils/*.h)

all: clean make

make: $(SOURCES)
	g++ -I/usr/local/Cellar/LuaBridge/ -I/usr/local/Cellar/assimp/4.1.0/include/ -L/usr/local/Cellar/assimp/4.1.0/lib/ -I/usr/local/Cellar/lua/5.3.4_3/include/ -L/usr/local/Cellar/lua/5.3.4_3/lib/ -I/usr/local/Cellar/libpng/1.6.32/include/ -lpng -I/usr/local/Cellar/boost/1.65.0/include -L/usr/local/Cellar/boost/1.65.0/lib/ -lassimp -llua.5.3 -lboost_filesystem -lboost_system -std=c++1z -framework OpenGL -lglfw3 -Wall -Wno-unused-function -Wno-unused-variable $(SOURCES)

clean:
	rm -rf *.o
	rm -rf *.out
	rm -rf *.obj
