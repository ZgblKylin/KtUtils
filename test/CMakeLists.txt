# Setup CMake
cmake_minimum_required(VERSION 3.7.8)
project(KtUtilsTest LANGUAGES C CXX)



# Setup Qt
find_package(Qt5
  COMPONENTS
    Test
  REQUIRED
)



# Setup target
add_executable(TestGlobal testglobal.hpp testglobal.cpp)
target_link_libraries(TestGlobal Qt5::Test KtUtils)
