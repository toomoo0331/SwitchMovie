if (USE_STATIC_BOOST)
    set(Boost_USE_STATIC_LIBS ON)
endif ()

set(Boost_MULTITHREADED ON)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)

find_package(Boost REQUIRED COMPONENTS program_options thread system timer filesystem signals)

if (Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
#    link_directories (${Boost_LIBRARY_DIRS})
    link_libraries(${Boost_LIBRARIES})

    message(STATUS "Boost version: ${Boost_MAJOR_VERSION}.${Boost_MINOR_VERSION}.${Boost_SUBMINOR_VERSION}")
    message(STATUS "Boost include dir: ${Boost_INCLUDE_DIR}")
    message(STATUS "Boost include dirs: ${Boost_INCLUDE_DIRS}")
    message(STATUS "Boost library dir: ${BOOST_LIBRARYDIR}")
    message(STATUS "Boost libraries: ${Boost_LIBRARIES}")
else()
    message(WARNING "Could not find Boost.")
endif()
