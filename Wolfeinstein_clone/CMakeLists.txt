
cmake_minimum_required(VERSION 2.8)

project(Wolfeinstein_clone)

set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR})
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})

#set(CMAKE_C_COMPILER ccache)
#set(CMAKE_CXX_COMPILER ccache)

SET(FLAGS "-I/usr/local/Cellar/LuaBridge/ -I/usr/local/Cellar/assimp/4.1.0/include/ -L/usr/local/Cellar/assimp/4.1.0/lib/ -I/usr/local/Cellar/lua/5.3.4_3/include/ -I/usr/local/Cellar/libnoise/src/noise/ -I/usr/local/Cellar/libnoise/noiseutils/ -L/usr/local/Cellar/lua/5.3.4_3/lib/ -I/usr/local/Cellar/libpng/1.6.32/include/ -lpng -I/usr/local/Cellar/boost/1.65.0/include -L/usr/local/Cellar/boost/1.65.0/lib/ -lassimp -llua.5.3 -lboost_filesystem -lboost_system -lnoise -std=c++1z -framework OpenGL -lglfw3 -Wall -Wno-unused-function -Wno-unused-variable -g")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${FLAGS}")

# No idea what that does, seems useful
include_directories(${PROJECT_SOURCE_DIR})

file(GLOB SOURCES "*.cpp" "TL/*.cpp" "GL/*.cpp" "PL/*.cpp" "RAL/*.cpp" "RAL/windows/*.cpp" "RL/*.cpp" "RL/animations/*.cpp" "RL/lights/*.cpp" "RL/models3D/*.cpp" "RL/models2D/*.cpp" "RL/shaders/*.cpp" "RL/textures/*.cpp" "RML/*.cpp" "RML/loaders/*.cpp" "core/*.cpp" "levels/*.cpp" "utils/*.cpp" "math/*.cpp")

add_executable(LumaxEngine ${SOURCES})
