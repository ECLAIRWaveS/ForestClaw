# Findp4est.cmake
# ---------------
#
# Find p4est library (and sc)
#
# Result Variables
# ----------------
#
# This module defines the following variables::
#
#   p4est_FOUND
#   p4est_INCLUDE_DIRS   - include directories for p4est
#   p4est_LIBRARIES      - link against this library to use p4est
#
# Imported Targets
# ^^^^^^^^^^^^^^^^
#   p4est::p4est


find_path (p4est_INCLUDE_DIR
  NAMES p4est.h
  DOC "p4est header")

find_library (p4est_LIBRARY
  NAMES p4est
  DOC "p4est library")

find_package(sc)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args (p4est
  REQUIRED_VARS p4est_LIBRARY p4est_INCLUDE_DIR sc_LIBRARY sc_INCLUDE_DIR)

if (p4est_FOUND)
  set(p4est_INCLUDE_DIRS ${p4est_INCLUDE_DIR})
endif()

if(p4est_FOUND AND NOT TARGET p4est::p4est)
    add_library(p4est::p4est INTERFACE IMPORTED)
    target_link_libraries(p4est::p4est INTERFACE sc::sc)
    set_target_properties(p4est::p4est PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${p4est_INCLUDE_DIR}"
        INTERFACE_LINK_LIBRARIES "${p4est_LIBRARY}"
    )
endif()

mark_as_advanced(p4est_INCLUDE_DIR p4est_LIBRARY)
