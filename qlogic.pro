# -------------------------------------------------
# Project created by QtCreator 2009-07-04T18:17:10
# -------------------------------------------------
QT += opengl \
    svg
TARGET = qlogic
TEMPLATE = app
SOURCES += main.cpp \
    mainwindow.cpp \
    scene.cpp \
    sceneitem.cpp \
    view.cpp \
    zoomwidget.cpp \
    line.cpp \
    inputdevice.cpp \
    outputdevice.cpp \
    logicgate.cpp \
    bubbleitem.cpp
HEADERS += mainwindow.h \
    scene.h \
    sceneitem.h \
    view.h \
    zoomwidget.h \
    line.h \
    inputdevice.h \
    outputdevice.h \
    logicgate.h \
    bubbleitem.h
FORMS += mainwindow.ui \
    zoomwidget.ui
RESOURCES += qlogic.qrc
