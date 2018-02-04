import qbs
import qbs.FileInfo

// FIXME: those two imports must be reduntant, but qbs unable to find modules automatically for some reason
import "qbs/imports/QtQmlApplication.qbs" as QtQmlApplication
import "qbs/imports/QmlModule.qbs" as QmlModule

Project {
    name: "rubankireader"
    qbsSearchPaths: "qbs"

    QtQmlApplication
    {
        name: "rubankireader"
        //appShortName: "rubankireader"
        
        // Qt modules
        // QT += qml quick widgets network multimedia
        // android: QT += androidextras
        Depends {
            name: "Qt"
            submodules: ["core", "network", "multimedia", "widgets", "qml", "quick"]

            Properties {
                condition: qbs.targetOS.contains("android")
                submodules: outer.concat(["androidextras"])
            }
        }
        
        // Third-party libraries
        // gumbo-parser
        // INCLUDEPATH += $$PWD/gumbo-parser/src
        // LIBS        += -L$$PWD/gumbo-parser/$${buildmode} -lgumbo-parser
        cpp.includePaths: [".", "gumbo-parser/src"]
//        cpp.dynamicLibraries: ["tidy"]
        Depends { name: "gumbo-parser" }

        Group {
            name: "C++ source code"
            files: [
                "common/filedownloader.cpp",
                "website_backend/gumboparserimpl.cpp",
                "website_backend/qtgumbodocument.cpp",
                "website_backend/qtgumbonode.cpp",
                "website_backend/websiteinterface.cpp",
                "qml_frontend/forumreader.cpp",
                "qml_frontend/main.cpp",
            ]
        }

        Group {
            name: "C++ headers"
            files: [
                "common/filedownloader.h",
                "website_backend/gumboparserimpl.h",
                "website_backend/qtgumbonode.h",
                "website_backend/websiteinterface.h",
                "website_backend/html_tag.h",
                "website_backend/qtgumbodocument.h",
                "qml_frontend/forumreader.h"
            ]
        }

        Group {
            name: "Qt Runtime Resources"
            files: "qml_frontend/qml.qrc"
        }

        Group {
            name: "Application Binary"
            fileTagsFilter: "application"
            qbs.install: true
            qbs.installDir: "bin"
        }
        
        // Compiler settings
        // cpp.defines: ["TEST_DEF"]
        // CONFIG += c++11
        cpp.cxxLanguageVersion: "c++14"
        
        // Additional import path used to resolve QML modules in Qt Creator's code model
        // QML_IMPORT_PATH =
        // property stringList qmlImportPaths: [sourceDirectory + "/path/to/qml/"]
    }
    
    references: ["gumbo-parser/gumbo-parser.qbs"]
}
