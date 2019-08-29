TARGET = Qt_TextViewer

QT = core gui

QMAKE_CXXFLAGS += -pedantic

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

SOURCES += \
    gui.cpp \
    main.cpp

HEADERS += \
    gui.h
