set(OSG_VERSION 3.3.8)
set(OSG_FILE_VERSION 122)

if(WIN32)
    file(INSTALL DESTINATION ${PACKAGE_DIR}/bin
        TYPE FILE
        FILES
            ${BIN_DIR}/omegaOsg.dll
            ${BIN_DIR}/osg${OSG_FILE_VERSION}-osg.dll
            ${BIN_DIR}/osg${OSG_FILE_VERSION}-osgAnimation.dll
            ${BIN_DIR}/osg${OSG_FILE_VERSION}-osgDB.dll
            ${BIN_DIR}/osg${OSG_FILE_VERSION}-osgFX.dll
            ${BIN_DIR}/osg${OSG_FILE_VERSION}-osgGA.dll
            ${BIN_DIR}/osg${OSG_FILE_VERSION}-osgManipulator.dll
            ${BIN_DIR}/osg${OSG_FILE_VERSION}-osgParticle.dll
            ${BIN_DIR}/osg${OSG_FILE_VERSION}-osgPresentation.dll
            ${BIN_DIR}/osg${OSG_FILE_VERSION}-osgShadow.dll
            ${BIN_DIR}/osg${OSG_FILE_VERSION}-osgSim.dll
            ${BIN_DIR}/osg${OSG_FILE_VERSION}-osgTerrain.dll
            ${BIN_DIR}/osg${OSG_FILE_VERSION}-osgText.dll
            ${BIN_DIR}/osg${OSG_FILE_VERSION}-osgUtil.dll
            ${BIN_DIR}/osg${OSG_FILE_VERSION}-osgViewer.dll
            ${BIN_DIR}/osg${OSG_FILE_VERSION}-osgVolume.dll
            ${BIN_DIR}/osg${OSG_FILE_VERSION}-osgWidget.dll
            ${BIN_DIR}/osgbCollision.dll
            ${BIN_DIR}/osgbDynamics.dll
            ${BIN_DIR}/osgbInteraction.dll
            ${BIN_DIR}/ot20-OpenThreads.dll
        )
    file(INSTALL DESTINATION ${PACKAGE_DIR}/bin
        TYPE DIRECTORY
        FILES
            ${BIN_DIR}/osgPlugins-${OSG_VERSION}
        )
elseif(APPLE)
	
	set(COMPONENTS
		osg
		osgAnimation
		osgDB
		osgFX
		osgGA
		osgManipulator
		osgParticle
		osgPresentation
		osgShadow
		osgSim
		osgTerrain
		osgText
		osgUtil
		osgViewer
		osgWidget)
		
	foreach(component ${COMPONENTS})
		file(INSTALL DESTINATION ${PACKAGE_DIR}/bin
			TYPE FILE
			FILES
				${BIN_DIR}/lib${component}.${OSG_FILE_VERSION}.dylib
				${BIN_DIR}/lib${component}.${OSG_VERSION}.dylib
				${BIN_DIR}/lib${component}.dylib
				)
	endforeach()
	
    file(INSTALL DESTINATION ${PACKAGE_DIR}/bin
        TYPE FILE
        FILES
            ${BIN_DIR}/libomegaOsg.dylib
            ${BIN_DIR}/libfbxsdk.dylib
            ${BIN_DIR}/libOpenThreads.20.dylib
            ${BIN_DIR}/libOpenThreads.3.3.0.dylib
        )
    file(INSTALL DESTINATION ${PACKAGE_DIR}/bin
        TYPE DIRECTORY
        FILES
            ${BIN_DIR}/osgPlugins-${OSG_VERSION}
        )
endif()
