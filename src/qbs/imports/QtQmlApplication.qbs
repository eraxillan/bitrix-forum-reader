
import qbs
import qbs.FileInfo
import "qmlTools.js" as Tools

QtGuiApplication {
    property string appName: name
    property string appShortName: appName

    property bool extraWarnings: false

    targetName: appShortName

    Depends { name: "cpp" }
    Depends { name: "Qt.qml" }
    Depends { name: "Qt.quick" }
    Depends { name: "Qt.quickcontrols2" }

    Depends { name: "app_config" }
    Depends { name: "qml_imports"; required: false }

    property string relativeQmlModulesDir: Tools.getRelativePath(app_config.binDir, app_config.qmlInstallDir)
    property string relativePluginsDir: Tools.getRelativePath(app_config.binDir, app_config.pluginsInstallDir)
    property string relativeLibDir: Tools.getRelativePath(app_config.binDir, app_config.libInstallDir)
    property string relativeConfigDir: Tools.getRelativePath(app_config.binDir, app_config.configInstallDir)

    cpp.cxxLanguageVersion: "c++11"

    Properties {
        condition: qbs.targetOS.contains("linux") && extraWarnings
        cpp.warningLevel: "all"
        cpp.commonCompilerFlags: ['-Wall', '-Wextra', '-pedantic', '-Weffc++', '-Wold-style-cast']
        cpp.systemIncludePaths: [
            FileInfo.joinPaths(Qt.core.incPath),
            FileInfo.joinPaths(Qt.core.incPath, 'QtCore'),
            FileInfo.joinPaths(Qt.core.incPath, 'QtGui'),
            FileInfo.joinPaths(Qt.core.incPath, 'QtQml'),
            FileInfo.joinPaths(Qt.core.incPath, 'QtSvg'),
        ]
    }

    Properties {
        condition: bundle.isBundle

        targetName: appName
        bundle.resources: [ FileInfo.joinPaths(qbs.installRoot, app_config.dataDir) ]

        relativeQmlModulesDir: Tools.getRelativePath(bundle.executableFolderPath,
                                                     FileInfo.joinPaths(bundle.contentsFolderPath, "Resources", app_config.qmlInstallDir))
        relativePluginsDir: Tools.getRelativePath(bundle.executableFolderPath,
                                                  FileInfo.joinPaths(bundle.contentsFolderPath, "Frameworks"))
        relativeLibDir: Tools.getRelativePath(bundle.executableFolderPath,
                                              FileInfo.joinPaths(bundle.contentsFolderPath, "Resources"))
        relativeConfigDir: Tools.getRelativePath(bundle.executableFolderPath,
                                                 FileInfo.joinPaths(bundle.contentsFolderPath, "Resources", app_config.configInstallDir))
    }

    cpp.defines: [
        'APP_QML_MODULES_PATH="' + relativeQmlModulesDir + '"',
        'APP_PLUGINS_PATH="' + relativePluginsDir + '"',
        'APP_CONFIG_PATH="' + relativeConfigDir + '"'
    ]
    cpp.rpaths: qbs.targetOS.contains("macos")
                ? ["@executable_path/" + relativeLibDir]
                : ["$ORIGIN/", "$ORIGIN/" + relativeLibDir]

    Group {
        fileTagsFilter: ["application"]
        condition: !bundle.isBundle
        qbs.install: true
        qbs.installDir: app_config.binDir
    }
}

