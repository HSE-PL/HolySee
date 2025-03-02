add_library(src STATIC
Runtime.cpp
memory/GarbageCollector.cpp
memory/alloca/Allocator.cpp
memory/alloca/heap/Heap.cpp
        memory/alloca/Region.cpp
        memory/alloca/Region.h
)

target_include_directories(src PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})


