QT += quick

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        main.cpp

RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH = $$PWD/../QtPiDeckCommon

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../QtPiDeckCommon/release/ -lQtPiDeckCommon
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../QtPiDeckCommon/debug/ -lQtPiDeckCommon
else:unix: LIBS += -L$$OUT_PWD/../QtPiDeckCommon/ -lQtPiDeckCommon

INCLUDEPATH += $$PWD/../QtPiDeckCommon
DEPENDPATH += $$PWD/../QtPiDeckCommon

win32:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../QtPiDeckCommon/release/QtPiDeckCommon.lib
else:win32:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../QtPiDeckCommon/debug/QtPiDeckCommon.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../QtPiDeckCommon/libQtPiDeckCommon.a
