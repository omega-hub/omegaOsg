include(${CMAKE_CURRENT_LIST_DIR}/UseBullet.cmake)


# Add external project osgBullet
# Pro Trick here: we can't pass the string directly as a CMAKE_ARG in
# ExternalProject_Add, because it would keep the double quotes, and we
# do not want them. Passing it as a variable removes the dobule quotes.
if(WIN32)
        set(BulletInstallType "Source And Build Tree")
else()
        set(BulletInstallType "Alternate Install Location")
endif()

if(OMEGA_ARCH_32)
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

  -DBUILD_SHARED_LIBS=ON
  -DOSGBULLET_BUILD_APPLICATIONS=OFF
  -DOSGBULLET_BUILD_EXAMPLES=OFF
  -DOSGBULLET_INSTALL_DATA=OFF
  #----------
  # NOTE: on linux, the osgWorks config files go in the lib or lib64 directory depending on the bitness.
  # This means we will need to take this into accout when building for 32 bit linux.
  #osgWorks
  -DosgWorks_DIR:PATH=${CMAKE_BINARY_DIR}/modules/omegaOsg/osgWorks-prefix/src/osgWorks-build/lib
)

# NOTE: On windows, we explicitly list all the required bullet and osg libraries,
# since using the library finding modules does not work correctly in debug builds.
# the reason is that even for debug builds, release versions of bullet and osg are
# expected to be found in the installation dirs, making it impossible to generate
# a debug build without building a release version first (which is annoying)
if(WIN32)
        # The OSGWORKS_STATIC preprocessor definition tells osgBullet that
        # we are using the static version of osgWorks.
        set(OSGBullet_CXX_FLAGS "${CMAKE_CXX_FLAGS} /D\"OSGWORKS_STATIC\"")
        set(OSGBULLET_ARGS
                #-DCMAKE_SHARED_LINKER_FLAGS:STRING="${CMAKE_SHARED_LINKER_FLAGS} /NODEFAULTLIB:msvcprt.lib /NODEFAULTLIB:libcpmt.lib"
                #-DCMAKE_LINKER_FLAGS:STRING="${CMAKE_LINKER_FLAGS} /NODEFAULTLIB:libcpmt.lib /NODEFAULTLIB:msvcprt.lib"
                -DCMAKE_CXX_FLAGS:STRING=${OSGBullet_CXX_FLAGS}
                -DCMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG:PATH=${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG}
                -DCMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE:PATH=${CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE}
                #Bullet
                -DBulletInstallType:STRING=${BulletInstallType}
                -DBulletSourceRoot:PATH=${CMAKE_BINARY_DIR}/modules/omegaOsg/bullet-prefix/src/bullet
                -DBulletBuildRoot:PATH=${CMAKE_BINARY_DIR}/modules/omegaOsg/bullet-prefix/src/bullet-build
                -DBULLET_INCLUDE_DIR:PATH=${BULLET_INCLUDES}
                #BulletCollision
                -DBULLET_COLLISION_LIBRARY:PATH=${BulletCollision_LIBRARY}
                -DBULLET_COLLISION_LIBRARY_DEBUG:PATH=${BulletCollision_LIBRARY_DEBUG}
                #BulletDynamics
                -DBULLET_DYNAMICS_LIBRARY:PATH=${BulletDynamics_LIBRARY}
                -DBULLET_DYNAMICS_LIBRARY_DEBUG:PATH=${BulletDynamics_LIBRARY_DEBUG}
                #BulletMath
                -DBULLET_MATH_LIBRARY:PATH=${LinearMath_LIBRARY}
                -DBULLET_MATH_LIBRARY_DEBUG:PATH=${LinearMath_LIBRARY_DEBUG}
                #BulletSoftBody
                -DBULLET_SOFTBODY_LIBRARY:PATH=${BulletSoftBody_LIBRARY}
                -DBULLET_SOFTBODY_LIBRARY_DEBUG:PATH=${BulletSoftBody_LIBRARY_DEBUG}
                #osg
                -DOSGInstallType:STRING="Alternate Install Location"
                -DOSGInstallLocation:PATH=${OSG_BINARY_DIR}
                -DOSG_INCLUDE_DIR:PATH=${OSG_INCLUDES}
                -DOSG_LIBRARY:PATH=${osg_LIBRARY}
                -DOSG_LIBRARY_DEBUG:PATH=${osg_LIBRARY_DEBUG}
		-DOSG_DIR:PATH=${OSG_INSTALL_DIR}
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
                #osgShadow
                -DOSGSHADOW_INCLUDE_DIR:PATH=${OSG_INCLUDES}
                -DOSGSHADOW_LIBRARY:PATH=${osgShadow_LIBRARY}
                -DOSSHADOW_LIBRARY_DEBUG:PATH=${osgShadow_LIBRARY_DEBUG}
                #openthreads
                -DOPENTHREADS_INCLUDE_DIR:PATH=${OSG_INCLUDES}
                -DOPENTHREADS_LIBRARY:PATH=${OpenThreads_LIBRARY}
                -DOPENTHREADS_LIBRARY_DEBUG:PATH=${OpenThreads_LIBRARY_DEBUG}
                ${OSGBULLET_ARGS}
        )
