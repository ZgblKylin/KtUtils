# Setup CMake
cmake_minimum_required(VERSION 3.7.8)
project(KtUtilsTest LANGUAGES C CXX)


# Setup Qt
set(QT_LIBS Test)
set(QT_MODULES ${QT_LIBS})
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS ${QT_MODULES})
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS ${QT_MODULES})



# Setup target
add_executable(TestGlobal TestGlobal.hpp TestGlobal.cpp)
foreach(QT_MODULE ${QT_LIBS})
  target_link_libraries(TestGlobal Qt${QT_VERSION_MAJOR}::${QT_MODULE})
endforeach()
target_link_libraries(TestGlobal KtUtils)
