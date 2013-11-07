# Open Scene Graph configuration, libraries and applications
if(OMEGA_BUILD_OSG_LIB)
	include(external/UseOpenSceneGraph.cmake)
	include_directories(./)
	include_directories(${OSG_INCLUDES})
    add_subdirectory(src)
	add_subdirectory(apps/osgviewer)

	#add osgBullet into include
	include(external/UseOsgBullet.cmake)
	include_directories(${OSGBULLET_INCLUDES})

	add_subdirectory(apps/osgbCollision)
	add_subdirectory(apps/osgbBasicDemo)
	add_subdirectory(apps/osgbSphere)
	add_subdirectory(apps/osgbDice)
	add_subdirectory(apps/osgbSlider)

	#if(OMEGA_USE_PORTHOLE)
	#	add_subdirectory(${CMAKE_SOURCE_DIR}/examples/portholeCustomDraw ${CMAKE_BINARY_DIR}/examples/portholeCustomDraw)
	#endif()
endif()

###############################################################################
# Install commands
# osgworks
if(${OMEGA_INSTALL_DEVLIBS})
	install(DIRECTORY ${CMAKE_BINARY_DIR}/src/osgWorks-prefix/src/osgWorks-build/lib/ DESTINATION omegalib/lib)
endif()
