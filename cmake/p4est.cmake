include(ExternalProject)

set(p4est_external true CACHE BOOL "build p4est library")
set(p4est_ROOT ${PROJECT_BINARY_DIR}/p4est CACHE PATH "p4est root directory" FORCE)

find_package(Autotools REQUIRED)

set(p4est_sc_LIBRARY ${p4est_ROOT}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}sc${CMAKE_STATIC_LIBRARY_SUFFIX} CACHE FILEPATH "sc library" FORCE)
set(p4est_LIBRARY ${p4est_ROOT}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}p4est${CMAKE_STATIC_LIBRARY_SUFFIX} CACHE FILEPATH "p4est library" FORCE)

set(p4est_flags --prefix=${p4est_ROOT})
set(p4est_mpi)
if(MPI_C_FOUND)
  set(p4est_mpi --enable-mpi)
endif()

ExternalProject_Add(p4est
GIT_REPOSITORY https://github.com/cburstedde/p4est.git
GIT_TAG 00da6a98a36c08833bb43eaf9d8f457ce587a3f2
CONFIGURE_COMMAND ${PROJECT_BINARY_DIR}/p4est-prefix/src/p4est/configure ${p4est_flags} ${p4est_mpi}
BUILD_COMMAND ${MAKE_EXECUTABLE} -j${Ncpu}
INSTALL_COMMAND ${MAKE_EXECUTABLE} -j${Ncpu} install
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
