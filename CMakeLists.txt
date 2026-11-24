cmake_minimum_required(VERSION 4.5)
project(linked_list LANGUAGES C)

# C23 STRICT: NO GNU EXTENSIONS, FAIL IF THE COMPILER DOESN'T SUPPORT IT
set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

# FILES (CONFIGURE_DEPENDS: NEW FILES ARE PICKED UP WITHOUT RE-RUNNING CMAKE)
file(GLOB_RECURSE SOURCE_FILES CONFIGURE_DEPENDS "src/*.c")
file(GLOB_RECURSE TEST_FILES CONFIGURE_DEPENDS "test/*.c")

# WARNINGS FOR EVERY TARGET (MUST BE BEFORE add_library / add_executable)
if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# RELEASE BY DEFAULT (OVERRIDE WITH -DCMAKE_BUILD_TYPE=Debug)
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# LIBRARY: liblinked_list.a
add_library(linked_list STATIC ${SOURCE_FILES})
target_include_directories(linked_list PUBLIC include)

# TEST PROGRAM: test.exe, USES THE LIBRARY
add_executable(test ${TEST_FILES})
target_link_libraries(test PRIVATE linked_list)

# PUT test.exe IN THE PROJECT ROOT INSTEAD OF build/
set_target_properties(test PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}"
)
