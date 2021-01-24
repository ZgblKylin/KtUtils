# Setup CMake
cmake_minimum_required(VERSION 3.7.8)
project(KtUtils LANGUAGES C CXX)
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
set(CMAKE_DEBUG_POSTFIX d)
set(CMAKE_CXX_STANDARD 11)

# Setup detailed warning
if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
  add_compile_options(/MP /wd4996 /wd4251)
  string(REPLACE "/W3" "" CMAKE_C_FLAGS ${CMAKE_C_FLAGS})
  string(REPLACE "/W3" "" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
  if(CMAKE_BUILD_TYPE MATCHES "Debug")
    # Not find pdb file while linking debug dll
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /ignore:4099")
  endif()
else() # gcc clang
  add_compile_options(-Wall -Wextra -pedantic -Weffc++)
  add_compile_options(-Wno-gnu-zero-variadic-macro-arguments)
endif()



# Setup Qt
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)  # if(CMAKE_VERSION VERSION_LESS "3.7.0")
if(NOT TARGET Qt::Core)
  find_package(Qt5
    COMPONENTS
      Core
      Gui
      Svg
    REQUIRED
  )
endif()



# Setup target
option(BUILD_SHARED_LIBS "Build/link shared library" OFF)
if(BUILD_SHARED_LIBS)
  add_library(${PROJECT_NAME} SHARED)
  target_compile_definitions(${PROJECT_NAME}
    PUBLIC
      KTUTILS_SHARED_LIBRARY
    PRIVATE
      KTUTILS_BUILD_SHARED_LIBRARY
  )
else()
  add_library(${PROJECT_NAME} STATIC)
endif()

target_precompile_headers(${PROJECT_NAME}
  PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/include/KtUtils/global.hpp
)

target_sources(${PROJECT_NAME}
  PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/include/KtUtils/ktutils.hpp

    ${CMAKE_CURRENT_LIST_DIR}/include/KtUtils/global.hpp
    ${CMAKE_CURRENT_LIST_DIR}/src/global.cpp

    ${CMAKE_CURRENT_LIST_DIR}/include/KtUtils/iconhelper.hpp
    ${CMAKE_CURRENT_LIST_DIR}/src/iconhelper.cpp

    ${CMAKE_CURRENT_LIST_DIR}/include/KtUtils/settings.hpp
    ${CMAKE_CURRENT_LIST_DIR}/src/settings.cpp

    ${CMAKE_CURRENT_LIST_DIR}/KtUtils.qrc
)

target_link_libraries(${PROJECT_NAME}
  PUBLIC
    Qt::Core
    Qt::Gui
    Qt::Svg
)

target_include_directories(${PROJECT_NAME}
  PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/include
)



# Setup test
option(BUILD_TESTING "Build test" OFF)
if (BUILD_TESTING)
  enable_testing()
  add_subdirectory(test)
  add_test(NAME TestGlobal COMMAND TestGlobal)
endif()
