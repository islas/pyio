cmake_minimum_required( VERSION 3.16 )

enable_language( C CXX Fortran )
set( CMAKE_CXX_STANDARD 11 )
# Use link paths as rpaths 
set( CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE )
set( CMAKE_Fortran_PREPROCESS ON )

set( USE_OPENMP OFF  )

set( PROJECT_TARGET pyio )
project( ${PROJECT_TARGET} )
set( EXPORT_NAME ${PROJECT_NAME} )

file( STRINGS ${PROJECT_SOURCE_DIR}/versions.txt VERSION_FILE_OUTPUT )
list( POP_FRONT VERSION_FILE_OUTPUT PYIO_VERSION )
set( PROJECT_VERSION ${PYIO_VERSION} )

message( STATUS "Setting project version to ${PROJECT_VERSION}" )

if ( NOT DEFINED PYBIND11_DIR )
  set( PYBIND11_DIR ${PROJECT_SOURCE_DIR}/../pybind11/ )
endif()

include( CMakePackageConfigHelpers )


if ( USE_OPENMP )
  find_package( OpenMP REQUIRED COMPONENTS Fortran CXX )
endif()

set( Python3_FIND_VIRTUALENV FIRST )
find_package( Python 3.0 REQUIRED COMPONENTS Development.Embed Interpreter )

# Create exec
add_library   ( ${PROJECT_NAME} SHARED )
add_executable( ${PROJECT_NAME}_demo )

add_subdirectory( src )

target_link_libraries(  ${PROJECT_NAME}
                          PRIVATE
                            $<$<BOOL:${USE_OPENMP}>:$<TARGET_NAME_IF_EXISTS:OpenMP::OpenMP_Fortran>>
                            $<$<BOOL:${USE_OPENMP}>:$<TARGET_NAME_IF_EXISTS:OpenMP::OpenMP_CXX>>
                            Python::Python
                        )
target_link_libraries(
                      ${PROJECT_NAME}_demo
                      PRIVATE
                        ${PROJECT_NAME}
                        $<$<BOOL:${USE_OPENMP}>:$<TARGET_NAME_IF_EXISTS:OpenMP::OpenMP_Fortran>>
                        Python::Python
                      )

target_include_directories( ${PROJECT_NAME}
                            PUBLIC
                              $<BUILD_INTERFACE:$<TARGET_PROPERTY:${PROJECT_NAME},Fortran_MODULE_DIRECTORY>>
                              $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src/pyio>
                              $<INSTALL_INTERFACE:include/>
                            PRIVATE
                              ${PYBIND11_DIR}/include
                            )

target_include_directories(
                            ${PROJECT_NAME}_demo
                            PRIVATE
                              ${PYBIND11_DIR}/include
                            )

set_target_properties(
                      ${PROJECT_NAME}
                      PROPERTIES
                        LINKER_LANGUAGE Fortran
                        )

set_target_properties( 
                      ${PROJECT_NAME}
                        PROPERTIES
                          Fortran_MODULE_DIRECTORY ${CMAKE_INSTALL_PREFIX}/include/${PROJECT_NAME}
                          Fortran_FORMAT           FREE
                          EXPORT_PROPERTIES        Fortran_MODULE_DIRECTORY
                      )

set_target_properties( 
                      ${PROJECT_NAME}_demo
                        PROPERTIES
                          Fortran_MODULE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/demo/
                          Fortran_FORMAT           FREE
                          EXPORT_PROPERTIES        Fortran_MODULE_DIRECTORY
                          LINKER_LANGUAGE          Fortran
                          INSTALL_RPATH            ${CMAKE_INSTALL_PREFIX}/lib
                      )

################################################################################
##
## Install and export
##
################################################################################
# Add to the export targets
install(
        TARGETS ${PROJECT_NAME}
        EXPORT ${EXPORT_NAME}Targets
        RUNTIME DESTINATION bin/
        ARCHIVE DESTINATION lib/
        LIBRARY DESTINATION lib/
        )

# Not part of export
install(
        TARGETS ${PROJECT_NAME}_demo
        RUNTIME DESTINATION bin/
        ARCHIVE DESTINATION lib/
        LIBRARY DESTINATION lib/
        )

# Install to namespace
install(
        EXPORT      ${EXPORT_NAME}Targets
        DESTINATION lib/cmake/
        FILE        ${EXPORT_NAME}Targets.cmake
        NAMESPACE   ${EXPORT_NAME}::
        )

configure_package_config_file(
                              ${PROJECT_SOURCE_DIR}/cmake/template/${EXPORT_NAME}Config.cmake.in
                              ${CMAKE_BINARY_DIR}/${EXPORT_NAME}Config.cmake
                              INSTALL_DESTINATION lib/cmake
                              )

write_basic_package_version_file(
                                  ${CMAKE_BINARY_DIR}/${EXPORT_NAME}ConfigVersion.cmake
                                  VERSION ${PROJECT_VERSION}
                                  #!TODO Check if this is the type of versioning support we want to use
                                  COMPATIBILITY SameMinorVersion
                                  )

install(
        FILES
          ${CMAKE_BINARY_DIR}/${EXPORT_NAME}Config.cmake
          ${CMAKE_BINARY_DIR}/${EXPORT_NAME}ConfigVersion.cmake
        DESTINATION lib/cmake
        )

install(
        FILES
          ${PROJECT_SOURCE_DIR}/src/pyio/EmbeddedInterpreter.hpp
        DESTINATION     include/${PROJECT_NAME}
        )