
add_library(src STATIC
        Runtime.cpp
        memory/GarbageCollector.cpp
        memory/alloca/Allocator.cpp
        memory/alloca/Bitset.cpp
        memory/alloca/Region.cpp
        memory/alloca/heap/Heap.cpp
        safepoints/Safepoint.cpp
        system/System.cpp
        threads/Threads.cpp
        threads/Horoutine.cpp
        memory/ThreadSafeVector.cpp
        memory/ThreadSafeVector.hpp
        threads/ThreadSafeCounter.cpp
        threads/ThreadSafeCounter.hpp
)

target_include_directories(src PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})


