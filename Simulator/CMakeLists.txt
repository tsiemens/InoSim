cmake_minimum_required (VERSION 2.6)
project (Simulator)
# Version number
set (Simulator_VERSION_MAJOR 1)
set (Simulator_VERSION_MINOR 0)

# configure a header file to pass some of the CMake settings
# to the source code
configure_file (
  "${PROJECT_SOURCE_DIR}/SimulatorConfig.h.in"
  "${PROJECT_BINARY_DIR}/SimulatorConfig.h"
  )

# add the binary tree to the search path for include files
# so that we will find SimulatorConfig.h
include_directories("${PROJECT_BINARY_DIR}")

# compile a windows binary
option (WIN_COMPILE
        "Compile using ming32 instead of g++" OFF)
if (WIN_COMPILE)
   set (CMAKE_CXX_COMPILER "i686-w64-mingw32-g++")
   # get rid of the -rdynamic flag
   set (CMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS)
endif (WIN_COMPILE)

set (INO_PROG_FILE "Sample.ino" CACHE STRING "The .ino file to be compiled")

include_directories ("${PROJECT_SOURCE_DIR}" "${PROJECT_SOURCE_DIR}/InoLib")
add_subdirectory (InoLib)
set (EXTRA_LIBS ${EXTRA_LIBS} InoLib)

add_custom_command (OUTPUT "${PROJECT_BINARY_DIR}/InoProg.cpp"
   COMMAND cat InoProgHead.cpp.in ${INO_PROG_FILE} InoProgTail.cpp.in > "${PROJECT_BINARY_DIR}/InoProg.cpp"
   DEPENDS InoProgHead.cpp.in ${INO_PROG_FILE} InoProgTail.cpp.in
   WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
   )

# add_custom_target (InoSrcFile SOURCES ${INO_PROG_FILE})
add_executable(Simulator
   Simulator.cpp
   InoBuiltins.cpp
   InoProg.cpp
   )
target_link_libraries (Simulator  ${EXTRA_LIBS})

# add the install targets
set (CMAKE_INSTALL_PREFIX "${PROJECT_SOURCE_DIR}/install")
install (TARGETS Simulator DESTINATION bin)
install (FILES "${PROJECT_BINARY_DIR}/SimulatorConfig.h"
         DESTINATION include)
