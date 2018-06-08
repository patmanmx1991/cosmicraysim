# $Id: CMakeLists.txt 68058 2013-03-13 14:47:43Z gcosmo $

#----------------------------------------------------------------------------
# Setup the project
#
cmake_minimum_required(VERSION 2.6 FATAL_ERROR)
project(COSMO)

#----------------------------------------------------------------------------
# Set the build flags
SET(CMAKE_CXX_FLAGS_RELEASE "-std=c++11 -Wno-shadow -Wno-reorder -O3 -Wno-unused-function")
SET(CMAKE_CXX_FLAGS_DEBUG "-std=c++11 -Wno-shadow -Wno-reorder -gdb")

if (CMAKE_BUILD_TYPE MATCHES RELEASE)
  set(CURRENT_CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS_RELEASE})
else()
  set(CURRENT_CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS_DEBUG})
endif()

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

SET(LINK_LIBRARIES ${Geant4_LIBRARIES} ${ROOT_LIBRARIES})

#----------------------------------------------------------------------------
# USE CADMESH
if(USE_CADMESH)
  message(STATUS "Including CADMESH in build.")

  find_package(cadmesh)

  if (EXISTS ${cadmesh_USE_FILE})
    message(STATUS "Loading cadmash from use file.")
  else()

    if(CADMESH_DIR) 
      message(STATUS "Reading CADMESH_DIR=${CADMESH_DIR}")
    else()
      message(STATUS "No CADMESH_DIR specified. Looking in 'external/cadmesh/install/'")
      set(CADMESH_DIR "${PROJECT_SOURCE_DIR}/external/cadmesh/install/")
    endif()

    # Check it exists
    if(EXISTS ${CADMESH_DIR})
    else()
      message(FATAL_ERROR "Need CADMESH build to work! Scripts to download this are in external.")
    endif()

    # Add libraries and headers if so.
    set(CMAKE_PREFIX_PATH "${CMAKE_PREFIX_PATH} {CADMESH_DIR}/lib/")
    include_directories(${CADMESH_DIR}/include/)
    set(cadmesh_LIBRARIES "assimp;cadmesh;tet")
#    find_library(cadmesh_LIBRARIES 
#                 NAMES assimp cadmesh tet
#                 HINTS "{CADMESH_DIR}/lib/}")
  endif()

  # Set the libraries and compiler flags
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D__USE_CADMESH__") 
  SET(LINK_LIBRARIES "${LINK_LIBRARIES};${cadmesh_LIBRARIES}")

endif()

#----------------------------------------------------------------------------
# USE PUMAS FLAG
if(USE_PUMAS)
  message(STATUS "Including PUMAS in build.")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D__USE_PUMAS__=1")
  file(GLOB pumas_sources ${PROJECT_SOURCE_DIR}/external/pumas/*.c ${PROJECT_SOURCE_DIR}/external/pumas/*.cc)
  file(GLOB pumas_headers ${PROJECT_SOURCE_DIR}/external/pumas/*.h ${PROJECT_SOURCE_DIR}/external/pumas/*.hh)
  add_library (pumas ${pumas_sources} ${pumas_headers})
  set(LINK_LIBRARIES "${LINK_LIBRARIES} pumas")
endif()



#----------------------------------------------------------------------------
message(STATUS "Build Flags : ${CMAKE_CXX_FLAGS}")
message(STATUS "LINK LIBS : ${LINK_LIBRARIES}")


#----------------------------------------------------------------------------
# Locate sources and headers for this project
# NB: headers are included so they will show up in IDEs
#
file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*/*.cc ${PROJECT_SOURCE_DIR}/src/geo/*/*.cc)
file(GLOB headers ${PROJECT_SOURCE_DIR}/src/*/*.hh ${PROJECT_SOURCE_DIR}/src/geo/*/*.hh)

#----------------------------------------------------------------------------
configure_file(cmake/setup.sh.in
  "${PROJECT_BINARY_DIR}/setup.sh" @ONLY)

# Build the CRESTA Library
add_library (cresta ${sources} ${headers})

#----------------------------------------------------------------------------
# Add the executable, and link it to the Geant4 libraries
#

add_executable(cosmicraysim ${PROJECT_SOURCE_DIR}/app/standard/cosmicraysim.cc)
target_link_libraries(cosmicraysim ${LINK_LIBRARIES}  cresta)

add_executable(chance_g4sim ${PROJECT_SOURCE_DIR}/app/chance/chance_g4sim.cc)
target_link_libraries(chance_g4sim  ${LINK_LIBRARIES} cresta)

add_executable(chance_trackfit ${PROJECT_SOURCE_DIR}/app/chance/chance_trackfit.cc)
target_link_libraries(chance_trackfit  ${LINK_LIBRARIES} cresta)


if(USE_PUMAS)
add_executable(pumas_example ${PROJECT_SOURCE_DIR}/app/pumas/pumas_example.cc)
target_link_libraries(pumas_example  ${LINK_LIBRARIES} cresta)
endif()

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
