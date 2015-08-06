module_version(2.1)

# Open Scene Graph configuration, libraries and applications
include(external/UseOpenSceneGraph.cmake)
include_directories(./)
include_directories(${OSG_INCLUDES})

#add osgBullet into include
include(external/UseOsgBullet.cmake)
include_directories(${OSGBULLET_INCLUDES})

add_subdirectory(src)
add_subdirectory(apps/osgviewer)

set(BULLET_BUILD_EXAMPLES false CACHE BOOL "set to true to build some omegalib/Bullet examples")

if(BULLET_BUILD_EXAMPLES)
add_subdirectory(apps/osgbCollision)
add_subdirectory(apps/osgbBasicDemo)
add_subdirectory(apps/osgbSphere)
add_subdirectory(apps/osgbDice)
add_subdirectory(apps/osgbSlider)
endif()

#if(OMEGA_USE_PORTHOLE)
#	add_subdirectory(${CMAKE_SOURCE_DIR}/examples/portholeCustomDraw ${CMAKE_BINARY_DIR}/examples/portholeCustomDraw)
#endif()

###############################################################################
# Install commands
# osgworks
if(${OMEGA_INSTALL_DEVLIBS})
	install(DIRECTORY ${CMAKE_BINARY_DIR}/src/osgWorks-prefix/src/osgWorks-build/lib/ DESTINATION omegalib/lib)
endif()
set_target_properties(omegaOsg PROPERTIES FOLDER modules)