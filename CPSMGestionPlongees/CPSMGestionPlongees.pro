QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    Data/Database.cpp \
    Data/global.cpp \
    Data/local.cpp \
    Debug/DbVisualizer.cpp \
    Debug/LogFile.cpp \
    Debug/Utils.cpp \
    Debug/global.cpp \
    GUI/Dialog_ConfirmDiverDeletion.cpp \
    GUI/Dialog_EditFamily.cpp \
    GUI/DiveEdit.cpp \
    GUI/DiveSearch.cpp \
    GUI/DiverEdit.cpp \
    GUI/DiverSearch.cpp \
    GUI/MainWindow.cpp \
    GUI/global.cpp \
    Info/Address.cpp \
    Info/Dive.cpp \
    Info/Diver.cpp \
    Info/User.cpp \
    Info/global.cpp \
    main.cpp

HEADERS += \
    Data/Database.hpp \
    Debug/DbVisualizer.h \
    Debug/LogFile.hpp \
    Debug/Utils.h \
    Debug/global.hpp \
    GUI/Dialog_ConfirmDiverDeletion.h \
    GUI/Dialog_EditFamily.h \
    GUI/DiveEdit.h \
    GUI/DiveSearch.hpp \
    GUI/DiverEdit.h \
    GUI/DiverSearch.hpp \
    GUI/MainWindow.h \
    GUI/constSettings.hpp \
    GUI/global.hpp \
    Info/Address.h \
    Info/Dive.h \
    Info/Diver.h \
    Info/User.h \
    Info/global.hpp \
    global.hpp

FORMS += \
    Debug/DbVisualizer.ui \
    GUI/Dialog_ConfirmDiverDeletion.ui \
    GUI/Dialog_EditFamily.ui \
    GUI/DiveEdit.ui \
    GUI/DiveSearch.ui \
    GUI/DiverEdit.ui \
    GUI/DiverSearch.ui \
    GUI/MainWindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    Ressources/Images/Icons/icon.ico \
    Ressources/softIcon.rc

RC_FILE = Ressources/softIcon.rc
