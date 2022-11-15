# Setup CMake
cmake_minimum_required(VERSION 3.15)
project(KtUtils LANGUAGES C CXX)
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
set(CMAKE_DEBUG_POSTFIX d)
set(CMAKE_CXX_STANDARD 14)

# Setup static build option
option(BUILD_SHARED_LIBS "Build/link shared library" OFF)
option(BUILD_TESTING "Build test" OFF)
option(BUILD_EXAMPLES "Build examples" OFF)



# Setup Qt
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
# set(CMAKE_INCLUDE_CURRENT_DIR ON)  # if(CMAKE_VERSION VERSION_LESS "3.7.0")
find_package(Qt5
  COMPONENTS
    Core
    Gui
    Svg
  REQUIRED
)
if(CMAKE_SYSTEM_NAME MATCHES "Linux")
  find_package(Qt5 COMPONENTS X11Extras REQUIRED)
endif()



# Setup target
if(BUILD_SHARED_LIBS)
  add_library(${PROJECT_NAME} SHARED)
  target_compile_definitions(${PROJECT_NAME}
    INTERFACE
      KTUTILS_SHARED_LIBRARY
    PRIVATE
      KTUTILS_BUILD_SHARED_LIBRARY
  )
else()
  add_library(${PROJECT_NAME} STATIC)
endif()

target_include_directories(${PROJECT_NAME}
  PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/include
)

target_precompile_headers(${PROJECT_NAME}
  INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/include/KtUtils/KtUtils.hpp
  PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/include/KtUtils/Global.hpp
)

target_sources(${PROJECT_NAME}
  PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/include/KtUtils/KtUtils.hpp

    ${CMAKE_CURRENT_LIST_DIR}/include/KtUtils/Global.hpp
    ${CMAKE_CURRENT_LIST_DIR}/src/Global.cpp

    ${CMAKE_CURRENT_LIST_DIR}/include/KtUtils/AnchorWidget.hpp
    ${CMAKE_CURRENT_LIST_DIR}/src/AnchorWidget.cpp

    ${CMAKE_CURRENT_LIST_DIR}/include/KtUtils/IconHelper.hpp
    ${CMAKE_CURRENT_LIST_DIR}/src/IconHelper.cpp

    ${CMAKE_CURRENT_LIST_DIR}/include/KtUtils/Json.hpp
    ${CMAKE_CURRENT_LIST_DIR}/src/Json.cpp

    ${CMAKE_CURRENT_LIST_DIR}/include/KtUtils/Settings.hpp
    ${CMAKE_CURRENT_LIST_DIR}/src/Settings_p.hpp
    ${CMAKE_CURRENT_LIST_DIR}/src/Settings.cpp

    ${CMAKE_CURRENT_LIST_DIR}/KtUtils.qrc
)

target_link_libraries(${PROJECT_NAME}
  PUBLIC
    Qt5::Core
    Qt5::Gui
    Qt5::Svg
    $<$<PLATFORM_ID:Linux>:Qt5::X11Extras>
    $<$<PLATFORM_ID:Linux>:X11>
)

target_compile_options(${PROJECT_NAME}
  PUBLIC
    $<$<CXX_COMPILER_ID:MSVC>:/W3>
    $<$<CXX_COMPILER_ID:MSVC>:/wd4996>
    $<$<CXX_COMPILER_ID:MSVC>:/wd4251>
    $<$<OR:$<CXX_COMPILER_ID:GNU>,$<PLATFORM_ID:Linux>>:$<$<COMPILE_LANGUAGE:C>:-Wall>>
    $<$<OR:$<CXX_COMPILER_ID:GNU>,$<PLATFORM_ID:Linux>>:$<$<COMPILE_LANGUAGE:CXX>:-Wall>>
    $<$<OR:$<CXX_COMPILER_ID:GNU>,$<PLATFORM_ID:Linux>>:$<$<COMPILE_LANGUAGE:C>:-Wextra>>
    $<$<OR:$<CXX_COMPILER_ID:GNU>,$<PLATFORM_ID:Linux>>:$<$<COMPILE_LANGUAGE:CXX>:-Wextra>>
    $<$<OR:$<CXX_COMPILER_ID:GNU>,$<PLATFORM_ID:Linux>>:$<$<COMPILE_LANGUAGE:C>:-pedantic>>
    $<$<OR:$<CXX_COMPILER_ID:GNU>,$<PLATFORM_ID:Linux>>:$<$<COMPILE_LANGUAGE:CXX>:-pedantic>>
    $<$<OR:$<CXX_COMPILER_ID:GNU>,$<PLATFORM_ID:Linux>>:$<$<COMPILE_LANGUAGE:CXX>:-Weffc++>>
)



# Setup test
if(BUILD_TESTING)
  enable_testing()
  add_subdirectory(test)
  add_test(NAME TestGlobal COMMAND TestGlobal)
endif()



# Build example
if(BUILD_EXAMPLES)
  add_subdirectory(examples)
endif()