elseif(APPLE)
		set(OsgInstallType "Source And Build Tree")
        #set(OSGBullet_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DOSGWORKS_STATIC")
        set(OSGBULLET_ARGS
        		-DCMAKE_CXX_FLAGS:STRING=${CMAKE_CXX_FLAGS}
                #osg
                -DOSGInstallType:STRING=${OsgInstallType}
                -DOSGSourceRoot:STRING=${CMAKE_BINARY_DIR}/modules/omegaOsg/osg-prefix/src/osg
                -DOSGBuildRoot:STRING=${CMAKE_BINARY_DIR}/modules/omegaOsg/osg-prefix/src/osg-build
		#OSG_DIR needs to avoid OSGWORKS_LIB_SUFFIX on osx.
		-DOSG_DIR:PATH=${OSG_INSTALL_DIR}
                #Bullet
                -DBulletInstallType:STRING=${BulletInstallType}
                -DBulletSourceRoot:STRING=${CMAKE_BINARY_DIR}/modules/omegaOsg/bullet-prefix/src/bullet
                -DBulletBuildRoot:STRING=${CMAKE_BINARY_DIR}/modules/omegaOsg/bullet-prefix/src/bullet-build
                -DBulletInstallLocation=${CMAKE_BINARY_DIR}/modules/omegaOsg/bullet-prefix/src/bullet-install
                -DCMAKE_RUNTIME_OUTPUT_DIRECTORY:PATH=${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG}/osg/osgPlugins-3.3.0
                ${OSGBULLET_ARGS}
        )

else()
        set(OsgInstallType "Source And Build Tree")
        #set(OSGBullet_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DOSGWORKS_STATIC")
        set(OSGBULLET_ARGS
                #osg
                -DOSGInstallType:STRING=${OsgInstallType}
                -DOSGSourceRoot:STRING=${CMAKE_BINARY_DIR}/modules/omegaOsg/osg-prefix/src/osg
                -DOSGBuildRoot:STRING=${CMAKE_BINARY_DIR}/modules/omegaOsg/osg-prefix/src/osg-build
		-DOSG_DIR:PATH=${OSG_INSTALL_DIR}/${OSGWORKS_LIB_SUFFIX}
                #Bullet
                -DBulletInstallType:STRING=${BulletInstallType}
                -DBulletSourceRoot:STRING=${CMAKE_BINARY_DIR}/modules/omegaOsg/bullet-prefix/src/bullet
                -DBulletBuildRoot:STRING=${CMAKE_BINARY_DIR}/modules/omegaOsg/bullet-prefix/src/bullet-build
                -DBulletInstallLocation=${CMAKE_BINARY_DIR}/modules/omegaOsg/bullet-prefix/src/bullet-install
                -DCMAKE_RUNTIME_OUTPUT_DIRECTORY:PATH=${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG}/osg/osgPlugins-3.3.0
                ${OSGBULLET_ARGS}
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
