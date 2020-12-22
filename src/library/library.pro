!versionAtLeast(QT_VERSION, "5.15.0") {
    message("Cannot build this program with Qt version $${QT_VERSION}")
    error("Use at least Qt 5.15.0")
}

#######################################################################################################################

TARGET = bfr_library
TEMPLATE = lib
CONFIG += dll
VERSION = 0.0.1

CONFIG -= debug_and_release debug_and_release_target

TOPDIR = $$clean_path($$PWD/../..)
FLUID_OUT_DIR = $$clean_path($$OUT_PWD/../../thirdparty/fluid/qml)
GUMBO_LIBRARY_DIR = $$clean_path($$OUT_PWD/../../thirdparty/gumbo-parser)

#######################################################################################################################
# Platform-specific setup

# TODO
#######################################################################################################################

# General compiler options
CONFIG += c++17 strict_c++ warn_on
INCLUDEPATH += "."
INCLUDEPATH += $$TOPDIR

QT += qml quick quickcontrols2
QT += multimedia concurrent

# FIXME: check whether those deps are required
android: QT += androidextras svg gui widgets

# qmake CONFIG option 'bfr_use_curl' force program to use libcurl
# instead of QNetworkManager (default choice)
!bfr_use_curl {
    QT += network
    DEFINES += USE_QT_NAM
}

# Enable all debug features in the corresponing build
CONFIG(debug, debug|release) {
    DEFINES += QT_GUMBO_METADATA
    DEFINES += BFR_DRAW_FRAME_ON_COMPONENT_FOR_DEBUG
    DEFINES += BFR_PRINT_DEBUG_OUTPUT
    DEFINES += BFR_DUMP_GENERATED_QML_IN_FILES
    DEFINES += "BFR_QML_OUTPUT_DIR=\"\\\"__temp_qml\\\"\""
    DEFINES += BFR_SERIALIZATION_ENABLED
}

# All forum UI items are enabled by default,
# but may be disabled for debugging purposes
# using qmake CONFIG options
!bfr_no_spoiler_item:   DEFINES += BFR_SHOW_SPOILER
!bfr_no_quote_item:     DEFINES += BFR_SHOW_QUOTE
!bfr_no_image_item:     DEFINES += BFR_SHOW_IMAGE
!bfr_no_linebreak_item: DEFINES += BFR_SHOW_LINEBREAK
!bfr_no_plaintext_item: DEFINES += BFR_SHOW_PLAINTEXT
!bfr_no_richtext_item:  DEFINES += BFR_SHOW_RICHTEXT
!bfr_no_video_item:     DEFINES += BFR_SHOW_VIDEO
!bfr_no_hyperlink_item: DEFINES += BFR_SHOW_HYPERLINK

#######################################################################################################################

SOURCES += \
    common/filedownloader.cpp               \
    common/forumthreadurl.cpp               \
    parser_frontend/forumthreadpool.cpp     \
    website_backend/gumboparserimpl.cpp     \
    website_backend/qtgumbodocument.cpp     \
    website_backend/qtgumbonode.cpp         \
    website_backend/websiteinterface.cpp    \
    website_backend/websiteinterface_qt.cpp

HEADERS += \
    common/filedownloader.h                 \
    common/forumthreadurl.h                 \
    common/logger.h                         \
    common/resultcode.h                     \
    parser_frontend/forumthreadpool.h       \
    website_backend/gumboparserimpl.h       \
    website_backend/html_tag.h              \
    website_backend/qtgumbodocument.h       \
    website_backend/qtgumbonode.h           \
    website_backend/websiteinterface.h      \
    website_backend/websiteinterface_fwd.h  \
    website_backend/websiteinterface_qt.h

#######################################################################################################################

# Static libraries for different architectures live in different directories
windows {
    contains(QT_ARCH, i386) {
        ARCH = x32
    } else {
        ARCH = x64
    }

    # On Windows libraries have different suffixes for debug and release modes
    CONFIG(debug, debug|release): SUFFIX = d
} else:macx {
    # NOTE: modern macOS are 64-bit
    ARCH = x64
} else:linux:!android {
    ARCH = x64
} else:android {
    equals(QT_ARCH, "arm64-v8a") {
        ARCH = arm64-v8a
    } else:equals(QT_ARCH, "armeabi-v7a") {
        ARCH = armeabi-v7a
    } else:contains(QT_ARCH, "i386") {
        ARCH = x86
    } else {
        error("Unsupported Android architecture $$QT_ARCH!")
    }
} else:ios {
    ARCH = arm64
} else {
    error("Unsupported OS")
}

# Link with:
#   1) OpenSSL library
#   2) cURL library (compiled with OpenSSL support)
#   3) Gumbo HTML5 parser library

