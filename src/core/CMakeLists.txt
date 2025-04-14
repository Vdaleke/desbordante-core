set(BINARY ${CMAKE_PROJECT_NAME})

file(GLOB_RECURSE lib_sources "*/*.h*" "*/*.cpp*" "*/*.cc*")

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

add_library(${BINARY} STATIC ${lib_sources})
target_link_libraries(${BINARY} PUBLIC Boost::headers Boost::included_unit_test_framework Boost::container Boost::thread Boost::graph Boost::any Boost::dynamic_bitset 
    Boost::serialization Boost::math Boost::format Boost::align Boost::asio Threads::Threads)
target_link_libraries(${BINARY} PUBLIC easyloggingpp)

option(SAFE_VERTICAL_HASHING
        "Enable safe vertical hashing. This feature allows to process\
         wide (>32 columns) datasets, possibly lowering the performance."
        OFF)
if (SAFE_VERTICAL_HASHING)
    target_compile_definitions(${BINARY} PUBLIC SAFE_VERTICAL_HASHING)
endif(SAFE_VERTICAL_HASHING)
