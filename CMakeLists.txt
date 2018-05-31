# $Id: CMakeLists.txt 68058 2013-03-13 14:47:43Z gcosmo $

#----------------------------------------------------------------------------
# Setup the project
#
cmake_minimum_required(VERSION 2.6 FATAL_ERROR)
project(COSMO)

#----------------------------------------------------------------------------
# Find Geant4 package, activating all available UI and Vis drivers by default
# You can set WITH_GEANT4_UIVIS to OFF via the command line or ccmake/cmake-gui
# to build a batch mode only executable
#
option(WITH_GEANT4_UIVIS "Build example with Geant4 UI and Vis drivers" ON)
if(WITH_GEANT4_UIVIS)
  find_package(Geant4 REQUIRED ui_all vis_all)
else()
  find_package(Geant4 REQUIRED)
endif()

#----------------------------------------------------------------------------
# Find ROOT variables if the variable G4ANALYSIS_USE_ROOT is set
#
list(APPEND CMAKE_PREFIX_PATH $ENV{ROOTSYS})
find_package(ROOT)
find_package(ROOT REQUIRED COMPONENTS RIO Net Hist Graf Graf3d Gpad Tree Rint Postscript Matrix Physics MathCore Thread Minuit Minuit2)
if(EXISTS ${ROOT_USE_FILE})
  include(${ROOT_USE_FILE})
endif()

#----------------------------------------------------------------------------
# Setup Geant4 include directories and compile definitions
# Setup include directory for this project
#
add_definitions(-DG4ANALYSIS_USE_ROOT)
include(${Geant4_USE_FILE})
include_directories(${ROOT_INCLUDE_DIR} ${Geant4_INCLUDE_DIR} ${PROJECT_SOURCE_DIR}/src/ ${PROJECT_SOURCE_DIR}/geo/)

#----------------------------------------------------------------------------
# Locate sources and headers for this project
# NB: headers are included so they will show up in IDEs
#
file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*/*.cc ${PROJECT_SOURCE_DIR}/src/geo/*/*.cc)
file(GLOB headers ${PROJECT_SOURCE_DIR}/src/*/*.hh ${PROJECT_SOURCE_DIR}/src/geo/*/*.cc)

#----------------------------------------------------------------------------
configure_file(cmake/setup.sh.in
  "${PROJECT_BINARY_DIR}/setup.sh" @ONLY)

# Build the CRESTA Library
add_library (cresta ${sources} ${headers})

#----------------------------------------------------------------------------
# Add the executable, and link it to the Geant4 libraries
#

add_executable(cosmicraysim ${PROJECT_SOURCE_DIR}/app/standard/cosmicraysim.cc)
target_link_libraries(cosmicraysim  ${Geant4_LIBRARIES} ${ROOT_LIBRARIES} cresta)

add_executable(chance_g4sim ${PROJECT_SOURCE_DIR}/app/chance/chance_g4sim.cc)
target_link_libraries(chance_g4sim  ${Geant4_LIBRARIES} ${ROOT_LIBRARIES} cresta)

add_executable(chance_trackfit ${PROJECT_SOURCE_DIR}/app/chance/chance_trackfit.cc)
target_link_libraries(chance_trackfit  ${Geant4_LIBRARIES} ${ROOT_LIBRARIES} cresta)


#----------------------------------------------------------------------------
# For internal Geant4 use - but has no effect if you build this
# example standalone
#
add_custom_target(COSMO DEPENDS cosmicraysim)

#----------------------------------------------------------------------------
# Copy all scripts to the build directory, i.e. the directory in which we
# build B3. This is so that we can run the executable directly because it
# relies on these scripts being in the current working directory.
#
set(COSMIC_SCRIPTS
  )

foreach(_script ${COSMIC_SCRIPTS})
  configure_file(
    ${PROJECT_SOURCE_DIR}/macros/${_script}
    ${PROJECT_BINARY_DIR}/${_script}
    COPYONLY
    )
endforeach()


#----------------------------------------------------------------------------
# Install the executable to 'bin' directory under CMAKE_INSTALL_PREFIX
#
install(TARGETS cosmicraysim DESTINATION bin )
#install(TARGETS pumas DESTINATION lib )
