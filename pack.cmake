if(WIN32)
    file(INSTALL DESTINATION ${PACKAGE_DIR}/bin
        TYPE FILE
        FILES
            ${BIN_DIR}/omegaOsg.dll
            ${BIN_DIR}/osg100-osg.dll
            ${BIN_DIR}/osg100-osgAnimation.dll
            ${BIN_DIR}/osg100-osgDB.dll
            ${BIN_DIR}/osg100-osgFX.dll
            ${BIN_DIR}/osg100-osgGA.dll
            ${BIN_DIR}/osg100-osgManipulator.dll
            ${BIN_DIR}/osg100-osgParticle.dll
            ${BIN_DIR}/osg100-osgPresentation.dll
            ${BIN_DIR}/osg100-osgShadow.dll
            ${BIN_DIR}/osg100-osgSim.dll
            ${BIN_DIR}/osg100-osgTerrain.dll
            ${BIN_DIR}/osg100-osgText.dll
            ${BIN_DIR}/osg100-osgUtil.dll
            ${BIN_DIR}/osg100-osgViewer.dll
            ${BIN_DIR}/osg100-osgVolume.dll
            ${BIN_DIR}/osg100-osgWidget.dll
            ${BIN_DIR}/osgbCollision.dll
            ${BIN_DIR}/osgbDynamics.dll
            ${BIN_DIR}/osgbInteraction.dll
            ${BIN_DIR}/ot20-OpenThreads.dll
        )
    file(INSTALL DESTINATION ${PACKAGE_DIR}/bin
        TYPE DIRECTORY
        FILES
            ${BIN_DIR}/osgPlugins-3.2.1
        )
endif()
