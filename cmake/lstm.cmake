set(SOLVER_PATH "${CMAKE_BINARY_DIR}/${third_part}/lstm")
include(ExternalProject)
ExternalProject_Add(lstm_lib
        SOURCE_DIR ${SOLVER_PATH}
        CONFIGURE_COMMAND ""
        INSTALL_COMMAND ""
        BUILD_COMMAND ${MAKE_EXE} libs
        BINARY_DIR ${SOLVER_PATH}/sources/simp
        GIT_TAG main
        GIT_REPOSITORY  https://github.com/caiswgroup/LSTech-Maple.git
)
find_package(ZLIB)
add_library(lstm STATIC IMPORTED)
set_property(TARGET lstm PROPERTY IMPORTED_LOCATION ${SOLVER_PATH}/sources/simp/lib.a)
include_directories(${SOLVER_PATH}/sources)
set(WRAPPER_FILES wrapper/LCMDCBDLSolverWrapper.cpp )
set(SOLVER_SUFFIX "_lstm" )
set(SOLVER_LIBS "lstm" )
list(APPEND SOLVER_LIBS  ${ZLIB_LIBRARY} )
set(WRAPPER_INCLUDE "wrapper/LCMDCBDLSolverWrapper.h" )
set(WRAPPER_CLASS "CicrLF::LCMDCBDLSolverWrapper" )
set(SAT_SOLVER_VERSION "v1" )
set(SAT_SOLVER "lstm" )
