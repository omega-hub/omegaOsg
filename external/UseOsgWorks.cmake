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

# The OSGWORKS_STATIC preprocessor definition tells osgBullet that
# we are using the static version of osgWorks.
if(WIN32)
  set(OSGWORKS_ARGS
    -DCMAKE_SHARED_LINKER_FLAGS:STRING="${CMAKE_SHARED_LINKER_FLAGS} /NODEFAULTLIB:msvcprt.lib /NODEFAULTLIB:libcpmt.lib"
		-DCMAKE_LINKER_FLAGS:STRING="${CMAKE_LINKER_FLAGS} /NODEFAULTLIB:libcpmt.lib /NODEFAULTLIB:msvcprt.lib"
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

