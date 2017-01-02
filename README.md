# bankirureader

[![N|Solid](https://cldup.com/dTxpPi9lDf.thumb.png)](https://nodesource.com/products/nsolid)

Lightweight `banki.ru` forum reader written in QML/C++.

## Compilation for different platforms

### Prerequisites
  - Qt 5.6+
  - Android SDK
  - Android NDK r13b+
  - Apache Ant

CMake build system handle most of crossplatform pain for us.
However, several common and platform-specific variables must be set to get job done.

### Common variables
  - BUILD_TESTING
  - CMAKE_BUILD_TYPE = `Release` or `Debug`
  - TEST
  - UPDATE_TRANSLATIONS

### Crosscompilation on Windows host for Android
  - ANDROID_ABI = armeabi-v7a
  - ANDROID_COMPILER_VERSION = 4.9
  - ANDROID_TOOLCHAIN_MACHINE_NAME = arm-linux-androideabi
  - ANDROID_TOOLCHAIN_NAME = arm-linux-androideabi-4.9
  - CMAKE_TOOLCHAIN_FILE = <Android NDK root>/build/cmake/android.toolchain.cmake
  - QT_ANDROID_ANT = <Ant root>/bin/ant.bat
  - QT_ANDROID_SDK_ROOT = <SDK root>
  - Qt5AndroidExtras_DIR = <Qt root>/android_armv7/lib/cmake/Qt5AndroidExtras
  - Qt5Concurrent_DIR = <Qt root>/android_armv7/lib/cmake/Qt5Concurrent
  - Qt5Core_DIR = <Qt root>/android_armv7/lib/cmake/Qt5Core
  - Qt5Gui_DIR = <Qt root>/android_armv7/lib/cmake/Qt5Gui
  - Qt5LinguistTools_DIR = <Qt root>/android_armv7/lib/cmake/Qt5LinguistTools
  - Qt5Multimedia_DIR = <Qt root>/android_armv7/lib/cmake/Qt5Multimedia
  - Qt5Network_DIR = <Qt root>/android_armv7/lib/cmake/Qt5Network
  - QtPositioning_DIR = <Qt root>/android_armv7/lib/cmake/Qt5Positioning
  - Qt5Qml_DIR = <Qt root>/android_armv7/lib/cmake/Qt5Qml
  - Qt5Quick_DIR = <Qt root>/android_armv7/lib/cmake/Qt5Quick
  - Qt5Svg_DIR = <Qt root>/android_armv7/lib/cmake/Qt5Svg
  - Qt5Test_DIR = <Qt root>/android_armv7/lib/cmake/Qt5Test
  - Qt5Widgets_DIR = <Qt root>/android_armv7/lib/cmake/Qt5Widgets
  - Qt5_DIR = <Qt root>/android_armv7/lib/cmake/Qt5
