################################################################################
# Executable targets
set( EXECUTABLES
  test_proj_metric
)

################################################################################
set( LIBS ${CEIGEN_LIBRARIES} )
set( INCLUDES ${CEIGEN_INCLUDE_DIR} )
set( DEFINITIONS ${CEIGEN_DEFINITIONS} )

################################################################################
find_package( OPENCV2 REQUIRED )
set( LIBS ${LIBS} ${OpenCV2_LIBRARIES} )
set( INCLUDES ${INCLUDES} ${OpenCV2_INCLUDE_DIR} )
set( DEFINITIONS ${DEFINITIONS} "-DHAS_OPENCV" )

################################################################################
find_package( MISC REQUIRED )
set( LIBS ${LIBS} ${MISC_LIBRARIES} )
set( INCLUDES ${INCLUDES} ${MISC_INCLUDE_DIR} )
set( DEFINITIONS ${DEFINITIONS} ${MISC_DEFINITIONS} )

################################################################################
include_directories( ${INCLUDES} )
add_definitions( ${DEFINITIONS} )

################################################################################
foreach( EXECUTABLE ${EXECUTABLES} )
  add_executable( ${EXECUTABLE} ${EXECUTABLE}.cpp )
  add_dependencies( ${EXECUTABLE} ceigen )
  target_link_libraries( ${EXECUTABLE} ${LIBS} )
endforeach( EXECUTABLE )
