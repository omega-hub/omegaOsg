add_definitions( -D_CRT_SECURE_NO_WARNINGS -DOOSG_EXPORTING)

set( srcs 
		OsgModule.cpp
		OsgRenderPass.cpp
		OsgSceneObject.cpp
		SceneView.cpp
		ReaderFreeImage.cpp
		)
		
SET( headers 
		# omega
		${CMAKE_SOURCE_DIR}/include/omegaOsg/oosgbase.h
		${CMAKE_SOURCE_DIR}/include/omegaOsg/OsgModule.h
		${CMAKE_SOURCE_DIR}/include/omegaOsg/OsgRenderPass.h
		${CMAKE_SOURCE_DIR}/include/omegaOsg/OsgSceneObject.h
		${CMAKE_SOURCE_DIR}/include/omegaOsg/SceneView.h
		${CMAKE_SOURCE_DIR}/include/omegaOsg/ReaderFreeImage.h
        ) 
# add the freetype plugin directly to omegaOsg. The freetype plugin is not compiled with
# openscenegraph since it can't easily find the freetype library dependency. Finding freetype
# would require forwarding an environment variable (FREETYPE_DIR) to the external project
# osg, which is not currently supported by cmake. 
# see http://www.cmake.org/pipermail/cmake/2011-October/046798.html
#So adding the freetype reader to omegaOsg is the easiest solution for now.
set(freetypeSrcs 
	freetype/FreeTypeFont.cpp
	freetype/FreeTypeFont.h
	freetype/FreeTypeLibrary.cpp
	freetype/FreeTypeLibrary.h
	freetype/ReaderWriterFreeType.cpp)

# optional modules        
set(OMEGAOSG_USE_FBX true CACHE BOOL "Enables Fbx support for omegaOsg.")
if(OMEGAOSG_USE_FBX)
	include(${CMAKE_SOURCE_DIR}/external/UseFbx.cmake)
	SET( srcs ${srcs}
			fbx/fbxRAnimation.cpp
			fbx/fbxRCamera.cpp
			fbx/fbxRLight.cpp
			fbx/fbxRMesh.cpp
			fbx/fbxRNode.cpp
			fbx/ReaderWriterFBX.cpp
			fbx/WriterCompareTriangle.cpp
			fbx/WriterNodeVisitor.cpp
			fbx/fbxMaterialToOsgStateSet.cpp
			)
			
	SET( headers ${headers}
			fbx/fbxMaterialToOsgStateSet.h
			fbx/fbxReader.h
			fbx/ReaderWriterFBX.h
			fbx/WriterCompareTriangle.h
			fbx/WriterNodeVisitor.h
			) 
endif()

###############################################################################
set(OMEGAOSG_USE_INVENTOR false CACHE BOOL "Enables Inventor (.vrml, .iv) support for omegaOsg. Needs the Coin3d library installed")
if(OMEGAOSG_USE_INVENTOR)
    find_package(Inventor)
	add_definitions(-DCOIN_NOT_DLL)
    if(INVENTOR_FOUND)
		set( srcs  ${srcs}
				Inventor/ConvertToInventor.cpp
				Inventor/ConvertFromInventor.cpp
				Inventor/PendulumCallback.cpp
				Inventor/ReaderWriterIV.cpp
				Inventor/ShuttleCallback.cpp
			)
		set( headers  ${headers}
				Inventor/ConvertToInventor.h
				Inventor/ConvertFromInventor.h
				Inventor/PendulumCallback.h
				Inventor/ReaderWriterIV.h
				Inventor/ShuttleCallback.h
			)
		#ADD_DEFINITIONS(-DCOIN_DLL)
		include_directories(${INVENTOR_INCLUDE_DIR})
	endif()
endif()

source_group(omegaOsg REGULAR_EXPRESSION omegaOsg/)		
source_group(fbx REGULAR_EXPRESSION omegaOsg/fbx/)		
source_group(Inventor REGULAR_EXPRESSION omegaOsg/Inventor/)		
source_group(Freetype REGULAR_EXPRESSION omegaOsg/freetype/)		

IF(WIN32)
    SET(TARGET_EXTERNAL_LIBRARIES wininet)
endif(WIN32)

IF(APPLE)
    FIND_LIBRARY(SYS_CONFIG_LIBRARY SystemConfiguration )
    MARK_AS_ADVANCED(SYS_CONFIG_LIBRARY)
    SET(TARGET_EXTERNAL_LIBRARIES iconv ${SYS_CONFIG_LIBRARY})
endif(APPLE)

###############################################################################
# Setup compile info
add_library( omegaOsg SHARED ${srcs} ${headers})
add_dependencies(omegaOsg osg osgWorks)

###############################################################################
# Setup link info.
target_link_libraries(omegaOsg 
	omega freetype ${OSG_LIBS} ${FBX_LIBNAME} ${TARGET_EXTERNAL_LIBRARIES})

if(OMEGAOSG_USE_INVENTOR)
    if(INVENTOR_FOUND)
		target_link_libraries(omegaOsg ${INVENTOR_LIBRARY})
	endif()
endif()