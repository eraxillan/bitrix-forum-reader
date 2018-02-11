import qbs
import qbs.File
import qbs.FileInfo
import qbs.Utilities

// FIXME: those two imports must be reduntant, but qbs unable to find modules automatically for some reason
import "qbs/imports/QtQmlApplication.qbs" as QtQmlApplication
import "qbs/imports/QmlModule.qbs" as QmlModule

Project {
    property string minimumQtVersion: "5.8"
    minimumQbsVersion: "1.7.0"
    qbsSearchPaths: ["qbs"]
    references: ["gumbo-parser/gumbo-parser.qbs"]

    Product {
        name: "qmake project files"
        files: ["**/*.pr[io]"]
    }


    Project {
        AndroidApk {
            condition: qbs.targetOS.contains("android")
            
            name: "bitrix-forum-reader"
            packageName: "org.example.bitrix_forum_reader"
            targetName: "bitrix-forum-reader"
            Depends {
                productTypes: ["android.nativelibrary"]
                limitToSubProject: true
            }

            // FIXME: replace @array and @string placeholders with actual values as qmake do
            manifestFile: sourceDirectory + "/android/AndroidManifest.xml"
            resourcesDir: sourceDirectory + "/android/res"
        }

        // NOTE: may be just "QtGuiApplication"?
        QtQmlApplication
        {
            //name: "bitrix-forum-reader"
            //appShortName: "bitrix-forum-reader"

            // Android NDK
            Depends { name: "Android.ndk"; required: false }

            // Qt modules
            Depends { name: "Qt.core"; versionAtLeast: project.minimumQtVersion }
            Depends {
                name: "Qt"
                // FIXME: make "network" and "multimedia" optional
                submodules: ["gui", "qml", "quick", "multimedia", "concurrent", "quickcontrols2"]

                Properties {
                    condition: qbs.targetOS.contains("android")
                    submodules: outer.concat(["androidextras"])
                }
            }

            // Project-bundled libraries
            Depends { name: "gumbo-parser"; profiles: []  }

            // Third-party libraries

            // Windows (Desktop, X86_64, static library)
            // FIXME: add x32 architecture and debug variant of library
            Properties {
                condition: qbs.targetOS.contains("windows")

                cpp.defines: outer.concat(["CURL_STATICLIB"])

                cpp.includePaths: outer.concat(["libs/curl/win32/include"])
                cpp.libraryPaths: outer.concat(["libs/curl/win32/VC14/x64/release"])
                cpp.dynamicLibraries: outer.concat(["curl"])
            }

            // Linux (Desktop, X86_64, shared library)
            // Just use system library installed ("sudo apt-get install libcurl4-openssl-dev" command for Ubuntu)
            Properties {
                condition: qbs.targetOS.contains("linux") && !qbs.targetOS.contains("android")

                cpp.includePaths: outer.concat(["/usr/include/x86_64-linux-gnu"])
                cpp.libraryPaths: outer.concat(["/usr/lib/x86_64-linux-gnu"])
                cpp.dynamicLibraries: outer.concat(["curl"])
            }

            // Android (Mobile, armv7, static library)
            // FIXME: add x86 architecture using qbs.architecture property
            // (returns "armv7a" for ARM, X86 kit don't working now)
            Properties {
                condition: qbs.targetOS.contains("android")

                cpp.includePaths: outer.concat(["curl/prebuilt-with-ssl/android/include"])
                cpp.libraryPaths: outer.concat(["curl/prebuilt-with-ssl/android/armeabi-v7a"])
                cpp.dynamicLibraries: outer.concat(["curl"])
            }

            // iOS (Mobile, Multi-arch, static library)
            Properties {
                condition: qbs.targetOS.contains("ios")

                cpp.includePaths: outer.concat(["curl/prebuilt-with-ssl/iOS/include"])
                cpp.libraryPaths: outer.concat(["curl/prebuilt-with-ssl/iOS"])
                cpp.dynamicLibraries: outer.concat(["curl"])
            }

//        cpp.dynamicLibraries: ["tidy"]

            // Use modern STL on Android
            Properties {
               condition: qbs.targetOS.contains("android")

               cpp.cxxFlags: outer.concat(["-Wformat"])
               // FIXME:
//               architectures: !qbs.architecture ? ["x86", "armv7a"] : undefined
               Android.ndk.appStl: "gnustl_shared"
            }
            
            // FIXME:
//            architectures: undefined
            cpp.minimumIosVersion: "8.0"
            cpp.rpaths: qbs.targetOS.contains("darwin") ? ["@loader_path/../Frameworks"] : undefined
            bundle.isBundle: qbs.targetOS.contains("darwin")
            targetName: bundle.isBundle ? "Bitrix Forum Reader" : "bitrix-forum-reader"
            
            files: [
                "Info.plist"
            ]

            // Basic compiler options
            cpp.cxxLanguageVersion: "c++11"
            cpp.includePaths: ["."]

            // Custom project defines to enable/disable features
            // QT_GUMBO_METADATA
            // FORUM_READER_SYNC_API
            // RBR_DRAW_FRAME_ON_COMPONENT_FOR_DEBUG
            // RBR_PRINT_DEBUG_OUTPUT
            // RBR_DUMP_GENERATED_QML_IN_FILES
            //cpp.defines: concat([])

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

            /* Group {
                name: "Application Binary"
                fileTagsFilter: "application"
                qbs.install: true
                qbs.installDir: "bin"
            } */
            
            Group {
                fileTagsFilter: ["application", "aggregate_infoplist", "pkginfo"]
                qbs.install: true
                qbs.installDir: bundle.isBundle ? "Applications" : "opt/bitrix-forum-reader"
                qbs.installSourceBase: product.buildDirectory
            }

            // Compiler settings
            // cpp.defines: ["TEST_DEF"]
            //cpp.cxxLanguageVersion: "c++14"

            // Additional import path used to resolve QML modules in Qt Creator's code model
            // QML_IMPORT_PATH =
            // property stringList qmlImportPaths: [sourceDirectory + "/path/to/qml/"]
        }
    }
}

