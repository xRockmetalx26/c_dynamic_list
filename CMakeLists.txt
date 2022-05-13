# PROJECT NAME
project(app)

# CMAKE VERSION
cmake_minimum_required(VERSION "3.22")

# C FLAGS
set(CMAKE_C_FLAGS "-std=c18 -Wextra -Wall -Wpedantic -O3 -DNDEBUG")

# C++ FLAGS
set(CMAKE_CXX_FLAGS "-std=c++2a -Wextra -Wall -Wpedantic -O3 -DNDEBUG")

# INCLUDE PATHS
include_directories("include")

# HEADER FILES
file(GLOB_RECURSE H_FILES "include/*.h" "include/*.hpp")

# C SOURCE FILES
file(GLOB_RECURSE C_FILES "src/*.c")

# C++ SOURCE FILES
file(GLOB_RECURSE CPP_FILES "src/*.cpp" "src/*.cc")

# RESOURCE FILES
file(GLOB_RECURSE RES_FILES "res/*.rc")

# LIBRARY FILES
file(GLOB_RECURSE LIB_FILES "lib/*.a")
file(GLOB_RECURSE LIB_FILES "lib/*.*.a")

# EXECUTABLE NAME
set(APP "test")

# FILES OUTPUT PATHS
set(EXECUTABLE_OUTPUT_PATH "..")
set(LIBRARY_OUTPUT_PATH "lib")

# APP EXECUTABLE
add_executable(${APP} ${C_FILES} ${CPP_FILES} ${RES_FILES})

# LINKED CONFIGURATION
target_link_libraries(${APP} ${LIB_FILES})
