if(WIN32)
	# On windows we are lazy. Just download precompiled libs. 
	# This is specific to Visual Studio 2010, Win32.
	# for other versions of GDAL look at http://www.gisinternals.com/sdk/
	# and add a new section here.
	set(EXTLIB_NAME gdal)
	set(EXTLIB_TGZ ${CMAKE_BINARY_DIR}/${EXTLIB_NAME}.tar.gz)
	set(EXTLIB_DIR ${CMAKE_BINARY_DIR}/gdal)
	
	if(NOT EXISTS ${EXTLIB_DIR})
		message(STATUS "Downloading GDAL library")
		file(DOWNLOAD "https://omegalib.googlecode.com/files/gdal.tar.gz" ${EXTLIB_TGZ} SHOW_PROGRESS)
		execute_process(COMMAND ${CMAKE_COMMAND} -E tar xzf ${EXTLIB_TGZ} WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
	endif(NOT EXISTS ${EXTLIB_DIR})
	
	set(GDAL_INCLUDE_DIR ${EXTLIB_DIR}/include CACHE INTERNAL "")
	set(GDAL_LIBRARY  ${EXTLIB_DIR}/lib/gdal_i.lib CACHE INTERNAL "")
	
	# create phony target gdal
	add_custom_target(gdal)
	# Copy the dlls into the target directories
	file(COPY ${EXTLIB_DIR}/bin/ DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG} PATTERN "*.dll")
	file(COPY ${EXTLIB_DIR}/bin/ DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE} PATTERN "*.dll")
else()
	ExternalProject_Add(
		collada
		DEPENDS minizip
        URL "http://sourceforge.net/projects/collada-dom/files/Collada%20DOM/Collada%20DOM%202.3/collada-dom-2.3.tgz"
		INSTALL_COMMAND ""
	)
	
ExternalProject_Get_Property(collada BINARY_DIR)
    # NOTE: setting the GDAL_INCLUDES as an internal cache variable, makes it accessible to other modules.
	if(APPLE)
		set(LIB_SUFFIX dylib)
	else()
		set(LIB_SUFFIX so)
	endif()

    set(COLLADA_LIBRARY  ${BINARY_DIR}/dom/src/1.4/libcollada14dom.${LIB_SUFFIX} CACHE INTERNAL "")
endif()


set_target_properties(collada PROPERTIES FOLDER "modules/omegaOsg")
