QT += core gui
QT += httpserver
QT += network
QT += webenginewidgets
QT += core
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    Tabs/home.cpp \
    Tabs/newtab.cpp \
    Tabs/settings.cpp \
    main.cpp \
    mainwindow.cpp \
    olop.cpp

HEADERS += \
    Download.h \
    Tabs/home.h \
    Tabs/newtab.h \
    Tabs/settings.h \
    mainwindow.h \
    olop.hpp

FORMS += \
    Tabs/home.ui \
    Tabs/newtab.ui \
    Tabs/settings.ui \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    Linux.qrc \
    assets.qrc \
    assetsui.qrc
