include(CheckIncludeFile)
include(CheckIncludeFiles)
include(CheckSymbolExists)

target_sources(forestclaw PRIVATE fclaw_base.c fclaw_options.c fclaw_gauges.c fclaw_package.c fclaw_math.c fclaw_timer.c fclaw_mpi.c fclaw2d_block.c fclaw2d_defs.c fclaw2d_options.c fclaw2d_global.c fclaw2d_forestclaw.c fclaw2d_vtable.c fclaw2d_map.c fclaw2d_map_query.c fclaw2d_map_brick.c fclaw2d_convenience.c fclaw2d_exchange.c fclaw2d_advance.c fclaw2d_elliptic_solver.c fclaw2d_physical_bc.c fclaw2d_ghost_fill.c fclaw2d_output.c fclaw2d_run.c fclaw2d_diagnostics.c fclaw2d_update_single_step.c fclaw2d_domain.c fclaw2d_regrid.c fclaw2d_patch.c fclaw2d_partition.c fclaw2d_timeinterp.c fclaw2d_initialize.c fclaw2d_finalize.c fclaw2d_time_sync.c fclaw2d_corner_neighbors.c fclaw2d_face_neighbors.c fclaw2d_farraybox.cpp fclaw2d_output_tikz.c forestclaw2d.c fp_exception_glibc_extension.c mappings/context.f mappings/scale_rot_map.f mappings/map_query.f mappings/cart/mapc2m_cart.f mappings/bilinear/mapc2m_bilinear.f mappings/annulus/mapc2m_annulus.f mappings/identity/mapc2m_identity.f mappings/pillowdisk/mapc2m_pillowdisk.f mappings/squareddisk/mapc2m_squareddisk.f mappings/pillowdisk5/mapc2m_pillowdisk5.f mappings/cubedsphere/mapc2m_cubedsphere.f mappings/pillowsphere/mapc2m_pillowsphere.f mappings/fivepatch/mapc2m_fivepatch.f mappings/torus/mapc2m_torus.f mappings/torus/mapc2m_twisted_torus.f mappings/latlong/mapc2m_latlong.f mappings/fclaw2d_map_nomap.c mappings/fclaw2d_map_nomap_brick.c fortran_source2d/cellave2.f fortran_source2d/fdisc.f fortran_source2d/philim.f)

# --- generate fclaw_config.h

set(CMAKE_REQUIRED_INCLUDES ${p4est_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR} ${PROJECT_BINARY_DIR}/include)
set(CMAKE_REQUIRED_LIBRARIES MPI::MPI_C ZLIB::ZLIB m)

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
set(FCLAW_LIBS \"${LAPACK_LIBRARIES} ${BLAS_LIBRARIES} ${ZLIB_LIBRARIES} m\")

set(FCLAW_ENABLE_MEMALIGN 1)

if(MPI_FOUND)
  set(FCLAW_ENABLE_MPI ${MPI_FOUND})
  check_symbol_exists(MPI_COMM_TYPE_SHARED mpi.h FCLAW_ENABLE_MPICOMMSHARED)
  set(FCLAW_ENABLE_MPIIO 1)
  check_symbol_exists(MPI_Init_thread mpi.h FCLAW_ENABLE_MPITHREAD)
  check_symbol_exists(MPI_Win_allocate_shared mpi.h FCLAW_ENABLE_MPIWINSHARED)
endif(MPI_FOUND)

check_symbol_exists(aligned_alloc stdlib.h FCLAW_HAVE_ALIGNED_ALLOC)
check_include_file(dlfcn.h FCLAW_HAVE_DLFCN_H)
check_symbol_exists(feenableexcept fenv.h FCLAW_HAVE_FEENABLEEXCEPT)
check_include_file(fenv.h FCLAW_HAVE_FENV_H)
check_include_file(inttypes.h FCLAW_HAVE_INTTYPES_H)

check_symbol_exists(pthread_create pthread.h HAVE_LPTHREAD)
check_symbol_exists(lua_createtable lua.h HAVE_LUA)

check_include_file(memory.h FCLAW_HAVE_MEMORY_H)
check_symbol_exists(omp_get_thread_num omp.h HAVE_OPENMP)
check_symbol_exists(posix_memalign stdlib.h FCLAW_HAVE_POSIX_MEMALIGN)
check_include_file(signal.h FCLAW_HAVE_SIGNAL_H)
check_include_file(stdint.h FCLAW_HAVE_STDINT_H)
check_include_file(stdlib.h FCLAW_HAVE_STDLIB_H)
check_include_file(strings.h FCLAW_HAVE_STRINGS_H)
check_include_file(string.h FCLAW_HAVE_STRING_H)
check_include_file(sys/stat.h FCLAW_HAVE_SYS_STAT_H)
check_include_file(sys/types.h FCLAW_HAVE_SYS_TYPES_H)
check_include_file(unistd.h FCLAW_HAVE_UNISTD_H)
check_symbol_exists(adler32_combine zlib.h FCLAW_HAVE_ZLIB)

set(FCLAW_WITH_BLAS ${BLAS_FOUND})
set(FCLAW_WITH_LAPACK ${LAPACK_FOUND})

set(WITH_P4EST ${p4est_ROOT})
set(WITH_SC ${p4est_ROOT})

set(FCLAW_PACKAGE \"${PROJECT_NAME}\")
set(FCLAW_PACKAGE_VERSION \"${PROJECT_VERSION}\")

set(FCLAW_SIZEOF_VOID_P ${CMAKE_SIZEOF_VOID_P})
check_include_files("stdlib.h;stdarg.h;string.h;float.h" FCLAW_STDC_HEADERS LANGUAGE C)

set(FCLAW_VERSION \"${PROJECT_VERSION}\")



configure_file(fclaw_config.h.in ${PROJECT_BINARY_DIR}/include/fclaw_config.h)
