cmake_minimum_required(VERSION 3.19 FATAL_ERROR)
project(exmp VERSION 0.0.0.1 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(pybind11 REQUIRED)
find_package(PythonLibs REQUIRED)

add_subdirectory(gen)
include_directories("${CMAKE_CURRENT_LIST_DIR}/gen" "${CMAKE_BINARY_DIR}")
include(GenerateExportHeader)

pybind11_add_module(external_module SHARED exmp_external_module.cpp)
target_link_libraries(external_module PRIVATE "interface_external_module_1_1")
target_compile_definitions(external_module PRIVATE -DEXTERNAL_MODULE_BUILDING)

pybind11_add_module(combine_module SHARED exmp_combine_module.cpp)
target_link_libraries(combine_module PRIVATE external_module "interface_combine_module_2_0")
target_compile_definitions(external_module PRIVATE -DCOMBINE_MODULE_BUILDING)

pybind11_add_module(mod SHARED exmp_mod.cpp)
target_link_libraries(mod PRIVATE "interface_mod_1_0")
target_include_directories(mod PRIVATE ${PYTHON_INCLUDE_DIRS})
