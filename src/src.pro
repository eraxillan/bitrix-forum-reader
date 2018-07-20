defineTest(minQtVersion) {
    maj = $$1
    min = $$2
    patch = $$3
    isEqual(QT_MAJOR_VERSION, $$maj) {
        isEqual(QT_MINOR_VERSION, $$min) {
            isEqual(QT_PATCH_VERSION, $$patch) {
                return(true)
            }
            greaterThan(QT_PATCH_VERSION, $$patch) {
                return(true)
            }
        }
        greaterThan(QT_MINOR_VERSION, $$min) {
            return(true)
        }
    }
    greaterThan(QT_MAJOR_VERSION, $$maj) {
        return(true)
    }
    return(false)
}

!minQtVersion(5, 8, 0) {
    message("Cannot build this demo with Qt version $${QT_VERSION}.")
    error("Use at least Qt 5.8.0.")
}

#######################################################################################################################

TEMPLATE = app
CONFIG -= debug_and_release debug_and_release_target

TOPDIR = $$PWD/..

#######################################################################################################################
# Platform-specific setup

macx {
    TARGET = "Bitrix Forum Reader"

    # NOTE: Qt Creator unable to run application with different bundle and executable names
#    TARGET = "bitrix-forum-reader"
#    QMAKE_APPLICATION_BUNDLE_NAME = "Bitrix Forum Reader"
} else:ios {
    # FIXME: build failed on iOS if bundle name contains whitespaces
    #
    # PhaseScriptExecution Project\ Copy __BUILD__/client/debug/ios-arm64-gcc/Bitrix\ Forum\ Reader.build/Debug-iphonesimulator/Bitrix\ Forum\ Reader.build/Script-9C316F444A62BF296E3E1F25.sh
    #    cd /Users/eraxillan/Projects/bitrix-forum-reader
    #    /bin/sh -c \"/Users/eraxillan/Projects/bitrix-forum-reader/__BUILD__/client/debug/ios-arm64-gcc/Bitrix\ Forum\ Reader.build/Debug-iphonesimulator/Bitrix\ Forum\ Reader.build/Script-9C316F444A62BF296E3E1F25.sh\"
    # cp: /Users/eraxillan/Projects/bitrix-forum-reader/Debug-iphonesimulator/Bitrix: No such file or directory
    # cp: Forum: No such file or directory
    # cp: Reader.app: No such file or directory
    TARGET = "bitrix-forum-reader"
} else {
    TARGET = "bitrix-forum-reader"
}

darwin: QMAKE_RPATHDIR += @loader_path/../Frameworks

android {
    DISTFILES += \
        android/AndroidManifest.xml \
        android/res/values/libs.xml \
        android/build.gradle

    ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android
} else:macos {
    QMAKE_INFO_PLIST = Info.plist
} else:ios|tvos {
    QMAKE_INFO_PLIST = Info-ios.plist
}

android {
    DEFINES += QT_EXTRA_FILE_SELECTOR=\\\"android\\\"
} else:ios|tvos {
    DEFINES += QT_EXTRA_FILE_SELECTOR=\\\"ios\\\"
}

#######################################################################################################################

# Build mode (release by default)
buildmode = release
CONFIG(debug, debug|release):buildmode = debug

APP_PLATFORM = $$first( $$list( $$QMAKE_PLATFORM ) )
APP_ARCH = $$first( $$list( $$QT_ARCH ) )
APP_COMPILER = $$first( $$list( $$QMAKE_COMPILER ) )
APP_BUILD_DIR = $$TOPDIR/__BUILD__/client/$${buildmode}/$${APP_PLATFORM}-$${APP_ARCH}-$${APP_COMPILER}

GUMBO_BUILD_DIR = $$TOPDIR/__BUILD__/gumbo/$${buildmode}/$${APP_PLATFORM}-$${APP_ARCH}-$${APP_COMPILER}

# Output directories setup
DESTDIR     = $${APP_BUILD_DIR}
UI_DIR      = $${APP_BUILD_DIR}
OBJECTS_DIR = $${APP_BUILD_DIR}
MOC_DIR     = $${APP_BUILD_DIR}

#######################################################################################################################

QT += qml quick quickcontrols2
QT += multimedia concurrent
# USE_QT_NAM: QT += network
# QT += gui widgets

android: QT += androidextras

CONFIG += c++11
INCLUDEPATH += "."
INCLUDEPATH += $$TOPDIR

# DEFINES += USE_QT_NAM
# DEFINES += QT_GUMBO_METADATA
# DEFINES += BITRIX_FORUM_READER_SYNC_API
# DEFINES += BFR_DRAW_FRAME_ON_COMPONENT_FOR_DEBUG
# DEFINES += BFR_PRINT_DEBUG_OUTPUT
# DEFINES += BFR_DUMP_GENERATED_QML_IN_FILES
DEFINES += BFR_SERIALIZATION_ENABLED

#######################################################################################################################

