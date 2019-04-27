#set(realsense2_DIR ~/Library/librealsense/build)

#find_package(realsense2 REQUIRED)

#include_directories(${realsense2_INCLUDE_DIR})
#link_directories(${realsense2_LIBRARY})


#message(STATUS "realsense2 include dirs : ${realsense2_INCLUDE_DIR}")
#message(STATUS "realsense2 libraries : ${realsense2_LIBRARY}")

include_directories("/usr/local/include")
link_directories("usr/loca/lib")

message(STATUS "realsense2 libraries : ")