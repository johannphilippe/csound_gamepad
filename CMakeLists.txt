cmake_minimum_required(VERSION 3.5)

project(gamepad LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake/Modules/)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

if(${CMAKE_BUILD_TYPE} MATCHES "Release")
    message(STATUS "Release")
    #add_compile_definitions($<$<CONFIG:Release>:_ITERATOR_DEBUG_LEVEL=0>)
else()
    message(STATUS "Debug")
    #add_compile_definitions($<$<CONFIG:Debug>:_ITERATOR_DEBUG_LEVEL=1>)

endif()

set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set( CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin )

###########################################################

set(GAMEPAD_ENABLE_TESTS OFF CACHE BOOL "disable tests")
set(GAMEPAD_ENABLE_STATIC ON CACHE BOOL "enable static")
set(GAMEPAD_ENABLE_SHARED OFF CACHE BOOL "disable shared")


add_subdirectory(libgamepad)
target_compile_options(gamepad_static PRIVATE -fPIC)



# switch RPATH use off
set(CMAKE_MACOSX_RPATH 0)
# force make to print the command lines
set(CMAKE_VERBOSE_MAKEFILE on)
# path to Csound cmake module
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH}
"${CMAKE_SOURCE_DIR}/../cmake/Modules/")
# set C++ compilation flags
set(CMAKE_CXX_FLAGS  ${CMAKE_CXX_FLAGS}  "-std=c++17")

# Csound opcode build
find_package(Csound REQUIRED)

function(make_plugin pluginlib srcs)
  if(APPLE)
        add_library(${pluginlib} SHARED ${srcs})
    else()
        add_library(${pluginlib} MODULE ${srcs})
  endif()
  target_include_directories(${pluginlib} PRIVATE ${CSOUND_INCLUDE_DIRS} libgamepad/include)
  target_link_libraries(${pluginlib} PUBLIC gamepad_static)
  target_compile_options(${pluginlib} PRIVATE -O3)
endfunction(make_plugin)

message(STATUS "Building Opcodes")
make_plugin(csound_gamepad "src/csound_gamepad.cpp")
#target_link_directories(csound_gamepad PUBLIC gamepad_static)

