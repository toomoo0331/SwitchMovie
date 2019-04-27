include_directories(../3party)

SET(SW_SRC
        HeadDetectors.cpp
        SwitchMovie.cpp
        )

# define the header files (make the headers appear in IDEs.)
SET(SW_HDRS
        HeadDetectors.h
        SwitchMovie.h
        Graph.h
        ../3party/radix-heap/radix_heap.h
        )

ADD_LIBRARY(sw STATIC ${SW_SRC} ${SW_HDRS})

TARGET_LINK_LIBRARIES(sw hd)