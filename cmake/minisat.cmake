set(SOLVER_SOURCE "${third_part}/minisat")
set(SOLVER_LIB_PATH "${third_part}_lib/minisat")

find_package(Patch)
find_package(ZLIB)
set(SOLVER_PATH "${CMAKE_BINARY_DIR}/${third_part}/minisat")
ExternalProject_Add(minisat_lib
        SOURCE_DIR ${SOLVER_PATH}
        CONFIGURE_COMMAND cmake .
        INSTALL_COMMAND ""
        BUILD_COMMAND ${MAKE_EXE} minisat-lib-static
        BINARY_DIR ${SOLVER_PATH}
        PATCH_COMMAND ${Patch_EXECUTABLE} -d ${SOLVER_PATH} -N -p1 < ${CMAKE_SOURCE_DIR}/patches/minisat.patch
#        GIT_TAG releases/2.2.0
        GIT_REPOSITORY https://github.com/niklasso/minisat.git
#        BUILD_IN_SOURCE true
        )
message("${Patch_EXECUTABLE} -d ${SOLVER_PATH} -N  -p1 < ${CMAKE_SOURCE_DIR}/patches/minisat.patch")

add_library(minisat STATIC IMPORTED)
set_property(TARGET minisat PROPERTY IMPORTED_LOCATION ${SOLVER_PATH}/libminisat.a)
include_directories(${SOLVER_PATH}/minisat)
include_directories(${SOLVER_PATH})
set(WRAPPER_FILES wrapper/MiniSATSolverWrapper.cpp )
set(SOLVER_LIBS "minisat" )
list(APPEND SOLVER_LIBS  ${ZLIB_LIBRARY})
set(WRAPPER_INCLUDE "wrapper/MiniSATSolverWrapper.h" )
set(WRAPPER_CLASS "CicrLF::MiniSATSolverWrapper" )
set(SAT_SOLVER_VERSION "v2.2.0")
set(SAT_SOLVER "minisat")