SOURCES += \
    common/filedownloader.cpp               \
    common/forumthreadurl.cpp               \
    website_backend/gumboparserimpl.cpp     \
    website_backend/qtgumbodocument.cpp     \
    website_backend/qtgumbonode.cpp         \
    website_backend/websiteinterface.cpp    \
    qml_frontend/forumreader.cpp            \
    qml_frontend/main.cpp                   \
    parser_frontend/forumthreadpool.cpp \
    tests/gumboparserimpl_tests.cpp

HEADERS += \
    common/filedownloader.h                 \
    common/forumthreadurl.h                 \
    common/logger.h                         \
    common/resultcode.h                     \
    website_backend/html_tag.h              \
    website_backend/qtgumbodocument.h       \
    website_backend/qtgumbonode.h           \
    website_backend/websiteinterface_fwd.h  \
    website_backend/websiteinterface.h      \
    website_backend/gumboparserimpl.h       \
    qml_frontend/forumreader.h              \
    parser_frontend/forumthreadpool.h

RESOURCES += qml_frontend/qml.qrc

#######################################################################################################################

# Static libraries for different architectures live in different directories
windows {
    contains(QT_ARCH, i386) {
        ARCH = x32
    } else {
        ARCH = x64
    }
} else:macx {
    # NOTE: modern macOS are 64-bit
    ARCH = x64
} else:linux:!android {
    ARCH = x64
} else:android {
    # TODO: only armeabi-v7a and x86 are currently supported
    contains(QT_ARCH, arm) {
        ARCH = armeabi-v7a
    } else:contains(QT_ARCH, i386) {
        ARCH = x86
    } else {
        error("Unsupported Android architecture")
    }
} else:ios {
    ARCH = arm64
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

INCLUDEPATH += $$TOPDIR/gumbo-parser/src
INCLUDEPATH += $$TOPDIR/spdlog/include

windows {
    # Windows (Desktop, X86_64, static libraries)
    DEFINES += CURL_STATICLIB
    INCLUDEPATH += $$TOPDIR/libs/curl/win32/include
    
    QMAKE_LFLAGS_DEBUG += /ignore:4099
    LIBS += -ladvapi32 -luser32 -lgdi32 -lws2_32 -lwsock32 -lWldap32

    # OpenSSL
    LIBS += -L$$PWD/libs/openssl/win32/VC14/$$ARCH/$$buildmode
    # cURL
    LIBS += -L$$PWD/libs/curl/win32/VC14/$$ARCH/$$buildmode
    # Gumbo
    LIBS += -L$$PWD/libs/gumbo-parser/win32/VC14/$$ARCH/$$buildmode

    LIBS += -llibeay32 -lssleay32
    LIBS += -llibcurl$$SUFFIX
    LIBS += -lgumbo-parser
} else:macx {
    # cURL
    INCLUDEPATH += $$TOPDIR/libs/curl/macos/include
    LIBS += -L$$PWD/libs/curl/macos/lib
    LIBS += -lcurl

    # Gumbo
    LIBS += -L$$GUMBO_BUILD_DIR
    LIBS += -lgumbo-parser
} else:linux:!android {
    # Linux (Desktop, X86_64, shared libraries)

    # cURL
    # Just use system library installed ("sudo apt-get install libcurl4-openssl-dev" command for Ubuntu)
    INCLUDEPATH += "/usr/include/x86_64-linux-gnu"
    LIBS += -L"/usr/lib/x86_64-linux-gnu"
    LIBS += -l"curl"

    # Gumbo
    LIBS += -L$$GUMBO_BUILD_DIR
    LIBS += -lgumbo-parser
} else:android {
    # Android (Mobile, X86/ARMv7, static libraries)

    # cURL
    INCLUDEPATH += $$TOPDIR/curl/prebuilt-with-ssl/android/include
    LIBS += -L$$PWD/curl/prebuilt-with-ssl/android/$$ARCH
    LIBS += -lcurl

    # Gumbo
    LIBS += -L$$GUMBO_BUILD_DIR
    LIBS += -lgumbo-parser
} else:ios {
    # iOS (Mobile, Universal binary, static libraries)

    # cURL
    INCLUDEPATH += $$TOPDIR/curl/prebuilt-with-ssl/iOS/include
    LIBS += -L$$PWD/curl/prebuilt-with-ssl/iOS
    LIBS += -lcurl

    # Gumbo
    LIBS += -L$$GUMBO_BUILD_DIR
    LIBS += -lgumbo-parser
} else {
    error("Unsupported OS")
}

#######################################################################################################################

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH += $$OUT_PWD/../fluid/qml
QML_IMPORT_PATH += $$OUT_PWD/qml_frontend/qml
QML_IMPORT_PATH += $$OUT_PWD/qml_frontend/qml/+android
#QML_IMPORT_PATH += $$OUT_PWD/qml_frontend/qml/+ios

# Default rules for deployment
include(deployment.pri)
