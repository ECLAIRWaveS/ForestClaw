include(CMakeDependentOption)

option(mpi "use MPI library" on)
option(openmp "use OpenMP" on)

option(geoclaw "build Geoclaw")
option(thunderegg "build ThunderEgg")
cmake_dependent_option(clawpack "build Clawpack" OFF "thunderegg" ON)

option(thunderegg_external "force build of ThunderEgg")
option(p4est_external "force build of p4est")
option(sc_external "force build of libsc")

set(CMAKE_EXPORT_COMPILE_COMMANDS on)

# --- default install directory under build/local
# users can specify like "cmake -B build -DCMAKE_INSTALL_PREFIX=~/mydir"
if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  # will not take effect without FORCE
  set(CMAKE_INSTALL_PREFIX "${PROJECT_BINARY_DIR}/local" CACHE PATH "Install top-level directory" FORCE)
endif()
