var FileInfo = require("qbs.FileInfo");

function getRelativePath(from, to) {
    var fromAbsPath = FileInfo.joinPaths("/", from)
    var toAbsPath = FileInfo.joinPaths("/", to)
    return FileInfo.relativePath(fromAbsPath, toAbsPath)
}

