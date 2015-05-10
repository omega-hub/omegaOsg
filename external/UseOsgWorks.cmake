set(OSGWORKS_BASE_DIR ${CMAKE_BINARY_DIR}/3rdparty/osgWorks)

# Set common project arguments
set(OSGWORKS_ARGS
    #cmake
    -DCMAKE_CXX_FLAGS:STRING=${CMAKE_CXX_FLAGS}
    -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
    -DCMAKE_INSTALL_LIBDIR=lib
    #osg
    -DOSGInstallType:STRING="Alternate Install Location"
    #osgWorks
    -DOSGWORKS_BUILD_APPS:BOOL=OFF
    -DBUILD_SHARED_LIBS:BOOLEAN=false
    -DWORKS_BUILD_APPS=OFF
    -DWORKS_INSTALL_DATA=OFF
)


# Set platform-specific arguments
if(OMEGA_ARCH_32)
    set(OSGWORKS_ARGS
        ${OSGWORKS_ARGS}
        -DOSG_DIR:PATH=${OSG_INSTALL_DIR}/lib
    )
else()
    set(OSGWORKS_ARGS
        ${OSGWORKS_ARGS}
        -DOSG_DIR:PATH=${OSG_INSTALL_DIR}/lib64
    )
endif()
# NOTE: On windows, we explicitly list all the required bullet and osg libraries,
# since using the library finding modules does not work correctly in debug builds.
# the reason is that even for debug builds, release versions of bullet and osg are
# expected to be found in the installation dirs, making it impossible to generate
# a debug build without building a release version first (which is annoying)
if(WIN32)
    # The OSGWORKS_STATIC preprocessor definition tells osgBullet that
    # we are using the static version of osgWorks.
    set(OSGWORKS_ARGS
        ${OSGWORKS_ARGS}
        -DCMAKE_SHARED_LINKER_FLAGS:STRING="${CMAKE_SHARED_LINKER_FLAGS} /NODEFAULTLIB:msvcprt.lib /NODEFAULTLIB:libcpmt.lib"
        -DCMAKE_LINKER_FLAGS:STRING="${CMAKE_LINKER_FLAGS} /NODEFAULTLIB:libcpmt.lib /NODEFAULTLIB:msvcprt.lib"
        -DOSGInstallLocation:PATH=${OSG_BINARY_DIR}
        #osg
        -DOSG_INCLUDE_DIR:PATH=${OSG_INCLUDES}
        -DOSG_LIBRARY:PATH=${osg_LIBRARY}
        -DOSG_LIBRARY_DEBUG:PATH=${osg_LIBRARY_DEBUG}
        #osgGA
        -DOSGGA_INCLUDE_DIR:PATH=${OSG_INCLUDES}
        -DOSGGA_LIBRARY:PATH=${osgGA_LIBRARY}
        -DOSGGA_LIBRARY_DEBUG:PATH=${osgGA_LIBRARY_DEBUG}
        #osgText
        -DOSGTEXT_INCLUDE_DIR:PATH=${OSG_INCLUDES}
        -DOSGTEXT_LIBRARY:PATH=${osgText_LIBRARY}
        -DOSGTEXT_LIBRARY_DEBUG:PATH=${osgText_LIBRARY_DEBUG}
        #osgViewer
        -DOSGVIEWER_INCLUDE_DIR:PATH=${OSG_INCLUDES}
        -DOSGVIEWER_LIBRARY:PATH=${osgViewer_LIBRARY}
        -DOSGVIEWER_LIBRARY_DEBUG:PATH=${osgViewer_LIBRARY_DEBUG}
        #osgSim
        -DOSGSIM_INCLUDE_DIR:PATH=${OSG_INCLUDES}
        -DOSGSIM_LIBRARY:PATH=${osgSim_LIBRARY}
        -DOSGSIM_LIBRARY_DEBUG:PATH=${osgSim_LIBRARY_DEBUG}
        #osgDB
        -DOSGDB_INCLUDE_DIR:PATH=${OSG_INCLUDES}
        -DOSGDB_LIBRARY:PATH=${osgDB_LIBRARY}
        -DOSGDB_LIBRARY_DEBUG:PATH=${osgDB_LIBRARY_DEBUG}
        #osgUtil
        -DOSGUTIL_INCLUDE_DIR:PATH=${OSG_INCLUDES}
        -DOSGUTIL_LIBRARY:PATH=${osgUtil_LIBRARY}
        -DOSGUTIL_LIBRARY_DEBUG:PATH=${osgUtil_LIBRARY_DEBUG}
        #openthreads
        -DOPENTHREADS_INCLUDE_DIR:PATH=${OSG_INCLUDES}
        -DOPENTHREADS_LIBRARY:PATH=${OpenThreads_LIBRARY}
        -DOPENTHREADS_LIBRARY_DEBUG:PATH=${OpenThreads_LIBRARY_DEBUG}
    )
