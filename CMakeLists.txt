cmake_minimum_required(VERSION 3.14)
project(symhpx_codegen)

find_package(Python3 COMPONENTS Interpreter Development REQUIRED)
find_package(HPX REQUIRED)                  # core is enough

execute_process(
  COMMAND ${Python3_EXECUTABLE} -c "import numpy,sys;sys.stdout.write(numpy.get_include())"
  OUTPUT_VARIABLE NUMPY_INCLUDE_DIR
  OUTPUT_STRIP_TRAILING_WHITESPACE
)

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${Python3_INCLUDE_DIRS}
  ${NUMPY_INCLUDE_DIR}
)

add_library(symhpx SHARED
    src/bindings.cpp
    a_bc.cpp
)

target_link_libraries(symhpx PRIVATE
    HPX::hpx
    ${Python3_LIBRARIES}
)

set_target_properties(symhpx PROPERTIES
    PREFIX ""
    OUTPUT_NAME "symhpx"
)
