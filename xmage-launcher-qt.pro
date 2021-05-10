QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    downloadmanager.cpp \
    main.cpp \
    mainwindow.cpp \
    settings.cpp \
    settingsdialog.cpp \
    unzipthread.cpp \
    xmageprocess.cpp

HEADERS += \
    downloadmanager.h \
    mainwindow.h \
    settings.h \
    settingsdialog.h \
    unzipthread.h \
    xmageprocess.h

FORMS += \
    mainwindow.ui \
    settingsdialog.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resources/resources.qrc

unix|win32: LIBS += -lzip
win32: LIBS += -lbz2 -llzma -lmsi
