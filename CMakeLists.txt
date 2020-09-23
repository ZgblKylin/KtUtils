# Setup CMake
cmake_minimum_required(VERSION 3.7.8)
project(KtUtils LANGUAGES C CXX)
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}/bin)
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/bin)
SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}/lib)
SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/lib)
set(CMAKE_DEBUG_POSTFIX d)
set(CMAKE_CXX_STANDARD 11)

# Setup detailed warning
if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
  add_compile_options(/MP /W4 /wd4996 /wd4251)
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
find_package(Qt5
  COMPONENTS
    Core
    Gui
    Svg
  REQUIRED
)



# Setup target
option(KT_UTILS_SHARED_LIBRARY "Build/link shared library" OFF)
if(KT_UTILS_SHARED_LIBRARY)
  add_library(${PROJECT_NAME} SHARED)
  target_compile_definitions(${PROJECT_NAME}
    PUBLIC
      KT_UTILS_SHARED_LIBRARY
    PRIVATE
      KT_UTILS_BUILD_SHARED_LIBRARY
  )
else()
  add_library(${PROJECT_NAME} STATIC)
endif()

target_precompile_headers(${PROJECT_NAME}
  PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/include/KtUtils/global.h
)

target_sources(${PROJECT_NAME}
  PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/include/KtUtils/global.h
    ${CMAKE_CURRENT_LIST_DIR}/src/global.cpp

    ${CMAKE_CURRENT_LIST_DIR}/include/KtUtils/iconhelper.h
    ${CMAKE_CURRENT_LIST_DIR}/src/iconhelper.cpp

    ${CMAKE_CURRENT_LIST_DIR}/KtUtils.qrc
)

target_link_libraries(${PROJECT_NAME}
  PUBLIC
    Qt5::Core
    Qt5::Gui
    Qt5::Svg
)

target_include_directories(${PROJECT_NAME}
  PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/include
)



# Setup test
option(BUILD_TESTING "Build test" OFF)
if (BUILD_TESTING)
  add_subdirectory(test)
  enable_testing()
  add_test(NAME TestGlobal COMMAND TestGlobal)
  add_test(NAME TestAsyncInvoker COMMAND TestAsyncInvoker)
endif()
