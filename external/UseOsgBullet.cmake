include(${CMAKE_CURRENT_LIST_DIR}/UseBullet.cmake)

# Add external project osgBullet
# Pro Trick here: we can't pass the string directly as a CMAKE_ARG in 
# ExternalProject_Add, because it would keep the double quotes, and we
# do not want them. Passing it as a variable removes the dobule quotes.
set(BulletInstallType "Alternate Install Location")
set(OsgInstallType "Source And Build Tree")

if(WIN32 OR APPLE)
  set(OSGWORKS_LIB_SUFFIX lib)
else()
  set(OSGWORKS_LIB_SUFFIX lib64)
endif()

set(OSGBULLET_ARGS
  -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
  #-DCMAKE_RUNTIME_OUTPUT_DIRECTORY:PATH=${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
  #-DCMAKE_RUNTIME_OUTPUT_DIRECTORY:PATH=${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/osg/osgPlugins-3.3.0
  -DCMAKE_LIBRARY_OUTPUT_DIRECTORY:PATH=${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/osg/osgPlugins-3.3.0
  -DCMAKE_LIBRARY_OUTPUT_DIRECTORY:PATH=${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/osg/osgPlugins-3.3.0
  -DCMAKE_ARCHIVE_OUTPUT_DIRECTORY:PATH=${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}/osg/osgPlugins-3.3.0
  -DCMAKE_ARCHIVE_OUTPUT_DIRECTORY:PATH=${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}/osg/osgPlugins-3.3.0

  -DOSGInstallType:STRING=${OsgInstallType}
  -DOSGSourceRoot:STRING=${CMAKE_BINARY_DIR}/modules/omegaOsg/osg-prefix/src/osg
  -DOSGBuildRoot:STRING=${CMAKE_BINARY_DIR}/modules/omegaOsg/osg-prefix/src/osg-build
  
  -DBUILD_SHARED_LIBS=ON
  -DOSGBULLET_BUILD_APPLICATIONS=OFF
  -DOSGBULLET_BUILD_EXAMPLES=OFF
  -DOSGBULLET_INSTALL_DATA=OFF
  #Bullet
  -DBulletInstallType:STRING=${BulletInstallType}
  -DBulletSourceRoot:STRING=${CMAKE_BINARY_DIR}/modules/omegaOsg/bullet-prefix/src/bullet
  -DBulletBuildRoot:STRING=${CMAKE_BINARY_DIR}/modules/omegaOsg/bullet-prefix/src/bullet-build
  -DBulletInstallLocation=${CMAKE_BINARY_DIR}/modules/omegaOsg/bullet-prefix/src/bullet-install
  #----------
  # NOTE: on linux, the osgWorks config files go in the lib or lib64 directory depending on the bitness.
  # This means we will need to take this into accout when building for 32 bit linux. 
  #osgWorks
  -DosgWorks_DIR:PATH=${CMAKE_BINARY_DIR}/modules/omegaOsg/osgWorks-prefix/src/osgWorks-build/${OSGWORKS_LIB_SUFFIX}
  #----------
  -DOSG_DIR:PATH=${OSG_INSTALL_DIR}
)

# The OSGWORKS_STATIC preprocessor definition tells osgBullet that
# we are using the static version of osgWorks.
if(WIN32)
	set(OSGBullet_CXX_FLAGS "${CMAKE_CXX_FLAGS} /D\"OSGWORKS_STATIC\"")
	set(OSGBULLET_ARGS
		#-DCMAKE_SHARED_LINKER_FLAGS:STRING="${CMAKE_SHARED_LINKER_FLAGS} /NODEFAULTLIB:msvcprt.lib /NODEFAULTLIB:libcpmt.lib"
		#-DCMAKE_LINKER_FLAGS:STRING="${CMAKE_LINKER_FLAGS} /NODEFAULTLIB:libcpmt.lib /NODEFAULTLIB:msvcprt.lib"
		-DCMAKE_CXX_FLAGS:STRING=${OSGBullet_CXX_FLAGS}
		-DCMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG:PATH=${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG}
		-DCMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE:PATH=${CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE}
		${OSGBULLET_ARGS}
	)
else()
	#set(OSGBullet_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DOSGWORKS_STATIC")
	set(OSGBULLET_ARGS
		-DCMAKE_RUNTIME_OUTPUT_DIRECTORY:PATH=${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG}/osg/osgPlugins-3.3.0
	)
endif()

ExternalProject_Add(
	osgBullet
	DEPENDS bullet osgWorks
	URL ${CMAKE_SOURCE_DIR}/modules/omegaOsg/external/osgbullet.tar.gz
	CMAKE_ARGS
    ${OSGBULLET_ARGS}
  INSTALL_COMMAND ""
)

set_target_properties(osgBullet PROPERTIES FOLDER "3rdparty")

set(OSGBULLET_BASE_DIR ${CMAKE_BINARY_DIR}/modules/omegaOsg/osgBullet-prefix/src)
# NOTE: setting the OSGBULLET_INCLUDES as an internal cache variable, makes it accessible to other modules.
set(OSGBULLET_INCLUDES ${BULLET_INCLUDES} ${OSGBULLET_BASE_DIR}/osgBullet/include CACHE INTERNAL "")

set(OSGBULLET_LIB_DIR ${OSGBULLET_BASE_DIR}/osgBullet-build/lib)
set(OSGBULLET_COMPONENTS osgbCollision osgbDynamics osgbInteraction)
if(OMEGA_OS_WIN)
	foreach( C ${OSGBULLET_COMPONENTS})
		set(${C}_LIBRARY ${OSGBULLET_LIB_DIR}/Release/${C}.lib)
		set(${C}_LIBRARY_DEBUG ${OSGBULLET_LIB_DIR}/Debug/${C}d.lib)
		set(OSGBULLET_LIBRARIES ${OSGBULLET_LIBRARIES} optimized ${${C}_LIBRARY} debug ${${C}_LIBRARY_DEBUG})
	endforeach()
elseif(OMEGA_OS_LINUX)
    foreach( C ${OSGBULLET_COMPONENTS} )
		set(${C}_LIBRARY ${OSGBULLET_LIB_DIR}/lib${C}.so)
		set(${C}_LIBRARY_DEBUG ${OSGBULLET_LIB_DIR}/lib${C}.so)
		set(OSGBULLET_LIBRARIES ${OSGBULLET_LIBRARIES} optimized ${${C}_LIBRARY} debug ${${C}_LIBRARY_DEBUG})
	endforeach()
elseif(APPLE)
	foreach( C ${OSGBULLET_COMPONENTS} )
		set(${C}_LIBRARY ${OSGBULLET_LIB_DIR}/lib${C}.dylib)
		set(${C}_LIBRARY_DEBUG ${OSGBULLET_LIB_DIR}/lib${C}.dylib)
		set(OSGBULLET_LIBRARIES ${OSGBULLET_LIBRARIES} optimized ${${C}_LIBRARY} debug ${${C}_LIBRARY_DEBUG})
	endforeach()
endif(OMEGA_OS_WIN)

if(WIN32)
	# Add the DOSGBULLET_STATIC to following projects, so we tell the header files 
	# we are not importing dll symbols.
	add_definitions(-DOSGBULLET_STATIC)
endif()

# NOTE: setting the OSGBULLET_LIBS as an internal cache variable, makes it accessible to other modules.
set(OSGBULLET_LIBS ${OSGBULLET_LIBRARIES} ${BULLET_LIBS} CACHE INTERNAL "")
