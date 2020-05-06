add_library(NLCL forest.cpp)

target_compile_features(NLCL PUBLIC cxx_std_17)
target_compile_options(NLCL PRIVATE -Wall -pedantic-errors)
