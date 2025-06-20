# Setup CMake
cmake_minimum_required(VERSION 3.15)
project(KtUtils_Example LANGUAGES C CXX)



# Setup Qt
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
# set(CMAKE_INCLUDE_CURRENT_DIR ON)  # if(CMAKE_VERSION VERSION_LESS "3.7.0")
set(QT_LIBS
  Core
  Gui
  Widgets
)
set(QT_MODULES ${QT_LIBS})
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS ${QT_MODULES})
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS ${QT_MODULES})



# Json
add_executable(${PROJECT_NAME}_Json WIN32)
target_sources(${PROJECT_NAME}_Json PRIVATE ${CMAKE_CURRENT_LIST_DIR}/Json.cpp)
foreach(QT_MODULE ${QT_LIBS})
  target_link_libraries(${PROJECT_NAME}_Json PUBLIC
    Qt${QT_VERSION_MAJOR}::${QT_MODULE}
  )
endforeach()
target_link_libraries(${PROJECT_NAME}_Json PUBLIC KtUtils)
