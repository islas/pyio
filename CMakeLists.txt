cmake_minimum_required( VERSION 3.16 )

enable_language( C CXX Fortran )
set( CMAKE_CXX_STANDARD 11 )
# Use link paths as rpaths 
set( CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE )
set( CMAKE_Fortran_PREPROCESS ON )

set( USE_MPI    OFF )
set( USE_OPENMP OFF  )

set( PROJECT_TARGET pybindDemo )
project( ${PROJECT_TARGET} )
set( CMAKE_BUILD_TYPE Debug )

if ( NOT DEFINED PYBIND11_DIR )
  set( PYBIND11_DIR ${PROJECT_SOURCE_DIR}/../pybind11/ )
endif()

if ( USE_MPI )
  find_package( MPI REQUIRED COMPONENTS Fortran CXX )
  target_compile_definitions( ${PROJECT_TARGET} PRIVATE -DUSE_MPI=1 )
endif()

if ( USE_OPENMP )
  find_package( OpenMP REQUIRED COMPONENTS Fortran CXX )
  if ( NOT OpenMP_Fortran_FOUND )
    message( "OpenMP_Fortran_FOUND => ${OpenMP_Fortran_FOUND}" )
  endif()
endif()

find_package( Python 3.0 REQUIRED COMPONENTS Development.Embed )

# Create exec
add_executable( ${PROJECT_TARGET} )

add_subdirectory( src )

target_link_libraries(  ${PROJECT_TARGET}
                          PRIVATE
                            $<$<BOOL:${USE_MPI}>:$<TARGET_NAME_IF_EXISTS:MPI::MPI_Fortran>>
                            $<$<BOOL:${USE_OPENMP}>:$<TARGET_NAME_IF_EXISTS:OpenMP::OpenMP_Fortran>>
                            $<$<BOOL:${USE_OPENMP}>:$<TARGET_NAME_IF_EXISTS:OpenMP::OpenMP_CXX>>
                            Python::Python
                        )

target_include_directories( ${PROJECT_TARGET}
                            PRIVATE
                              ${PYBIND11_DIR}/include
                            )

set_target_properties(
                      ${PROJECT_TARGET}
                      PROPERTIES
                        LINKER_LANGUAGE Fortran
                        )

install(
        TARGETS ${PROJECT_TARGET}
        RUNTIME DESTINATION bin/
        ARCHIVE DESTINATION lib/
        LIBRARY DESTINATION lib/
        )