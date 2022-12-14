QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

RC_ICONS = icon.ico

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    huffdata.cpp \
    hufftree.cpp \
    main.cpp \
    mainwindow.cpp \
    minheap.cpp

HEADERS += \
    huffdata.h \
    hufftree.h \
    mainwindow.h \
    minheap.h \
    treenode.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target


DEFINES += QT_NO_WARNING_OUTPUT\
           QT_NO_DEBUG_OUTPUT