INCLUDEPATH += $$TOPDIR/thirdparty/gumbo-parser/src
INCLUDEPATH += $$TOPDIR/thirdparty/spdlog/include
INCLUDEPATH += $$TOPDIR/thirdparty

windows {
    # Windows (Desktop, X86_64, static libraries)
    # FIXME: check whether Windows version can be build without external OpenSSL
    
    QMAKE_LFLAGS_DEBUG += /ignore:4099
    LIBS += -ladvapi32 -luser32 -lgdi32 -lws2_32 -lwsock32 -lWldap32

    # libcurl and openssl
    bfr_use_curl {
        LIBS += -L$$TOPDIR/libs/openssl/win32/VC14/$$ARCH/$$buildmode
        LIBS += -llibeay32 -lssleay32

        DEFINES += CURL_STATICLIB
        INCLUDEPATH += $$TOPDIR/libs/curl/win32/include
        LIBS += -L$$TOPDIR/libs/curl/win32/VC14/$$ARCH/$$buildmode
        LIBS += -llibcurl$$SUFFIX
    }

    # Gumbo
    LIBS += -L$$TOPDIR/libs/gumbo-parser/win32/VC14/$$ARCH/$$buildmode
    LIBS += -lgumbo-parser
} else:macx {
    # libcurl
    bfr_use_curl {
        INCLUDEPATH += $$TOPDIR/libs/curl/macos/include
        LIBS += -L$$PWD/libs/curl/macos/lib
        LIBS += -lcurl
    }

    # Gumbo
    LIBS += -L$$GUMBO_LIBRARY_DIR
    LIBS += -lgumbo-parser
} else:linux:!android {
    # Linux (Desktop, X86_64, shared libraries)

    # cURL
    # Just use system library installed:
    # Ubuntu: sudo apt-get install libcurl4-openssl-dev
    # Fedora: sudo dnf install libcurl-devel
    bfr_use_curl {
        INCLUDEPATH += "/usr/include/x86_64-linux-gnu"
        LIBS += -L"/usr/lib/x86_64-linux-gnu"
        LIBS += -l"curl"
    }

    # Gumbo
    LIBS += -L$$GUMBO_LIBRARY_DIR
    LIBS += -lgumbo-parser
} else:android {
    # Android (Mobile, X86/ARMv7/ARM64, static and shared libraries)

    # OpenSSL 1.1: shared
    # NOTE: recommended way below:
    # include($$TOPDIR/thirdparty/android_openssl/openssl.pri)
    # will not work for custom Qt built with shared OpenSSL libraries as we are using;
    # so just manually include libraries for latest Qt and Android
    CONFIG(release, debug|release): SSL_PATH = $$TOPDIR/thirdparty/android_openssl
                              else: SSL_PATH = $$TOPDIR/thirdparty/android_openssl/no-asm
    ANDROID_EXTRA_LIBS += \
        $$SSL_PATH/latest/arm/libcrypto_1_1.so \
        $$SSL_PATH/latest/arm/libssl_1_1.so \
        $$SSL_PATH/latest/arm64/libcrypto_1_1.so \
        $$SSL_PATH/latest/arm64/libssl_1_1.so \
        $$SSL_PATH/latest/x86/libcrypto_1_1.so \
        $$SSL_PATH/latest/x86/libssl_1_1.so \
        $$SSL_PATH/latest/x86_64/libcrypto_1_1.so \
        $$SSL_PATH/latest/x86_64/libssl_1_1.so

    # libcurl: static
    bfr_use_curl {
        INCLUDEPATH += $$TOPDIR/thirdparty/curl/prebuilt-with-ssl/android/include
        LIBS += -L$$TOPDIR/thirdparty/curl/prebuilt-with-ssl/android/$$ARCH
        LIBS += -lcurl
    }

    # Gumbo: shared
    LIBS += -L$$GUMBO_LIBRARY_DIR
    LIBS += -lgumbo-parser_$$QT_ARCH
} else:ios {
    # iOS (Mobile, Universal binary, static libraries)

    # libcurl: static
    bfr_use_curl {
        INCLUDEPATH += $$TOPDIR/thirdparty/curl/prebuilt-with-ssl/iOS/include
        LIBS += -L$$TOPDIR/thirdparty/curl/prebuilt-with-ssl/iOS
        LIBS += -lcurl
    }

    # Gumbo
    LIBS += -L$$GUMBO_LIBRARY_DIR
    LIBS += -lgumbo-parser
} else {
    error("Unsupported OS")
}

#######################################################################################################################

ANDROID_ABIS = armeabi-v7a arm64-v8a
