cmake_minimum_required(VERSION 3.29)
#set(CMAKE_C_COMPILER clang)
#set(CMAKE_CXX_COMPILER clang++)
project(Runtime)
#if (NOT CMAKE_BUILD_TYPE)
#    set(CMAKE_BUILD_TYPE Debug)
#endif ()

set(IS_DEBUG_BUILD CMAKE_BUILD_TYPE STREQUAL "Debug")
if (${IS_DEBUG_BUILD})
    add_compile_definitions(DEBUG)
endif ()

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


enable_language(ASM_NASM)

add_subdirectory(src)
set(CMAKE_POSITION_INDEPENDENT_CODE OFF)

add_executable(Runtime)
target_sources(Runtime PRIVATE main.asm)
target_link_libraries(Runtime PRIVATE src)
target_compile_options(Runtime PRIVATE "-g -O0")
set(CMAKE_ASM_NASM_FLAGS "-f elf64 -F dwarf")
set_source_files_properties(
        main.asm
        PROPERTIES
        COMPILE_FLAGS "-F dwarf"
)

target_link_options(Runtime PRIVATE
        "-nostartfiles"
        "-no-pie"
        "-Wl,-e,start"
        "-lstdc++"
        "-lc"
        "-Wl,--gc-sections"
)


