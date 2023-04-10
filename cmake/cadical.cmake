set(SOLVER_PATH "${third_part}/cadical")
find_package(ZLIB)
ExternalProject_Add(cadical_lib
        SOURCE_DIR "${SOLVER_PATH}"
        CONFIGURE_COMMAND ./configure
        INSTALL_COMMAND ""
        BUILD_IN_SOURCE true
        BUILD_COMMAND ${MAKE_EXE}
        GIT_TAG rel-1.5.3
        GIT_REPOSITORY  https://github.com/arminbiere/cadical.git
        )
include_directories(${CMAKE_BINARY_DIR}/third_part/cadical/src)
link_directories(${CMAKE_BINARY_DIR}/third_part/cadical/build)
add_library(cadical STATIC IMPORTED)
set_property(TARGET cadical PROPERTY IMPORTED_LOCATION ${SOLVER_PATH}/build/libcadical.a)
set(WRAPPER_FILES wrapper/CadicalSolver.cpp )
set(SOLVER_LIBS "cadical" )
list(APPEND SOLVER_LIBS  ${ZLIB_LIBRARY})
set(WRAPPER_INCLUDE "wrapper/CadicalSolver.h" )
set(WRAPPER_CLASS "CicrLF::CadicalSolver" )

set(SAT_SOLVER_VERSION "rel-1.5.3")
set(SAT_SOLVER "cadical")
set(SOLVER_SUFFIX "_cadical" )
