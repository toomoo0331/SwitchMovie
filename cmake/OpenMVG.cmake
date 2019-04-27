set(OpenMVG_DIR ~/Library/openMVG_Build/openMVG_install/share/openMVG/cmake)

find_package(OpenMVG REQUIRED)

include_directories(${OPENMVG_INCLUDE_DIRS})
link_directories(${OPENMVG_LIBRARIES})


message(STATUS "OpenMVG include dirs : ${OPENMVG_INCLUDE_DIRS}")
message(STATUS "OoenMVG libraries : ${OPENMVG_LIBRARIES}")
