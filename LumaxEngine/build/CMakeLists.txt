
cmake_minimum_required(VERSION 2.8)

project(LumaxEngine)

set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR})
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})

#set(CMAKE_C_COMPILER ccache)
#set(CMAKE_CXX_COMPILER ccache)

SET(FLAGS "-I/usr/local/Cellar/LuaBridge/ -I/usr/local/Cellar/assimp/4.1.0/include/ -I/usr/local/Cellar/lua/5.3.5_1/include/lua/ -I/usr/local/Cellar/boost/1.65.0/include -isystem/usr/local/include/reactphysics3d/ -std=c++1z -Wall -Wno-unused-function -Wno-unused-variable -Wno-deprecated-declarations -g")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${FLAGS}")

# No idea what that does, seems useful
include_directories(${PROJECT_SOURCE_DIR})

file(GLOB SOURCES "../*.cpp" "../GL/*.cpp" "../PL/*.cpp" "../RAL/*.cpp" "../RAL/windows/*.cpp" "../RL/*.cpp" "../RL/animations/*.cpp" "../RL/lights/*.cpp" "../RL/models3D/*.cpp" "../RL/models2D/*.cpp" "../RL/shaders/*.cpp" "../RL/textures/*.cpp" "../RML/*.cpp" "../RML/loaders/*.cpp" "../core/*.cpp" "../levels/*.cpp" "../utils/*.cpp" "../math/*.cpp" "../TL/*.cpp")

# Libraries paths
link_directories(/usr/local/Cellar/boost/1.65.0/lib/)
link_directories(/usr/local/Cellar/lua/5.3.4_3/lib/)
link_directories(/usr/local/Cellar/assimp/4.1.0/lib/)
link_directories(/usr/local/lib/)
find_package(OpenGL)

add_executable(LumaxEngine ${SOURCES})

# Linking with libraries
target_link_libraries(LumaxEngine glfw3)
target_link_libraries(LumaxEngine boost_system boost_filesystem)
target_link_libraries(LumaxEngine lua.5.3)
target_link_libraries(LumaxEngine assimp)
target_link_libraries(LumaxEngine png)
target_link_libraries(LumaxEngine reactphysics3d)

# Opengl dependency linking
target_link_libraries(LumaxEngine ${OPENGL_LIBRARIES})
