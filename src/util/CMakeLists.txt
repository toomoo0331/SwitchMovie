#add_subdirectory(calibration)

include_directories("${PROJECT_BINARY_DIR}/src")
SET(UTIL_SRC DataLoader.cpp
        visualizer.cpp)

# define the header files (make the headers appear in IDEs.)
FILE(GLOB UTIL_HDRS *.h)

ADD_LIBRARY(util STATIC ${UTIL_SRC} ${UTIL_HDRS})

TARGET_LINK_LIBRARIES(util)