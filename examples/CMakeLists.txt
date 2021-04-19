# Setup CMake
cmake_minimum_required(VERSION 3.15)
project(KtUtils_Example LANGUAGES C CXX)



# Setup Qt
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
# set(CMAKE_INCLUDE_CURRENT_DIR ON)  # if(CMAKE_VERSION VERSION_LESS "3.7.0")
find_package(Qt5
  COMPONENTS
    Core
    Gui
    Widgets
  REQUIRED
)



# Json
add_executable(${PROJECT_NAME}_Json WIN32)
target_sources(${PROJECT_NAME}_Json PRIVATE ${CMAKE_CURRENT_LIST_DIR}/Json.cpp)
target_link_libraries(${PROJECT_NAME}_Json
  PUBLIC
    Qt5::Core
    Qt5::Gui
    Qt5::Widgets
    KtUtils
)
