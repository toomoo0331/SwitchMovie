include_directories(../..)

SET(CALIBRATION_SRC detectPoints.cpp
        drawUtil.cpp)

# define the header files (make the headers appear in IDEs.)
FILE(GLOB CALIBRATION_HDRS *.h)

ADD_LIBRARY(calibration STATIC ${CALIBRATION_SRC} ${CALIBRATION_HDRS})

TARGET_LINK_LIBRARIES(calibration)