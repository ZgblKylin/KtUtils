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
add_executable(TestGlobal
  global/testglobal.h
  global/testglobal.cpp)
target_link_libraries(TestGlobal Qt5::Test KtUtils)

add_executable(TestAsyncInvoker
  asyncinvoker/testasyncinvoker.h
  asyncinvoker/testasyncinvoker.cpp)
target_link_libraries(TestAsyncInvoker Qt5::Test KtUtils)

# ${CMAKE_CURRENT_LIST_DIR}/iconhelper.h
# ${CMAKE_CURRENT_LIST_DIR}/iconhelper.cpp
