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
APP_BUILD_DIR = __BUILD__/client/$${buildmode}/$${APP_PLATFORM}-$${APP_ARCH}-$${APP_COMPILER}

GUMBO_BUILD_DIR = __BUILD__/gumbo/$${buildmode}/$${APP_PLATFORM}-$${APP_ARCH}-$${APP_COMPILER}

# Output directories setup
DESTDIR     = $${APP_BUILD_DIR}
UI_DIR      = $${APP_BUILD_DIR}
OBJECTS_DIR = $${APP_BUILD_DIR}
MOC_DIR     = $${APP_BUILD_DIR}

QT += quickcontrols2
QT += qml quick widgets network multimedia
android: QT += androidextras

CONFIG += c++11

SOURCES += \
    common/filedownloader.cpp               \
    website_backend/gumboparserimpl.cpp     \
    website_backend/qtgumbonode.cpp         \
    website_backend/websiteinterface.cpp    \
    qml_frontend/forumreader.cpp            \
    qml_frontend/main.cpp

HEADERS += \
    common/filedownloader.h                 \
    website_backend/gumboparserimpl.h       \
    website_backend/qtgumbonode.h           \
    website_backend/websiteinterface.h      \
    website_backend/html_tag.h              \
    website_backend/qtgumbodocument.h       \
    qml_frontend/forumreader.h

RESOURCES += qml_frontend/qml.qrc

# Link with gumbo-parser library
INCLUDEPATH += $$PWD/gumbo-parser/src
LIBS += -L$${GUMBO_BUILD_DIR} -lgumbo-parser

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
include(deployment.pri)
