cmake_minimum_required(VERSION 3.29)
project(Runtime)
#set(CMAKE_BUILD_TYPE Debug)

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

find_package(TBB REQUIRED COMPONENTS tbb)

target_link_libraries(Runtime PRIVATE TBB::tbb)


target_compile_options(Runtime PRIVATE "-g3 -O0 -fsanitize=address -ltbb")
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
        "-g"
)


