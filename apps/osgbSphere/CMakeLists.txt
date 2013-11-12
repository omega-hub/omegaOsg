# Source files
set(SRCS
		osgbSphere.cpp)

###############################################################################
# Setup compile info
add_executable(osgbSphere ${SRCS})
set_target_properties(osgbSphere PROPERTIES FOLDER osgBulletExamples)
target_link_libraries(osgbSphere
	omega
	omegaToolkit
	omegaOsg
	${OSGBULLET_LIBS})
