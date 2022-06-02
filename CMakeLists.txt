#----------------------------------------------------------------------------
# Setup the project
cmake_minimum_required(VERSION 3.8...3.18)
set(CMAKE_CXX_STANDARD 17)
if(${CMAKE_VERSION} VERSION_LESS 3.12)
    cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
endif()
project(GDMLExportFixer)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_C_FLAGS "-O0 -pthread -luuid")
    set(CMAKE_CXX_FLAGS "-O0 -pthread -luuid")
elseif (CMAKE_BUILD_TYPE STREQUAL "Release")
    set(CMAKE_C_FLAGS "-O3 -pthread -flto -luuid")
    set(CMAKE_CXX_FLAGS "-O3 -pthread -flto -luuid")
elseif (CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
    set(CMAKE_C_FLAGS "-O2 -pthread -fprofile-arcs -luuid")
    set(CMAKE_CXX_FLAGS "-O2 -pthread -fprofile-arcs -luuid")
endif ()


#----------------------------------------------------------------------------
# Find Geant4 package, activating all available Vis drivers by default
# You can set WITH_GEANT4_VIS to OFF via the command line or ccmake/cmake-gui
# to build a batch mode only executable
#
#set(CMAKE_FIND_DEBUG_MODE 1)
option(WITH_GEANT4_VIS "Build example with Geant4 Vis drivers" ON)
if(WITH_GEANT4_VIS)
    find_package(Geant4 REQUIRED gdml vis_all)
else()
    find_package(Geant4 REQUIRED gdml)
endif()

#----------------------------------------------------------------------------
# Setup Geant4 include directories and compile definitions
#
include(${Geant4_USE_FILE})


###### Finding BOOST
find_package(Boost COMPONENTS regex REQUIRED)

if (NOT TARGET Boost::filesystem)
  add_library(Boost::filesystem IMPORTED INTERFACE)
  set_property(TARGET Boost::filesystem PROPERTY
          INTERFACE_INCLUDE_DIRECTORIES ${Boost_INCLUDE_DIR})
  set_property(TARGET Boost::filesystem PROPERTY
          INTERFACE_LINK_LIBRARIES ${Boost_LIBRARIES})
endif ()

include_directories(${Boost_INCLUDE_DIRS})

#----------------------------------------------------------------------------
# Add the executable, and link it to the Geant4 libraries
#
include_directories(${Boost_INCLUDE_DIRS})
add_executable(GDMLExportFixer Replace.cpp)
target_link_libraries(GDMLExportFixer ${Boost_LIBRARIES})
