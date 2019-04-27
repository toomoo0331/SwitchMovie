SET(hd_src
        Classifier.cpp
        FeatureComputer.cpp
        HandDetector.cpp
        LcBasic.cpp)

SET(hd_hdrs
        Classifier.h
        FeatureComputer.hpp
        HandDetector.hpp
        LcBasic.h)

add_library(hd STATIC ${hd_src} ${hd_hdrs})

add_executable(test_handtracker main.cpp)

target_link_libraries(test_handtracker hd)