
add_library(src STATIC
        Runtime.cpp
        memory/GarbageCollector.cpp
        #        memory/GarbageCollector.hpp
        memory/alloca/Allocator.cpp
        #        memory/alloca/Allocator.hpp
        memory/alloca/Region.cpp
        #        memory/alloca/Region.hpp
        #        memory/alloca/Arena.hpp
        memory/alloca/heap/Heap.cpp
        #        memory/alloca/heap/Heap.hpp
        safepoints/Safepoint.cpp
        #        safepoints/Safepoint.hpp
        system/System.cpp
        #        system/System.hpp
)

target_include_directories(src PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})


