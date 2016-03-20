TEMPLATE = app

QT += qml quick widgets network

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

INCLUDEPATH += "C:/Projects/gumbo-parser/src"
contains(QT_ARCH, i386) {
    message("qmake: 32-bit build will be produced")
    LIBS += "C:/Projects/gumbo-parser/visualc/Debug/gumbo.lib"
} else {
    message("qmake: 64-bit build will be produced")
    LIBS += "C:/Projects/gumbo-parser/visualc/x64/Debug/gumbo.lib"
}

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
include(deployment.pri)
