target_sources( 
                ${PROJECT_TARGET}
                PRIVATE
                  ${CMAKE_CURRENT_SOURCE_DIR}/EmbeddedInterpreter.cpp
                  ${CMAKE_CURRENT_SOURCE_DIR}/EmbeddedInterpreter.f90
                  ${CMAKE_CURRENT_SOURCE_DIR}/f_c_helpers.f90
              )