set( BUILT_IN_PATH ${CMAKE_CURRENT_SOURCE_DIR}/pymodules )
configure_file( built_in_path.inc.in ${CMAKE_CURRENT_BINARY_DIR}/built_in_path.inc @ONLY )

target_sources( 
                ${PROJECT_NAME}_demo
                PRIVATE
                  ${CMAKE_CURRENT_SOURCE_DIR}/driver.f90
                  ${CMAKE_CURRENT_SOURCE_DIR}/EmbeddedModules.cpp
              )

target_include_directories( 
                            ${PROJECT_NAME}_demo
                            PRIVATE
                              ${CMAKE_CURRENT_BINARY_DIR}
                            )
