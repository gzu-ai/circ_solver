cmake_minimum_required(VERSION 3.0)
project(circ_solver)
SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)

set(CMAKE_CXX_STANDARD 11)
SET(APP_VERSION "1.0.0")
SET(APP_NAME "circ_solver")
set(SAT_Solver "lstm" CACHE STRING "circ_solver")
include(ExternalProject)
include(cmake/common.cmake)
include(cmake/${SAT_Solver}.cmake)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_BINARY_DIR})
configure_file(config.h.in config.h)

add_library(cicrlf
        ${WRAPPER_FILES}
        Application.cpp
        core/Eliminate.cpp
        utils/DIMACSParser.cpp utils/SystemUtils.cpp utils/Parser.cpp core/CluaseSet.cpp core/Status.cpp )

# only use reduct
add_executable(circ_reduct_solver${SOLVER_SUFFIX} main.cpp core/CircReductSolver.cpp)
# use max loop and comp
add_executable(circ_lfc_solver${SOLVER_SUFFIX} main.cpp core/ReductSolver.cpp core/DnfLoopFormula.cpp core/LoopFormula.h)
#
add_executable(circ_solver${SOLVER_SUFFIX} main.cpp core/CircSolver.cpp)

target_link_libraries(circ_reduct_solver${SOLVER_SUFFIX} cicrlf ${SOLVER_LIBS})
target_link_libraries(circ_lfc_solver${SOLVER_SUFFIX} cicrlf ${SOLVER_LIBS})
target_link_libraries(circ_solver${SOLVER_SUFFIX} cicrlf ${SOLVER_LIBS})


