include(ExternalProject)

set(OMEGA_OSG_DEPENDENCIES CACHE INTERNAL "")
set(OSG_LIBS "")

# Directories used by osg
set(OSG_BASE_DIR ${CMAKE_BINARY_DIR}/3rdparty/osg)
set(OSG_BINARY_DIR ${OSG_BASE_DIR}/build)
set(OSG_SOURCE_DIR ${OSG_BASE_DIR}/source)
set(OSG_INSTALL_DIR ${OSG_BASE_DIR}/install CACHE INTERNAL "")

# Collada support (for reading VRML files.... we might just drop this in the 
# future)
if(_OMEGA_OSG_ENABLE_COLLADA_DOM)
    set(OMEGA_OSG_ENABLE_COLLADA_DOM true CACHE BOOL "Enable the Collada plugin for OSG.")
else()
    set(OMEGA_OSG_ENABLE_COLLADA_DOM false CACHE BOOL "Enable the Collada plugin for OSG.")
endif()
set(OMEGA_COLLADA_INCLUDE_DIR CACHE INTERNAL "")
set(OMEGA_COLLADA_LIBRARY CACHE INTERNAL "")
if(OMEGA_OSG_ENABLE_COLLADA_DOM)
    include(${CMAKE_CURRENT_LIST_DIR}/UseMinizip.cmake)
    include(${CMAKE_CURRENT_LIST_DIR}/UseCollada.cmake)
    set(OMEGA_OSG_DEPENDENCIES collada minizip)
    set(OMEGA_COLLADA_INCLUDE_DIR ${COLLADA_INCLUDE_DIR})
    set(OMEGA_COLLADA_LIBRARY ${COLLADA_LIBRARY})
endif()

# Save the version of OSG used. Right now this variable only used by modules
# that create osg plugins (like omegaOsgEarth) to put the plugins in the
# right directory.
# You can find the OSG version in the main osg cmake file around line 54..
# in the future we can probably pull the value directly from there.
set(OSG_VERSION "3.3.8" CACHE INTERNAL "")

# Set the common project arguments
set(OSG_ARGS
    -DFREETYPE_DIR=${CMAKE_BINARY_DIR}/freetype
    -DBUILD_OSG_APPLICATIONS=OFF
    -DCMAKE_INSTALL_PREFIX:PATH=${OSG_INSTALL_DIR}
    -DCOLLADA_DYNAMIC_LIBRARY=${COLLADA_LIBRARY}
    -DCOLLADA_INCLUDE_DIR=${COLLADA_INCLUDE_DIR}
    -DCOLLADA_MINIZIP_LIBRARY=${MINIZIP_LIBRARY}
    -DCMAKE_CXX_FLAGS:STRING=${CMAKE_CXX_FLAGS}
)

# Set platform-specific arguments
if(WIN32)
    set(OSG_ARGS
        ${OSG_ARGS}
        -DCMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG:PATH=${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG}
        -DCMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE:PATH=${CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE}
        -DCMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG:PATH=${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG}
        -DCMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE:PATH=${CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE}
        -DCMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE:PATH=${CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE}
        -DCMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG:PATH=${CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG}
    )
else()
    set(OSG_ARGS
        ${OSG_ARGS}
        -DCMAKE_MACOSX_RPATH=${CMAKE_MACOSX_RPATH}
        -DCMAKE_INSTALL_RPATH=${CMAKE_INSTALL_RPATH}
        -DCMAKE_BUILD_WITH_INSTALL_RPATH=${CMAKE_BUILD_WITH_INSTALL_RPATH}
        # We force osg to compile using C++98 (libstdc++) on all platforms.
        # On apple, osg tried to use C++11 (libc++) by default, which causes
        # linker error with other platforms.
        # In the future, we should move everything to c++11, but for the time being
        # this is an easier solution.
        -DOSG_CXX_LANGUAGE_STANDARD:STRING=c++98
        -DCMAKE_RUNTIME_OUTPUT_DIRECTORY:PATH=${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
        -DCMAKE_RUNTIME_OUTPUT_DIRECTORY:PATH=${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
        -DCMAKE_LIBRARY_OUTPUT_DIRECTORY:PATH=${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
        -DCMAKE_LIBRARY_OUTPUT_DIRECTORY:PATH=${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
        -DCMAKE_ARCHIVE_OUTPUT_DIRECTORY:PATH=${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}
        -DCMAKE_ARCHIVE_OUTPUT_DIRECTORY:PATH=${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}
    )
endif()

# Create the OSG external project
ExternalProject_Add(
    osg
    DEPENDS ${OMEGA_OSG_DEPENDENCIES}
    URL http://github.com/omega-hub/osg/archive/master.tar.gz
    CMAKE_ARGS ${OSG_ARGS}
    INSTALL_COMMAND ${PLATFORM_INSTALL_COMMAND}
    
    # directories
    TMP_DIR ${CMAKE_BINARY_DIR}/3rdparty/tmp
    STAMP_DIR ${CMAKE_BINARY_DIR}/3rdparty/stamp
    DOWNLOAD_DIR ${CMAKE_BINARY_DIR}/3rdparty/osg
    SOURCE_DIR ${OSG_SOURCE_DIR}
    BINARY_DIR ${OSG_BINARY_DIR}
    INSTALL_DIR ${OSG_INSTALL_DIR}
)
set_target_properties(osg PROPERTIES FOLDER "3rdparty")

set(OSG_INCLUDES ${OSG_INSTALL_DIR}/include)

set(OSG_COMPONENTS 
    osg 
    osgAnimation 
    osgDB 
    osgManipulator 
    osgFX 
    osgShadow 
    osgTerrain 
    osgText 
    osgUtil 
    osgVolume 
    OpenThreads 
    osgGA 
    osgViewer 
    osgSim 
    osgWidget)

# Set variables pointing to all the osg component libraries    
if(WIN32)
    foreach( C ${OSG_COMPONENTS} )
        set(${C}_LIBRARY ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE}/${C}.lib CACHE INTERNAL "")
        set(${C}_LIBRARY_DEBUG ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG}/${C}d.lib CACHE INTERNAL "")
        set(OSG_LIBS ${OSG_LIBS} optimized ${${C}_LIBRARY} debug ${${C}_LIBRARY_DEBUG} CACHE INTERNAL "")
    endforeach()
elseif(APPLE)
	foreach( C ${OSG_COMPONENTS} )
		set(${C}_LIBRARY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/lib${C}.dylib)
		set(${C}_LIBRARY_DEBUG ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/lib${C}d.dylib)
		set(OSG_LIBS ${OSG_LIBS} optimized ${${C}_LIBRARY} debug ${${C}_LIBRARY_DEBUG})
	endforeach()
else()
    foreach( C ${OSG_COMPONENTS} )
        set(${C}_LIBRARY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/lib${C}.so)
        set(${C}_LIBRARY_DEBUG ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/lib${C}d.so)
        set(OSG_LIBS ${OSG_LIBS} optimized ${${C}_LIBRARY} debug ${${C}_LIBRARY_DEBUG})
    endforeach()
endif()

include(${CMAKE_CURRENT_LIST_DIR}/UseOsgWorks.cmake)
    
# Add osgWorks to openscenegraph includes and libraries (this simplified inclusion 
# in other projects. We consider osg and osgWorks as a single package.
set(OSG_INCLUDES ${OSG_INCLUDES} ${OSGWORKS_INCLUDES} CACHE INTERNAL "")
set(OSG_LIBS ${OSG_LIBS} ${OSGWORKS_LIBS} CACHE INTERNAL "")

