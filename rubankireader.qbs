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
        Depends {
            name: "Qt"
            submodules: ["core", "network", "multimedia", "concurrent", "widgets", "qml", "quick", "quickcontrols2"]

            Properties {
                condition: qbs.targetOS.contains("android")
                submodules: outer.concat(["androidextras"])
            }
        }

        // Basic stuff
        cpp.includePaths: ["."]
        
        // Project-bundled libraries
        Depends { name: "gumbo-parser" }

        // Third-party libraries

        // FIXME: define for every system except of Linux
        // cpp.defines: ["CURL_STATICLIB"]

        Properties {
            condition: qbs.targetOS.contains("linux")

            cpp.includePaths: outer.concat(["/usr/include/x86_64-linux-gnu"])
            cpp.libraryPaths: outer.concat(["/usr/lib/x86_64-linux-gnu"])
            cpp.dynamicLibraries: outer.concat(["curl"])
        }

//        cpp.dynamicLibraries: ["tidy"]

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
                "tests/gumboparserimpl_tests.cpp"
            ]
        }

        Group {
            name: "C++ headers"
            files: [
                "common/resultcode.h",
                "common/filedownloader.h",
                "website_backend/html_tag.h",
                "website_backend/qtgumbodocument.h",
                "website_backend/qtgumbonode.h",
                "website_backend/websiteinterface.h",
                "website_backend/gumboparserimpl.h",
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
        cpp.cxxLanguageVersion: "c++14"
        
        // Additional import path used to resolve QML modules in Qt Creator's code model
        // QML_IMPORT_PATH =
        // property stringList qmlImportPaths: [sourceDirectory + "/path/to/qml/"]
    }
    
    references: ["gumbo-parser/gumbo-parser.qbs"]
}
