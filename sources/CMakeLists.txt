project(PathfindingRStar)

SET(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "--std=c++11")

set(SOURCES
    tinyxmlparser.cpp
    tinyxmlerror.cpp
    tinyxml.cpp
    tinystr.cpp
    xmllogger.cpp
    isearch.cpp
    mission.cpp
    map.cpp
    list.cpp
    config.cpp
    astar.cpp
    asearch.cpp
    environmentoptions.cpp
    jpsearch.cpp
    thetastar.cpp
    rstar.cpp
    rjpsearch.cpp
)

set(HEADERS
    tinyxml.h
    tinystr.h
    node.h
    gl_const.h
    xmllogger.h
    isearch.h
    mission.h
    map.h
    ilogger.h
    list.h
    config.h
    astar.h
    searchresult.h
    environmentoptions.h
    jpsearch.h
    opencontainer.h
    thetastar.h
    rstar.h
    rjpsearch.h
)

add_executable(${PROJECT_NAME} ${SOURCES})
