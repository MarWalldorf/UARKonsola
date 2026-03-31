QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    ARX_Model.cpp \
    FeedbackLoop.cpp \
    Generator.cpp \
    MenagerSymulacji.cpp \
    NetworkReceiver.cpp \
    NetworkSender.cpp \
    Regulator_PID.cpp \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    ARX_Model.h \
    FeedbackLoop.h \
    Generator.h \
    MenagerSymulacji.h \
    NQueue.h \
    NetworkReceiver.h \
    NetworkSender.h \
    Regulator_PID.h \
    mainwindow.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
