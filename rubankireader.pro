TEMPLATE = app

# Build mode (release by default)
buildmode = release
CONFIG(debug, debug|release):buildmode = debug

# FIXME: add x64 build mode support
#contains(QT_ARCH, i386) {

# Output directories setup
DESTDIR     = $${buildmode}
UI_DIR      = $${buildmode}
OBJECTS_DIR = $${buildmode}
MOC_DIR     = $${buildmode}

QT += qml quick widgets network
android: QT += androidextras

CONFIG += c++11

SOURCES += main.cpp \
    src/network/filedownloader.cpp \
    src/qml_backend/useravatarimageprovider.cpp \
    src/website_backend/gumboparserimpl.cpp \
    src/qml_backend/forumreader.cpp

HEADERS += \
    src/network/filedownloader.h \
    src/qml_backend/useravatarimageprovider.h \
    src/website_backend/gumboparserimpl.h \
    src/website_backend/websiteinterface.h \
    src/qml_backend/forumreader.h

RESOURCES += qml.qrc

# Link with gumbo-parser library
INCLUDEPATH += $$PWD/gumbo-parser/src
LIBS        += -L$$PWD/gumbo-parser/$${buildmode} -lgumbo-parser

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
include(deployment.pri)
