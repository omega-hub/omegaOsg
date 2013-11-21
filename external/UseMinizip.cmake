if(WIN32)
	# On windows we are lazy. Just download precompiled libs. 
	# This is specific to Visual Studio 2010, Win32.
  set(EXTLIB_NAME minizip)
  set(EXTLIB_TGZ ${CMAKE_BINARY_DIR}/${EXTLIB_NAME}.tar.gz)
  set(EXTLIB_DIR ${CMAKE_BINARY_DIR}/minizip/)
	
  if(NOT EXISTS ${EXTLIB_DIR})
  	message(STATUS "Unziping Minizip library")
  	execute_process(COMMAND ${CMAKE_COMMAND} -E tar xzf ${EXTLIB_TGZ} WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
  endif(NOT EXISTS ${EXTLIB_DIR})
	
  set(MINIZIP_INCLUDE_DIR ${EXTLIB_DIR}/include CACHE INTERNAL "")
  set(MINIZIP_LIBRARY  ${EXTLIB_DIR}/lib/win32/zlibwapi.lib CACHE INTERNAL "")
	
	# create phony target minizip
  add_custom_target(minizip)
	# Copy the dlls into the target directories
  file(COPY ${EXTLIB_DIR}/lib/win32 DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG} PATTERN "*.dll")
  file(COPY ${EXTLIB_DIR}/lib/win32 DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE} PATTERN "*.dll")
else()

    set(MINIZIP_NAME minizip)
    set(MINIZIP_DIR ${CMAKE_BINARY_DIR}/modules/omegaOsg/minizip CACHE INTERNAL "")

    if(NOT EXISTS ${MINIZIP_DIR})
        message(STATUS "Unpacking minizip.")
        execute_process(COMMAND ${CMAKE_COMMAND} -E tar xzvf ${CMAKE_CURRENT_LIST_DIR}/minizip.tar.gz WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/modules/omegaOsg)
    endif()

    # create phony target minizip
    add_custom_target(minizip)
    
    if(APPLE)
        set(LIB_SUFFIX dylib)
        set(PATH_PREFIX osx)
    else()
        set(LIB_SUFFIX so)
        set(PATH_PREFIX linux64)
    endif()
 
    set(MINIZIP_INCLUDE_DIR ${MINIZIP_DIR}/${PATH_PREFIX}/include CACHE INTERNAL "")
    set(MINIZIP_LIBRARY ${MINIZIP_DIR}/lib/${PATH_PREFIX}/lib/libminizip.${LIB_SUFFIX} CACHE INTERNAL "")
    set(MINIZIP_LIBRARY_PATH ${MINIZIP_DIR}/lib/${PATH_PREFIX}/lib CACHE INTERNAL "")
endif()
