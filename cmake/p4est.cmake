include(ExternalProject)

set(p4est_external true CACHE BOOL "build p4est library")

find_package(Git REQUIRED)
execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive)

set(p4est_ROOT ${PROJECT_BINARY_DIR}/p4est)

set(p4est_sc_LIBRARY ${p4est_ROOT}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}sc${CMAKE_STATIC_LIBRARY_SUFFIX} CACHE FILEPATH "sc library" FORCE)
set(p4est_LIBRARY ${p4est_ROOT}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}p4est${CMAKE_STATIC_LIBRARY_SUFFIX} CACHE FILEPATH "p4est library" FORCE)

set(p4est_flags --prefix=${p4est_ROOT})
set(p4est_mpi)
if(MPI_C_FOUND)
  set(p4est_mpi --enable-mpi)
endif()

ExternalProject_Add(p4est
# GIT_REPOSITORY https://github.com/cburstedde/p4est.git
# GIT_TAG prev3-develop
# # v2.2 requires BLAS and has problems detecting gfortran on MacOS
SOURCE_DIR ${PROJECT_SOURCE_DIR}/p4est
CONFIGURE_COMMAND ${PROJECT_SOURCE_DIR}/p4est/configure ${p4est_flags} ${p4est_mpi}
BUILD_COMMAND make -j${Ncpu}
INSTALL_COMMAND make -j${Ncpu} install
TEST_COMMAND ""
BUILD_BYPRODUCTS ${p4est_LIBRARY} ${p4est_sc_LIBRARY}
)

ExternalProject_Get_Property(p4est SOURCE_DIR)

ExternalProject_Add_Step(p4est
  bootstrap
  COMMAND ./bootstrap
  DEPENDEES download
  DEPENDERS configure
  WORKING_DIRECTORY ${SOURCE_DIR})

set(p4est_INCLUDE_DIRS ${p4est_ROOT}/include)
set(p4est_LIBRARIES ${p4est_LIBRARY} ${p4est_sc_LIBRARY})

# --- imported target
file(MAKE_DIRECTORY ${p4est_ROOT}/include)  # avoid race condition

add_library(p4est::p4est INTERFACE IMPORTED GLOBAL)
target_include_directories(p4est::p4est INTERFACE ${p4est_INCLUDE_DIRS})
target_link_libraries(p4est::p4est INTERFACE "${p4est_LIBRARIES}")  # need the quotes to expand list
# set_target_properties didn't work, but target_link_libraries did work

add_dependencies(p4est::p4est p4est)
