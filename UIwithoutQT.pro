QT += core opengl
QT -= gui

CONFIG += c++11

TARGET = UIwithoutQT
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp \
    obj_loader.cpp \
    mesh.cpp \
    camera.cpp \
    shader.cpp \
    display.cpp \
    db_elements.cpp \
    similarityevaluator.cpp \
    cmshapecontext.cpp

HEADERS += \
    obj_loader.h \
    mesh.h \
    transform.h \
    camera.h \
    shader.h \
    display.h \
    db_elements.h \
    similarityevaluator.h \
    cmshapecontext.h \
    material.h

INCLUDEPATH +=  /usr/include/GL
LIBS += -L/usr/lib/i386-linux-gnu -lGLEW

INCLUDEPATH +=  /usr/local/include/SDL2
LIBS += -L/usr/local/lib -lSDL2

INCLUDEPATH +=  /usr/local/include/opencv
LIBS += -L/usr/local/lib -lopencv_core -lopencv_highgui -lopencv_imgcodecs -lopencv_imgproc -lopencv_highgui
