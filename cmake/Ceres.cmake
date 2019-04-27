find_package(Ceres)

if (Ceres_FOUND)
    message(STATUS "Ceres version: ${CERES_VERSION}")
    message(STATUS "Ceres include directory: ${CERES_INCLUDE_DIR}")
    include_directories(${CERES_INCLUDE_DIR})
    include_directories(${EIGEN_INCLUDE_DIR})
    link_libraries(${CERES_LIBRARIES})
else()
    message(WARNING "Could not find Ceres.")
endif()