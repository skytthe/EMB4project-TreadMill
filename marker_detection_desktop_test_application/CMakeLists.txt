cmake_minimum_required (VERSION 2.6) 

PROJECT(MarkerDetectorTest)

# Requires OpenCV
FIND_PACKAGE( OpenCV REQUIRED )

SET(SRC
    main_desktop.cpp
    TinyLA.cpp
    Marker.cpp
    GeometryTypes.cpp
)

ADD_EXECUTABLE( ${PROJECT_NAME} ${SRC} )
TARGET_LINK_LIBRARIES( ${PROJECT_NAME}  ${OpenCV_LIBS} )
