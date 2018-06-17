import qbs
import qbs.FileInfo

Module {
    name: "app_config"

    property string binDir: ""
    property string dataDir: "data"
    property string qmlInstallDir: FileInfo.joinPaths(dataDir, "qml")
    property string configInstallDir: FileInfo.joinPaths(dataDir, "config")
    property string pluginsInstallDir: FileInfo.joinPaths(dataDir, "plugins")
    property string libsInstallDir: "lib"
    property string includesInstallDir: FileInfo.joinPaths(dataDir, "include")
}

