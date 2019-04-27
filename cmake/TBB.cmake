FIND_PATH(TBB_INCLUDE_DIRS NAMES tbb/parallel_for_each.h
        PATHS
        /opt/local/include
        /usr/include
        /usr/local/include
        )

FIND_LIBRARY(TBB_LIB NAMES tbb
        PATHS
        /opt/local/lib
        /usr/local/lib
        /usr/lib
        )
FIND_LIBRARY(TBB_MALLOC_LIB NAMES tbbmalloc
        PATHS
        /opt/local/lib
        /usr/local/lib
        /usr/lib
        )


SET(TBB_LIBRARIES ${TBB_LIB} ${TBB_MALLOC_LIB})

IF (TBB_INCLUDE_DIRS AND TBB_LIBRARIES)
    SET(TBB_FOUND TRUE)
    MESSAGE(STATUS "TBB found")
    MESSAGE(STATUS "TBB Include dirs:" ${TBB_INCLUDE_DIRS})
    MESSAGE(STATUS "TBB Library:" ${TBB_LIBRARIES})
    include_directories(${TBB_INCLUDE_DIRS})
    link_libraries(${TBB_LIBRARIES})
ELSE (TBB_INCLUDE_DIRS AND TBB_LIBRARIES)
    MESSAGE(STATUS "TBB was not found")
ENDIF(TBB_INCLUDE_DIRS AND TBB_LIBRARIES)