cmake_minimum_required(VERSION 3.0.0)

project(SSE)

# set (CMAKE_CMAKE_CXX_FLAGS "${CMAKE_CMAKE_CXX_FLAGS} -D  _CBLAS=1 -Wall -Werror =std=c++11  -llapack -lblas  -lhdf5_cpp -lhdf5")
set (CMAKE_CMAKE_CXX_FLAGS "${CMAKE_CMAKE_CXX_FLAGS} -D  -Wall -Werror =std=c++11 -lhdf5_cpp -lhdf5")


set (source_dir "${PROJECT_SOURCE_DIR}/src/")

file (GLOB source_files "${source_dir}/*.cpp")

add_executable (SSE ${source_files})
target_include_directories( SSE PRIVATE "${PROJECT_SOURCE_DIR}/headers/")

find_package(HDF5 REQUIRED)
include_directories(${HDF5_INCLUDE_DIRS})
target_link_libraries(SSE ${HDF5_LIBRARIES})

