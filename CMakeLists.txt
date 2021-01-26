cmake_minimum_required(VERSION 3.15)
project(ForestCLAW
  LANGUAGES C Fortran
  VERSION 0.1.4880)

list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake/Modules)

cmake_host_system_information(RESULT Ncpu QUERY NUMBER_OF_PHYSICAL_CORES)
message(VERBOSE "CMake ${CMAKE_VERSION} using ${Ncpu} threads")

include(cmake/compilers.cmake)
include(cmake/options.cmake)

find_package(MPI COMPONENTS C Fortran)
find_package(LAPACK)
find_package(BLAS)
find_package(ZLIB REQUIRED)

# --- p4est, sc
if(NOT p4est_external)
  find_package(p4est)
endif()
if(NOT p4est_FOUND)
  include(cmake/p4est.cmake)
endif(NOT p4est_FOUND)


# --- ForestClaw
add_library(forestclaw)
target_include_directories(forestclaw
  PRIVATE src ${PROJECT_BINARY_DIR}/include src/patches/clawpatch)
target_link_libraries(forestclaw PRIVATE p4est::p4est)
if(MPI_FOUND)
  target_link_libraries(forestclaw PRIVATE MPI::MPI_Fortran MPI::MPI_C)
endif()

add_subdirectory(src)
