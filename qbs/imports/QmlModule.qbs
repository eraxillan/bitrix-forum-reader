import qbs
import qbs.FileInfo


Product {
    name: "qml_module"
    type: "qml_import"

    Depends { name: "app_config" }

    property string moduleSourcesDir: "modules"
    property path moduleBaseDir: FileInfo.joinPaths(sourceDirectory, moduleSourcesDir)
    property string moduleInstallDir: app_config.qmlInstallDir

    property pathList qmlImportPaths: [ moduleBaseDir ]

    Group {
        name: "qml_install"
        fileTagsFilter: ["qml_import"]

        qbs.install: true
        qbs.installSourceBase: moduleBaseDir
        qbs.installPrefix: moduleInstallDir
    }
}

