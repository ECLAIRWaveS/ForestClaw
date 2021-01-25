include(CheckIncludeFile)
include(CheckFunctionExists)

target_sources(forestclaw PRIVATE fclaw_base.c fclaw_options.c fclaw_gauges.c fclaw_package.c fclaw_math.c fclaw_timer.c fclaw_mpi.c fclaw2d_block.c fclaw2d_defs.c fclaw2d_options.c fclaw2d_global.c fclaw2d_forestclaw.c fclaw2d_vtable.c fclaw2d_map.c fclaw2d_map_query.c fclaw2d_map_brick.c fclaw2d_convenience.c fclaw2d_exchange.c fclaw2d_advance.c fclaw2d_elliptic_solver.c fclaw2d_physical_bc.c fclaw2d_ghost_fill.c fclaw2d_output.c fclaw2d_run.c fclaw2d_diagnostics.c fclaw2d_update_single_step.c fclaw2d_domain.c fclaw2d_regrid.c fclaw2d_patch.c fclaw2d_partition.c fclaw2d_timeinterp.c fclaw2d_initialize.c fclaw2d_finalize.c fclaw2d_time_sync.c fclaw2d_corner_neighbors.c fclaw2d_face_neighbors.c fclaw2d_farraybox.cpp fclaw2d_output_tikz.c forestclaw2d.c fp_exception_glibc_extension.c mappings/context.f mappings/scale_rot_map.f mappings/map_query.f mappings/cart/mapc2m_cart.f mappings/bilinear/mapc2m_bilinear.f mappings/annulus/mapc2m_annulus.f mappings/identity/mapc2m_identity.f mappings/pillowdisk/mapc2m_pillowdisk.f mappings/squareddisk/mapc2m_squareddisk.f mappings/pillowdisk5/mapc2m_pillowdisk5.f mappings/cubedsphere/mapc2m_cubedsphere.f mappings/pillowsphere/mapc2m_pillowsphere.f mappings/fivepatch/mapc2m_fivepatch.f mappings/torus/mapc2m_torus.f mappings/torus/mapc2m_twisted_torus.f mappings/latlong/mapc2m_latlong.f mappings/fclaw2d_map_nomap.c mappings/fclaw2d_map_nomap_brick.c fortran_source2d/cellave2.f fortran_source2d/fdisc.f fortran_source2d/philim.f)

# --- generate fclaw_config.h

set(CMAKE_REQUIRED_INCLUDES ${p4est_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR} ${PROJECT_BINARY_DIR}/include)

if(MPI_FOUND)
  set(FCLAW_CC \"${MPI_C_COMPILER}\")
  set(FCLAW_CPP \"${MPI_C_COMPILER} -E\")
  set(FCLAW_CXX \"${MPI_CXX_COMPILER}\")
  SET(FCLAW_F77 \"${MPI_Fortran_COMPILER}\")
else()
  set(FCLAW_CC \"${CMAKE_C_COMPILER}\")
  set(FCLAW_CPP \"${CMAKE_C_COMPILER} -E\")
  set(FCLAW_CXX \"${CMAKE_CXX_COMPILER}\")
  SET(FCLAW_F77 \"${CMAKE_Fortran_COMPILER}\")
endif()

set(FCLAW_CFLAGS \"${CMAKE_C_FLAGS} ${MPI_C_COMPILE_OPTIONS}\")

set(FCLAW_CPPFLAGS \"\")

set(FCLAW_FFLAGS \"${CMAKE_Fortran_FLAGS} ${MPI_Fortran_COMPILE_OPTIONS}\")
set(FCLAW_FLIBS \"${MPI_Fortran_LIBRARIES}\")

set(FCLAW_LDFLAGS \"${MPI_C_LINK_FLAGS}\")
set(FCLAW_LIBS \"${LAPACK_LIBRARIES} ${ZLIB_LIBRARIES} m\")

set(FCLAW_ENABLE_MEMALIGN 1)

if(MPI_FOUND)
  set(FCLAW_ENABLE_MPI 1)
  set(FCLAW_ENABLE_MPICOMMSHARED 1)
  set(FCLAW_ENABLE_MPIIO 1)
  set(FCLAW_ENABLE_MPITHREAD 1)
  set(FCLAW_ENABLE_MPIWINSHARED 1)
endif(MPI_FOUND)

check_include_file(dlfcn.h has_dlfcn)
set(FCLAW_HAVE_DLFCN_H ${has_dlfcn})

check_function_exists(feenableexcept has_feenableexcept)
set(FCLAW_HAVE_FEENABLEEXCEPT ${has_feenableexcept})

check_include_file(fenv.h has_fenv)
set(FCLAW_HAVE_FENV_H ${has_fenv})

check_include_file(inttypes.h has_inttypes)
set(FCLAW_HAVE_INTTYPES_H ${has_inttypes})


set(FCLAW_PACKAGE \"${PROJECT_NAME}\")
set(FCLAW_PACKAGE_VERSION \"${forestclaw_VERSION}\")


configure_file(fclaw_config.h.in ${PROJECT_BINARY_DIR}/include/fclaw_config.h)