else()
    # The OSGWORKS_STATIC preprocessor definition tells osgBullet that
    # we are using the static version of osgWorks.
    set(OSGWORKS_ARGS
        ${OSGWORKS_ARGS}
        -DCMAKE_SHARED_LINKER_FLAGS:STRING=${CMAKE_SHARED_LINKER_FLAGS}
        -DCMAKE_LINKER_FLAGS:STRING=${CMAKE_LINKER_FLAGS}
        -DOSGInstallLocation:PATH=${OSG_BINARY_DIR}
    )
endif()

# Create the osgWorks project
ExternalProject_Add(
    osgWorks
    URL http://github.com/omega-hub/osgworks/archive/master.tar.gz
    DEPENDS osg
    CMAKE_ARGS ${OSGWORKS_ARGS}
    INSTALL_COMMAND ""
    
    # directories
    TMP_DIR ${CMAKE_BINARY_DIR}/3rdparty/tmp
    STAMP_DIR ${CMAKE_BINARY_DIR}/3rdparty/stamp
    DOWNLOAD_DIR ${CMAKE_BINARY_DIR}/3rdparty/osgWorks
    SOURCE_DIR ${OSGWORKS_BASE_DIR}/source
    BINARY_DIR ${OSGWORKS_BASE_DIR}/build
    INSTALL_DIR ${OSGWORKS_BASE_DIR}/install
)
set_target_properties(osgWorks PROPERTIES FOLDER "3rdparty")

# Set variables with paths of all the osgWorks components
set(OSGWORKS_BINARY_DIR ${OSGWORKS_BASE_DIR}/build/lib)
set(OSGWORKS_INCLUDES ${OSGWORKS_BASE_DIR}/source/include)

set(OSGWORKS_COMPONENTS 
    osgwTools 
    osgwQuery
)

if(WIN32)
    foreach( C ${OSGWORKS_COMPONENTS} )
		set(${C}_LIBRARY ${OSGWORKS_BINARY_DIR}/Release/${C}.lib)
		set(${C}_LIBRARY_DEBUG ${OSGWORKS_BINARY_DIR}/Debug/${C}d.lib)
		set(OSGWORKS_LIBS ${OSGWORKS_LIBS} optimized ${${C}_LIBRARY} debug ${${C}_LIBRARY_DEBUG})
	endforeach()
elseif(APPLE)
	foreach( C ${OSGWORKS_COMPONENTS} )
		set(${C}_LIBRARY ${OSGWORKS_BINARY_DIR}/lib${C}.a)
		set(${C}_LIBRARY_DEBUG ${OSGWORKS_BINARY_DIR}/lib${C}.a)
		set(OSGWORKS_LIBS ${OSGWORKS_LIBS} optimized ${${C}_LIBRARY} debug ${${C}_LIBRARY_DEBUG})
	endforeach()
else()
    foreach( C ${OSGWORKS_COMPONENTS} )
		set(${C}_LIBRARY ${OSGWORKS_BINARY_DIR}/lib${C}.a)
		set(${C}_LIBRARY_DEBUG ${OSGWORKS_BINARY_DIR}/lib${C}.a)
		set(OSGWORKS_LIBS ${OSGWORKS_LIBS} optimized ${${C}_LIBRARY} debug ${${C}_LIBRARY_DEBUG})
	endforeach()
endif()

add_definitions(-DOSGWORKS_STATIC)

