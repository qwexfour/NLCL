add_executable(testForest forest_test.cpp)

target_compile_features(testForest PUBLIC cxx_std_17)
target_compile_options(testForest PRIVATE -Wall -pedantic-errors)

target_link_libraries(testForest NLCL)
