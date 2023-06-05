target_sources( 
                ${PROJECT_TARGET}
                PRIVATE
                  ${CMAKE_CURRENT_SOURCE_DIR}/EmbeddedInterpreter.cpp
              )

target_include_directories( 
                            ${PROJECT_TARGET}
                            PRIVATE
                              ${CMAKE_CURRENT_SOURCE_DIR}
                            )