set(OSGWORKS_GENERATOR ${CMAKE_GENERATOR})

set(OSGWorks_CXX_FLAGS ${CMAKE_CXX_FLAGS} -fPIC)

set(OSGWORKS_ARGS
  -DCMAKE_SHARED_LINKER_FLAGS:STRING=${CMAKE_SHARED_LINKER_FLAGS}
  -DCMAKE_LINKER_FLAGS:STRING=${CMAKE_LINKER_FLAGS}
  -DCMAKE_CXX_FLAGS:STRING=${OSGWorks_CXX_FLAGS}
  -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
  -DBUILD_SHARED_LIBS:BOOLEAN=false
  -DWORKS_BUILD_APPS=OFF
  -DWORKS_INSTALL_DATA=OFF
  #osg
  -DOSG_DIR:PATH=${OSG_INSTALL_DIR}
  #osgWorks
  -DOSGWORKS_BUILD_APPS:BOOL=OFF
)

# NOTE: On windows, we explicitly list all the required bullet and osg libraries,
# since using the library finding modules does not work correctly in debug builds.
# the reason is that even for debug builds, release versions of bullet and osg are
# expected to be found in the installation dirs, making it impossible to generate
# a debug build without building a release version first (which is annoying)
if(WIN32)
	# The OSGWORKS_STATIC preprocessor definition tells osgBullet that
	# we are using the static version of osgWorks.
	message("OSG INCLUDE DIR: ${OSG_INCLUDES}")
  set(OSGWORKS_ARGS
    -DCMAKE_SHARED_LINKER_FLAGS:STRING="${CMAKE_SHARED_LINKER_FLAGS} /NODEFAULTLIB:msvcprt.lib /NODEFAULTLIB:libcpmt.lib"
	-DCMAKE_LINKER_FLAGS:STRING="${CMAKE_LINKER_FLAGS} /NODEFAULTLIB:libcpmt.lib /NODEFAULTLIB:msvcprt.lib"
	-DOSGInstallType:STRING="Alternate Install Location"
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
	${OSGWORKS_ARGS}
  )
endif()

ExternalProject_Add(
  osgWorks
  URL ${CMAKE_SOURCE_DIR}/modules/omegaOsg/external/osgw2.tar.gz
  CMAKE_GENERATOR ${OSGWORKS_GENERATOR}
  DEPENDS osg
  CMAKE_ARGS 
    ${OSGWORKS_ARGS}
  INSTALL_COMMAND ""
)

set_target_properties(osgWorks PROPERTIES FOLDER "3rdparty")
# define path to libraries built by the equalizer external project
set(OSGWORKS_BINARY_DIR ${CMAKE_BINARY_DIR}/modules/omegaOsg/osgWorks-prefix/src/osgWorks-build/lib)
set(OSGWORKS_COMPONENTS osgwTools osgwQuery)

# NEED SECTIONS DEPENDENT ON BUILD TOOL, NOT OS!
if(OMEGA_OS_WIN)
    foreach( C ${OSGWORKS_COMPONENTS} )
		set(${C}_LIBRARY ${OSGWORKS_BINARY_DIR}/Release/${C}.lib)
		set(${C}_LIBRARY_DEBUG ${OSGWORKS_BINARY_DIR}/Debug/${C}d.lib)
		set(OSGWORKS_LIBS ${OSGWORKS_LIBS} optimized ${${C}_LIBRARY} debug ${${C}_LIBRARY_DEBUG})
	endforeach()
elseif(OMEGA_OS_LINUX)
    foreach( C ${OSGWORKS_COMPONENTS} )
		set(${C}_LIBRARY ${OSGWORKS_BINARY_DIR}/lib${C}.a)
		set(${C}_LIBRARY_DEBUG ${OSGWORKS_BINARY_DIR}/lib${C}.a)
		set(OSGWORKS_LIBS ${OSGWORKS_LIBS} optimized ${${C}_LIBRARY} debug ${${C}_LIBRARY_DEBUG})
	endforeach()
elseif(APPLE)
	foreach( C ${OSGWORKS_COMPONENTS} )
		set(${C}_LIBRARY ${OSGWORKS_BINARY_DIR}/lib${C}.a)
		set(${C}_LIBRARY_DEBUG ${OSGWORKS_BINARY_DIR}/lib${C}.a)
		set(OSGWORKS_LIBS ${OSGWORKS_LIBS} optimized ${${C}_LIBRARY} debug ${${C}_LIBRARY_DEBUG})
	endforeach()
endif(OMEGA_OS_WIN)

add_definitions(-DOSGWORKS_STATIC)

set(OSGWORKS_INCLUDES ${CMAKE_BINARY_DIR}/modules/omegaOsg/osgWorks-prefix/src/osgWorks/include)

