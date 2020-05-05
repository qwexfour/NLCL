cmake_minimum_required(VERSION 3.0)
project(NLCL)

add_library(NLCL forest.cpp)
add_executable(testForest main.cpp)

target_compile_features(NLCL PUBLIC cxx_std_17)
target_compile_options(NLCL PRIVATE -Wall -pedantic-errors)

target_link_libraries(testForest NLCL)
