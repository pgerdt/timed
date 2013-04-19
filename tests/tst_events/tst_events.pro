TEMPLATE = app
TARGET = tst_events

QT += testlib dbus
QT -= gui

CONFIG += timed

INCLUDEPATH += ../../src/h

QMAKE_LIBDIR_FLAGS += -L../../src/lib -L../../src/voland
equals(QT_MAJOR_VERSION, 4): LIBS += -ltimed -ltimed-voland
equals(QT_MAJOR_VERSION, 5): LIBS += -ltimed-qt5 -ltimed-voland-qt5

HEADERS += tst_events.h \
           simplevolandadaptor.h

SOURCES += tst_events.cpp \
           simplevolandadaptor.cpp

target.path = $$(DESTDIR)/opt/tests/timed-tests
INSTALLS += target
