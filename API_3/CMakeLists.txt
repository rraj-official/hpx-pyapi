cmake_minimum_required(VERSION 3.15)
project(hpx_python_bindings LANGUAGES CXX)

# Required for Python development and interpreter functionality
find_package(Python3 REQUIRED COMPONENTS Development Interpreter)

# Get pybind11 library for creating Python bindings
find_package(pybind11 REQUIRED)

# Find HPX high-performance computing library
find_package(HPX REQUIRED)

# Create a shared library module from our C++ binding code
add_library(hpx_bindings MODULE binding.cpp)
target_link_libraries(hpx_bindings PRIVATE pybind11::module HPX::hpx)

# Configure build properties for the module
set_target_properties(hpx_bindings PROPERTIES
    CXX_STANDARD 17
    OUTPUT_NAME "hpx_bindings"
    PREFIX ""   # Ensures correct naming convention for Python modules
)

# cmake_minimum_required(VERSION 3.15)
# project(hpx_bindings LANGUAGES CXX)

# set(CMAKE_CXX_STANDARD 17)
# set(CMAKE_CXX_STANDARD_REQUIRED ON)

# find_package(HPX REQUIRED CONFIG)
# find_package(pybind11 REQUIRED)

# pybind11_add_module(hpx_bindings binding.cpp)
# target_link_libraries(hpx_bindings PRIVATE HPX::hpx)






