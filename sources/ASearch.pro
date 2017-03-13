#-------------------------------------------------
#
# Project created by QtCreator 2011-02-26T12:08:02
#
#-------------------------------------------------

TARGET = ASearch
CONFIG   += console
CONFIG   -= app_bundle
CONFIG   += c++14
TEMPLATE = app

win32 {
QMAKE_LFLAGS += -static -static-libgcc -static-libstdc++
}

QMAKE_CXXFLAGS_DEBUG *= -pg
QMAKE_LFLAGS_DEBUG *= -pg

SOURCES += \
    tinyxmlparser.cpp \
    tinyxmlerror.cpp \
    tinyxml.cpp \
    tinystr.cpp \
    xmllogger.cpp \
    isearch.cpp \
    mission.cpp \
    map.cpp \
    list.cpp \
    config.cpp \
    astar.cpp \
    asearch.cpp \
    environmentoptions.cpp \
    jpsearch.cpp \
    thetastar.cpp \
    rstar.cpp \
    rjpsearch.cpp

HEADERS += \
    tinyxml.h \
    tinystr.h \
    node.h \
    gl_const.h \
    xmllogger.h \
    isearch.h \
    mission.h \
    map.h \
    ilogger.h \
    list.h \
    config.h \
    astar.h \
    searchresult.h \
    environmentoptions.h \
    jpsearch.h \
    opencontainer.h \
    thetastar.h \
    rstar.h \
    rjpsearch.h
