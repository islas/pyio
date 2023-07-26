target_sources( 
                ${PROJECT_TARGET}
                PRIVATE
                  ${CMAKE_CURRENT_SOURCE_DIR}/driver.f90
              )

target_include_directories( 
                            ${PROJECT_TARGET}
                            PRIVATE
                              ${CMAKE_CURRENT_SOURCE_DIR}
                            )