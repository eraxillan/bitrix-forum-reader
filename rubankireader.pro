TEMPLATE = app

CONFIG -= debug_and_release debug_and_release_target

# Build mode (release by default)
buildmode = release
CONFIG(debug, debug|release):buildmode = debug

# FIXME: add x64 build mode support
#contains(QT_ARCH, i386) {

APP_PLATFORM = $$first( $$list( $$QMAKE_PLATFORM ) )
APP_ARCH = $$first( $$list( $$QT_ARCH ) )
APP_COMPILER = $$first( $$list( $$QMAKE_COMPILER ) )
APP_BUILD_DIR = __BUILD__/$${buildmode}/$${APP_PLATFORM}-$${APP_ARCH}-$${APP_COMPILER}

# Output directories setup
DESTDIR     = $${APP_BUILD_DIR}
UI_DIR      = $${APP_BUILD_DIR}
OBJECTS_DIR = $${APP_BUILD_DIR}
MOC_DIR     = $${APP_BUILD_DIR}

QT += qml quick widgets network
android: QT += androidextras

CONFIG += c++11

SOURCES += main.cpp \
    src/network/filedownloader.cpp \
    src/qml_backend/useravatarimageprovider.cpp \
    src/website_backend/gumboparserimpl.cpp \
    src/qml_backend/forumreader.cpp \
    src/website_backend/websiteinterface.cpp \
    src/website_backend/qtgumbonode.cpp

HEADERS += \
    src/network/filedownloader.h \
    src/qml_backend/useravatarimageprovider.h \
    src/website_backend/gumboparserimpl.h \
    src/website_backend/websiteinterface.h \
    src/qml_backend/forumreader.h \
    src/website_backend/html_tag.h \
    src/website_backend/qtgumbonode.h \
    src/website_backend/qtgumbodocument.h

RESOURCES += qml.qrc

# Link with gumbo-parser library
INCLUDEPATH += $$PWD/gumbo-parser/src
LIBS        += -L$$PWD/gumbo-parser/$${buildmode} -lgumbo-parser

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
include(deployment.pri)
