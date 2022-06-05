# PROJECT NAME
project(app)

# CMAKE VERSION
cmake_minimum_required(VERSION "3.22")

# C FLAGS
set(CMAKE_C_FLAGS "-std=c17 -Wextra -Wall -Wpedantic")

# INCLUDE PATHS
include_directories("include")

# HEADER FILES
file(GLOB_RECURSE H_FILES "include/*.h")

# C SOURCE FILES
file(GLOB_RECURSE C_FILES "src/*.c")

# LIBRARY FILES
file(GLOB_RECURSE LIB_FILES "lib/*.a")

# LINKED LIST LIB
file(GLOB_RECURSE LINKED_LIST_FILES "src/linked_list/*.c")
add_library(linked_list STATIC ${LINKED_LIST_FILES})

# EXECUTABLE NAME
set(APP "test")

# FILES OUTPUT PATHS
set(EXECUTABLE_OUTPUT_PATH "..")

# APP EXECUTABLE
add_executable(${APP} ${C_FILES})

# LINKED CONFIGURATION
target_link_libraries(${APP} ${LIB_FILES})
