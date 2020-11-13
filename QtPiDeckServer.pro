TEMPLATE = subdirs

SUBDIRS += \
    QtPiDeckCommon \
    QtPiDeckServer

QtPiDeckServer.depends = QtPiDeckCommon
