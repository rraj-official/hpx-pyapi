cmake_minimum_required(VERSION 3.15)
project(hpx_python_bindings LANGUAGES CXX)

# 1) Find Python with Interpreter component
find_package(Python3 REQUIRED COMPONENTS Development Interpreter)

# 2) Find pybind11
# If not found automatically, set pybind11_DIR or specify an include path.
find_package(pybind11 REQUIRED)

# 3) Find HPX
find_package(HPX REQUIRED)

# 4) Build the module
add_library(hpx_bindings MODULE binding.cpp)
target_link_libraries(hpx_bindings PRIVATE pybind11::module HPX::hpx)

# 5) Output library name
set_target_properties(hpx_bindings PROPERTIES
    CXX_STANDARD 17
    OUTPUT_NAME "hpx_bindings"
    PREFIX ""   # ensures the module is named 'hpx_bindings.so' on WSL
)
