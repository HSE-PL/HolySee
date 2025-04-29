
add_library(src STATIC
        Runtime.cpp
        memory/GarbageCollector.cpp
        memory/Allocator.cpp
        memory/alloca/Bitset.cpp
        memory/alloca/heap/Heap.cpp
        safepoints/Safepoint.cpp
        system/System.cpp
        threads/Threads.cpp
        threads/Horoutine.cpp
        memory/ThreadSafeVector.cpp
        memory/alloca/Arena.cpp
        memory/MemoryManager.cpp
        memory/alloca/BitMap.cpp
)

target_include_directories(src PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_options(src PUBLIC -g -O0)
