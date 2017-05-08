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

# Static libraries for different architectures live in different directories
windows {
    contains(QT_ARCH, i386) {
        ARCH = x32
    } else {
        ARCH = x64
    }
} else:macx {
    # FIXME: implement
} else:linux:!android {
    # FIXME: implement
} else:android {
    # TODO: only armeabi-v7a and x86 are currently supported
    contains(QT_ARCH, arm) {
        ARCH = armeabi-v7a
    } else:contains(QT_ARCH, i386) {
        ARCH = x86
    } else {
        error("Unsupported OS")
    }
} else:ios {
    # FIXME: implement
} else {
    error("Unsupported OS")
}

# ... and have different suffixes for debug and release modes
CONFIG(debug, debug|release) {
    SUFFIX = d
}

# FIXME: check whether Windows version can be build without external OpenSSL

# Link with:
#   1) OpenSSL library
#   2) cURL library (compiled with OpenSSL support)
#   3) Gumbo HTML5 parser library
DEFINES += CURL_STATICLIB
INCLUDEPATH += $$PWD/curl/include
INCLUDEPATH += $$PWD/gumbo-parser/src

windows {
    QMAKE_LFLAGS_DEBUG += /ignore:4099
    LIBS += -ladvapi32 -luser32 -lgdi32 -lws2_32 -lwsock32 -lWldap32

    # OpenSSL
    LIBS += -L$$PWD/libs/openssl/$$APP_PLATFORM/VC14/$$ARCH/$$buildmode
    # cURL
    LIBS += -L$$PWD/libs/curl/$$APP_PLATFORM/VC14/$$ARCH/$$buildmode
    # Gumbo
    LIBS += -L$$PWD/libs/gumbo-parser/$$APP_PLATFORM/VC14/$$ARCH/$$buildmode

    LIBS += -llibeay32 -lssleay32
    LIBS += -llibcurl$$SUFFIX
    LIBS += -lgumbo-parser
} else:macx {
    # FIXME: implement
} else:linux:!android {
    # FIXME: implement
} else:android {
    # cURL
    LIBS += -L$$PWD/libs/curl/$$APP_PLATFORM/$$ARCH
    # Gumbo
    LIBS += -L$$PWD/libs/gumbo-parser/$$APP_PLATFORM/$$ARCH/$$buildmode

    LIBS += -lcurl
    LIBS += -lgumbo-parser
} else:ios {
    # FIXME: implement
} else {
    error("Unsupported OS")
}

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment
include(deployment.pri)
