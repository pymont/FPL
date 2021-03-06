################################################################################
# Executable targets
SET( EXECUTABLES
)

################################################################################
SET( LIBS ${MISC_LIBRARIES} )
SET( INCLUDES ${MISC_INCLUDE_DIR} )
SET( DEFINITIONS ${MISC_DEFINITIONS} )

################################################################################
FIND_PACKAGE( CCAMERASENSOR REQUIRED )
#MESSAGE( STATUS "Found CCameraSensor, adding support. ${CCAMERASENSOR_INCLUDE_DIR}" )
SET( LIBS ${LIBS} ${CCAMERASENSOR_LIBRARIES} )
SET( INCLUDES ${INCLUDES} ${CCAMERASENSOR_INCLUDE_DIR} )
SET( DEFINITIONS ${DEFINITIONS} ${CCAMERASENSOR_DEFINITIONS} )

################################################################################
FIND_PACKAGE( OPENCV2 REQUIRED )
SET( LIBS ${LIBS} ${OpenCV2_LIBRARIES} )
SET( INCLUDES ${INCLUDES} ${OpenCV2_INCLUDE_DIR} )
SET( DEFINITIONS ${DEFINITIONS} "-DHAS_OPENCV" )

################################################################################
INCLUDE_DIRECTORIES( ${INCLUDES} )
ADD_DEFINITIONS( ${DEFINITIONS} )

################################################################################
FOREACH( EXECUTABLE ${EXECUTABLES} )
  ADD_EXECUTABLE( ${EXECUTABLE} ${EXECUTABLE}.cpp )
  TARGET_LINK_LIBRARIES( ${EXECUTABLE} ${LIBS} )
ENDFOREACH( EXECUTABLE )
