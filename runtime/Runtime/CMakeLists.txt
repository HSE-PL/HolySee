cmake_minimum_required(VERSION 3.29)
#set(CMAKE_C_COMPILER clang)
#set(CMAKE_CXX_COMPILER clang++)
project(Runtime)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_POSITION_INDEPENDENT_CODE OFF)

enable_language(ASM_NASM)

add_subdirectory(src)
set(CMAKE_POSITION_INDEPENDENT_CODE OFF)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0")
set(CMAKE_ASM_NASM_FLAGS "-f elf64 -g -F dwarf")
set_source_files_properties(
        main.asm
        PROPERTIES
        COMPILE_FLAGS "-g -F dwarf"
)
add_executable(Runtime)
target_sources(Runtime PRIVATE main.asm)
target_link_libraries(Runtime PRIVATE src)

target_link_options(Runtime PRIVATE
        "-nostartfiles"
        "-no-pie"
        "-Wl,-e,start"
        "-lstdc++"
        "-lc"
        "-Wl,--gc-sections"
        "-O0"
        "-g"
)


