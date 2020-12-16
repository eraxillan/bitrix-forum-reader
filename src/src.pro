!versionAtLeast(QT_VERSION, "5.15.0") {
    message("Cannot build this program with Qt version $${QT_VERSION}")
    error("Use at least Qt 5.15.0")
}

#######################################################################################################################

TEMPLATE = app
CONFIG -= debug_and_release debug_and_release_target

TOPDIR = $$clean_path($$PWD/..)
FLUID_OUT_DIR = $$clean_path($$OUT_PWD/../thirdparty/fluid/qml)
GUMBO_PARSER_OUT_DIR = $$clean_path($$OUT_PWD/../gumbo-parser)

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
    # NOTE: free icon got from https://www.materialui.co/materialIcons/communication/forum_black_64x64.png
    DISTFILES += \
        android/AndroidManifest.xml \
        android/res/drawable/icon.png \
        android/res/values/strings.xml

    ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android

    # Bundle Fluid QML plugins with the application
    ANDROID_EXTRA_PLUGINS = $$FLUID_OUT_DIR
} else:macos {
    QMAKE_INFO_PLIST = Info.plist
} else:ios|tvos {
    QMAKE_INFO_PLIST = Info-ios.plist
}

# Use platform-specific QML source files
android {
    DEFINES += QT_EXTRA_FILE_SELECTOR=\\\"android\\\"
} else:ios|tvos {
    DEFINES += QT_EXTRA_FILE_SELECTOR=\\\"ios\\\"
}

#######################################################################################################################

# General compiler options
CONFIG += c++17
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
    website_backend/gumboparserimpl.cpp     \
    website_backend/qtgumbodocument.cpp     \
    website_backend/qtgumbonode.cpp         \
    website_backend/websiteinterface.cpp    \
    website_backend/websiteinterface_qt.cpp \
    qml_frontend/forumreader.cpp            \
    qml_frontend/task.cpp                   \
    qml_frontend/main.cpp                   \
    parser_frontend/forumthreadpool.cpp     \
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
    website_backend/websiteinterface_qt.h   \
    website_backend/gumboparserimpl.h       \
    parser_frontend/forumthreadpool.h       \
    qml_frontend/forumreader.h              \
    qml_frontend/task.h

QML_FILES += \
    $$TOPDIR/src/qml_frontend/qml/+android/main.qml \
    $$TOPDIR/src/qml_frontend/qml/+android/NavigationPanel.qml \
    $$TOPDIR/src/qml_frontend/qml/+android/PostList.qml \
    $$TOPDIR/src/qml_frontend/qml/+ios/main.qml \
    $$TOPDIR/src/qml_frontend/qml/main.qml \
    $$TOPDIR/src/qml_frontend/qml/PostAnimatedImage.qml \
    $$TOPDIR/src/qml_frontend/qml/PostHyperlink.qml \
    $$TOPDIR/src/qml_frontend/qml/PostImage.qml \
    $$TOPDIR/src/qml_frontend/qml/PostLineBreak.qml \
    $$TOPDIR/src/qml_frontend/qml/PostList.qml \
    $$TOPDIR/src/qml_frontend/qml/PostPlainText.qml \
    $$TOPDIR/src/qml_frontend/qml/Post.qml \
    $$TOPDIR/src/qml_frontend/qml/PostQuote.qml \
    $$TOPDIR/src/qml_frontend/qml/PostRichText.qml \
    $$TOPDIR/src/qml_frontend/qml/PostSpoiler.qml \
    $$TOPDIR/src/qml_frontend/qml/PostVideo.qml \
    $$TOPDIR/src/qml_frontend/qml/UserListDialog.qml \
    $$TOPDIR/src/qml_frontend/qml/User.qml

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

# ... and have different suffixes for debug and release modes
CONFIG(debug, debug|release) {
    SUFFIX = d
}

# Link with:
#   1) OpenSSL library
#   2) cURL library (compiled with OpenSSL support)
#   3) Gumbo HTML5 parser library

INCLUDEPATH += $$TOPDIR/gumbo-parser/src
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
    LIBS += -L$$GUMBO_PARSER_OUT_DIR
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
    LIBS += -L$$GUMBO_PARSER_OUT_DIR
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
    LIBS += -L$$GUMBO_PARSER_OUT_DIR
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
    LIBS += -L$$GUMBO_PARSER_OUT_DIR
    LIBS += -lgumbo-parser
} else {
    error("Unsupported OS")
}

#######################################################################################################################

# Copy all files to the build directory so that QtCreator will recognize
# the QML module and the demo will run without installation
#isEmpty(DESTDIR): DESTDIR = $$OUT_PWD
#qmlfiles2build.files = $$QML_FILES
#qmlfiles2build.path = $$DESTDIR
#COPIES += qmlfiles2build

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH += $$FLUID_OUT_DIR
#QML_IMPORT_PATH += $$DESTDIR

#QML_IMPORT_PATH += $$TOPDIR/thirdparty/fluid/qml
#QML_IMPORT_PATH += $$OUT_PWD/qml_frontend/qml
#QML_IMPORT_PATH += $$OUT_PWD/qml_frontend/qml/+android
#QML_IMPORT_PATH += $$OUT_PWD/qml_frontend/qml/+ios

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# Default rules for deployment
include(deployment.pri)

ANDROID_ABIS = armeabi-v7a arm64-v8a
