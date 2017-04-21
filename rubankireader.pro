TEMPLATE = app

CONFIG -= debug_and_release debug_and_release_target

# Build mode (release by default)
buildmode = release
CONFIG(debug, debug|release):buildmode = debug

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

QT += qml quick quickcontrols2
QT += multimedia concurrent
android: QT += androidextras

CONFIG += c++11

SOURCES += \
    common/filedownloader.cpp               \
    website_backend/gumboparserimpl.cpp     \
    website_backend/qtgumbonode.cpp         \
    website_backend/websiteinterface.cpp    \
    qml_frontend/forumreader.cpp            \
    qml_frontend/main.cpp                   \
    tests/gumboparserimpl_tests.cpp

HEADERS += \
    common/filedownloader.h                 \
    common/resultcode.h                     \
    website_backend/gumboparserimpl.h       \
    website_backend/qtgumbonode.h           \
    website_backend/websiteinterface.h      \
    website_backend/html_tag.h              \
    website_backend/qtgumbodocument.h       \
    qml_frontend/forumreader.h

RESOURCES += qml_frontend/qml.qrc

# Link with curl library
windows {
    QT += network
} else {
    #INCLUDEPATH +=
    LIBS += -lcurl
}

# Link with gumbo-parser library
INCLUDEPATH += $$PWD/gumbo-parser/src
LIBS += -L$${GUMBO_BUILD_DIR} -lgumbo-parser

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
include(deployment.pri)
