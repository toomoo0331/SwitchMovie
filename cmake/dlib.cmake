FIND_PATH(DLIB_INCLUDE_DIRS NAMES dlib/opencv.h
        PATHS
        /opt/local/include
        /usr/include
        /usr/local/include
        )

FIND_LIBRARY(DLIB_LIB NAMES dlib
        PATHS
        /opt/local/lib
        /usr/local/lib
        /usr/lib
        )
IF (DLIB_INCLUDE_DIRS AND DLIB_LIB)
    SET(DLIB_FOUND TRUE)
    MESSAGE(STATUS "DLIB found")
    MESSAGE(STATUS "DLIB Include dirs:" ${DLIB_INCLUDE_DIRS})
    MESSAGE(STATUS "DLIB Library:" ${DLIB_LIB})
    include_directories(${DLIB_INCLUDE_DIRS})
    link_libraries(${DLIB_LIB})
ELSE (DLIB_INCLUDE_DIRS AND DLIB_LIB)
    MESSAGE(STATUS "DLIB was not found")
ENDIF(DLIB_INCLUDE_DIRS AND DLIB_LIB)

find_package(X11 REQUIRED)
if (X11_FOUND)
    include_directories(${X11_INCLUDE_DIR})
    MESSAGE(STATUS "X11 found")
    MESSAGE(STATUS "X11 Include dirs:" ${X11_INCLUDE_DIR})
    message(STATUS "X11 libraries : ${X11_LIBRARIES}")
    link_libraries(${X11_LIBRARIES})
else()
    message(WARNING "Could not find X11.")
endif()