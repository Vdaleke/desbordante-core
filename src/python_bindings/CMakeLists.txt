SET(BINDINGS_NAME desbordante)

file(GLOB_RECURSE sources "*.h*" "*.cpp*")
pybind11_add_module(${BINDINGS_NAME} ${sources})
set_target_properties(${BINDINGS_NAME} PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
target_link_libraries(${BINDINGS_NAME} PRIVATE ${CMAKE_PROJECT_NAME} Boost::headers Boost::included_unit_test_framework Boost::container Boost::thread Boost::graph Boost::any Boost::dynamic_bitset 
    Boost::serialization Boost::math Boost::format Boost::align Boost::asio)

include_directories(".")

if (PYTHON STREQUAL INSTALL)
    install(TARGETS ${BINDINGS_NAME} LIBRARY DESTINATION .)
endif()
