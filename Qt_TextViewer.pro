TARGET = Qt_TextViewer

QT = core gui

QMAKE_CXXFLAGS += -pedantic

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

SOURCES += \
    codeedit.cpp \
    gui.cpp \
    main.cpp

HEADERS += \
    codeedit.h \
    dtl/Diff.hpp \
    dtl/Diff3.hpp \
    dtl/Lcs.hpp \
    dtl/Sequence.hpp \
    dtl/Ses.hpp \
    dtl/dtl.hpp \
    dtl/functors.hpp \
    dtl/variables.hpp \
    gui.h
