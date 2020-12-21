!versionAtLeast(QT_VERSION, "5.15.0") {
    message("Cannot build this program with Qt version $${QT_VERSION}")
    error("Use at least Qt 5.15.0")
}

#######################################################################################################################

TARGET = bfr_tests
TEMPLATE = app
CONFIG -= debug_and_release debug_and_release_target
CONFIG += cmdline

#######################################################################################################################

TOPDIR = $$clean_path($$PWD/../..)
# Our library
BFR_INCLUDE_DIR = $$TOPDIR/src/library
BFR_LIBRARY_DIR = $$clean_path($$OUT_PWD/../../src/library)
# Thirdparty libraries
GUMBO_LIBRARY_DIR = $$clean_path($$OUT_PWD/../../thirdparty/gumbo-parser)

#######################################################################################################################

# General compiler options
CONFIG += c++17 strict_c++ warn_on
INCLUDEPATH += $$PWD
INCLUDEPATH += $$TOPDIR
INCLUDEPATH += $$BFR_INCLUDE_DIR

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

#######################################################################################################################

SOURCES += \
    gumboparserimpl_tests.cpp \
    main.cpp

#######################################################################################################################

# Link with:
#   1) OpenSSL library
#   2) cURL library (compiled with OpenSSL support)
#   3) Gumbo HTML5 parser library

INCLUDEPATH += $$TOPDIR/thirdparty
INCLUDEPATH += $$TOPDIR/thirdparty/gumbo-parser/src
INCLUDEPATH += $$TOPDIR/thirdparty/spdlog/include
INCLUDEPATH += $$TOPDIR/thirdparty/catch2

windows {
    # Windows (Desktop, X86_64, static libraries)

    # Gumbo
    LIBS += -L$$TOPDIR/libs/gumbo-parser/win32/VC14/$$ARCH/$$buildmode
    LIBS += -lgumbo-parser
} else:macx {
    # macOS (Desktop, X86_64, shared libraries)
    QMAKE_RPATHDIR += @loader_path/../Frameworks

    # Gumbo
    LIBS += -L$$GUMBO_LIBRARY_DIR
    LIBS += -lgumbo-parser
} else:linux:!android {
    # Linux (Desktop, X86_64, shared libraries)

    # BFR library
    LIBS += -L$$BFR_LIBRARY_DIR
    LIBS += -lbfr_library

    # Gumbo
    LIBS += -L$$GUMBO_LIBRARY_DIR
    LIBS += -lgumbo-parser
} else:android {
    # Android (Mobile, X86/ARMv7/ARM64, static and shared libraries)

    # BFR library
    LIBS += -L$$BFR_LIBRARY_DIR
    LIBS += -lbfr_library_$$QT_ARCH

    # Gumbo: shared
    LIBS += -L$$GUMBO_LIBRARY_DIR
    LIBS += -lgumbo-parser_$$QT_ARCH
} else:ios {
    # iOS (Mobile, Universal binary, static libraries)
    QMAKE_RPATHDIR += @loader_path/../Frameworks

    # BFR library
    LIBS += -L$$BFR_LIBRARY_DIR
    LIBS += -lbfr_library_$$QT_ARCH

    # Gumbo
    LIBS += -L$$GUMBO_LIBRARY_DIR
    LIBS += -lgumbo-parser
} else {
    error("Unsupported OS")
}

#######################################################################################################################

# Default rules for deployment
include($$TOPDIR/src/deployment.pri)

ANDROID_ABIS = armeabi-v7a arm64-v8a
