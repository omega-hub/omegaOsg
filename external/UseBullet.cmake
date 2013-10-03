# This script sets up compilation of Bullet, and creates two variables:
# BULLET_INCLUDES - path to the bullet include dir. 
#		Add to include_directories
# BULLET_LIBS - path to all the debug and release bullet libraries. 
#		Add to target_link_libraries

set(BULLET_GENERATOR ${CMAKE_GENERATOR})
if(WIN32)
	set(BULLET_CXX_FLAGS ${CMAKE_CXX_FLAGS})
else()
	set(BULLET_CXX_FLAGS ${CMAKE_CXX_FLAGS} -fPIC)
endif()
ExternalProject_Add(
	bullet
	URL ${CMAKE_SOURCE_DIR}/modules/omegaOsg/external/bullet-2.81-rev2613.tar.gz
	CMAKE_GENERATOR ${BULLET_GENERATOR}
	CMAKE_ARGS 
		-DCMAKE_CXX_FLAGS:STRING=${BULLET_CXX_FLAGS}
		-DCMAKE_BUILD_TYPE:STRING=Release
		-DBUILD_AMD_OPENCL_DEMOS=OFF
		-DBUILD_CPU_DEMOS=OFF
		-DBUILD_DEMOS=OFF
		INSTALL_COMMAND ""
	)
set_target_properties(bullet PROPERTIES FOLDER "3rdparty")

set(BULLET_BASE_DIR ${CMAKE_BINARY_DIR}/src/bullet-prefix/src)

# This is wrong: The include dir for a build / source install is the
# one in the next line. 
set(BULLET_INCLUDES ${BULLET_BASE_DIR}/bullet/include)
include_directories(${CMAKE_BINARY_DIR}/modules/omegaOsg/bullet-prefix/src/bullet/src)

set(BULLET_LIB_DIR ${BULLET_BASE_DIR}/bullet-build/lib)
set(BULLET_COMPONENTS BulletDynamics BulletSoftBody BulletCollision LinearMath)
if(OMEGA_OS_WIN)
	foreach( C ${BULLET_COMPONENTS})
		set(${C}_LIBRARY ${BULLET_LIB_DIR}/Release/${C}.lib)
		set(${C}_LIBRARY_DEBUG ${BULLET_LIB_DIR}/Debug/${C}_Debug.lib)
		set(BULLET_LIBS ${BULLET_LIBS} optimized ${${C}_LIBRARY} debug ${${C}_LIBRARY_DEBUG})
	endforeach()
elseif(OMEGA_OS_LINUX)
	# on Linux, libs end up in a directory with theyr same name, instead of the common 'lib' dir.
    foreach( C ${BULLET_COMPONENTS} )
		set(${C}_LIBRARY ${BULLET_BASE_DIR}/bullet-build/src/${C}/lib${C}.a)
		set(${C}_LIBRARY_DEBUG ${BULLET_BASE_DIR}/bullet-build/src/${C}/lib${C}.a)
		set(BULLET_LIBS ${BULLET_LIBS} optimized ${${C}_LIBRARY} debug ${${C}_LIBRARY_DEBUG})
	endforeach()
elseif(APPLE)
	foreach( C ${BULLET_COMPONENTS} )
		set(${C}_LIBRARY ${BULLET_BASE_DIR}/bullet-build/src/${C}/lib${C}.a)
		set(${C}_LIBRARY_DEBUG ${BULLET_BASE_DIR}/bullet-build/src/${C}/lib${C}.a)
		set(BULLET_LIBS ${BULLET_LIBS} optimized ${${C}_LIBRARY} debug ${${C}_LIBRARY_DEBUG})
	endforeach()
endif(OMEGA_OS_WIN)
