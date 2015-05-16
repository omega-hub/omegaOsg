# This script sets up compilation of Bullet, and creates two variables:
# BULLET_INCLUDES - path to the bullet include dir.
#		Add to include_directories
# BULLET_LIBS - path to all the debug and release bullet libraries.
#		Add to target_link_libraries

set(BULLET_GENERATOR ${CMAKE_GENERATOR})

set(BULLET_CXX_FLAGS ${CMAKE_CXX_FLAGS} -fPIC)

set(BULLET_BASE_DIR ${CMAKE_BINARY_DIR}/3rdparty/bullet)


ExternalProject_Add(
	bullet
	URL ${CMAKE_SOURCE_DIR}/modules/omegaOsg/external/bullet-2.81-rev2613.tar.gz
	CMAKE_GENERATOR ${BULLET_GENERATOR}
	CMAKE_ARGS
		-DCMAKE_SHARED_LINKER_FLAGS:STRING=${CMAKE_SHARED_LINKER_FLAGS}
    -DCMAKE_CXX_FLAGS:STRING=${BULLET_CXX_FLAGS}
		-DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
		-DBUILD_AMD_OPENCL_DEMOS=OFF
		-DBUILD_CPU_DEMOS=OFF
		-DBUILD_DEMOS=OFF
		-DUSE_DX11=OFF
		-DBUILD_EXTRAS=OFF
		-DBUILD_NVIDIA_OPENCL_DEMOS=OFF
		-DBUILD_INTEL_OPENCL_DEMOS=OFF
		-DBUILD_MINICL_OPENCL_DEMOS=OFF
		-DUSE_GLUT=OFF
		-DUSE_GRAPHICAL_BENCHMARK=OFF
		-DUSE_MSVC_RUNTIME_LIBRARY_DLL=ON
		# NOTE: On Windows, windows are not set to be installed by default.
		# This flag makes sure they get installed.
		-DINSTALL_LIBS=ON
    -DPKGCONFIG_INSTALL_PREFIX=${BULLET_BASE_DIR}/install/lib/pckconfig
    -DINCLUDE_INSTALL_DIR=${BULLET_BASE_DIR}/install/include
    -DCMAKE_INSTALL_PREFIX=${BULLET_BASE_DIR}/install
  #INSTALL_COMMAND ${PLATFORM_INSTALL_COMMAND}
  
    # directories
    TMP_DIR ${CMAKE_BINARY_DIR}/3rdparty/tmp
    STAMP_DIR ${CMAKE_BINARY_DIR}/3rdparty/stamp
    DOWNLOAD_DIR ${CMAKE_BINARY_DIR}/3rdparty
    SOURCE_DIR ${BULLET_BASE_DIR}/source
    BINARY_DIR ${BULLET_BASE_DIR}/build
    INSTALL_DIR ${BULLET_BASE_DIR}/install
)

set_target_properties(bullet PROPERTIES FOLDER "3rdparty")

# This is wrong: The include dir for a build / source install is the
# one in the next line.
set(BULLET_INCLUDES ${BULLET_BASE_DIR}/source/src)

set(BULLET_LIB_DIR ${BULLET_BASE_DIR}/build/lib)
set(BULLET_COMPONENTS BulletDynamics BulletSoftBody BulletCollision LinearMath)
if(OMEGA_OS_WIN)
	foreach( C ${BULLET_COMPONENTS})
		set(${C}_LIBRARY ${BULLET_LIB_DIR}/Release/${C}.lib)
		set(${C}_LIBRARY_DEBUG ${BULLET_LIB_DIR}/Debug/${C}_Debug.lib)
		set(BULLET_LIBS ${BULLET_LIBS} optimized ${${C}_LIBRARY} debug ${${C}_LIBRARY_DEBUG})
	endforeach()
else()
	foreach( C ${BULLET_COMPONENTS} )
		set(${C}_LIBRARY ${BULLET_BASE_DIR}/build/src/${C}/lib${C}.a)
		set(${C}_LIBRARY_DEBUG ${BULLET_BASE_DIR}/build/src/${C}/lib${C}.a)
		set(BULLET_LIBS ${BULLET_LIBS} optimized ${${C}_LIBRARY} debug ${${C}_LIBRARY_DEBUG})
	endforeach()
endif(OMEGA_OS_WIN)
