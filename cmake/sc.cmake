include(ExternalProject)

set(sc_external true CACHE BOOL "build s library")

set(sc_ROOT ${PROJECT_BINARY_DIR}/sc)

set(sc_LIBRARY ${sc_ROOT}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}sc${CMAKE_STATIC_LIBRARY_SUFFIX} CACHE FILEPATH "sc library" FORCE)

set(sc_flags --prefix=${sc_ROOT})

ExternalProject_Add(sc
SOURCE_DIR ${PROJECT_SOURCE_DIR}/sc
CONFIGURE_COMMAND ${PROJECT_SOURCE_DIR}/sc/configure ${sc_flags}
BUILD_COMMAND make -j${Ncpu}
INSTALL_COMMAND make -j${Ncpu} install
TEST_COMMAND ""
BUILD_BYPRODUCTS ${sc_LIBRARY}
)

ExternalProject_Get_Property(sc SOURCE_DIR)

ExternalProject_Add_Step(sc
  bootstrap
  COMMAND ./bootstrap
  DEPENDEES download
  DEPENDERS configure
  WORKING_DIRECTORY ${SOURCE_DIR})
