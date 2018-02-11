# CMakeLists for qml_frontend directory
#
# provides the qml_frontend library

SET(qml_frontend_SOURCES
    main.cpp
    forumreader.cpp
)

SET(qml_frontend_HEADERS
    forumreader.h
)

INCLUDE_DIRECTORIES(${Qt5Core_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${Qt5Network_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${Qt5Widgets_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${Qt5Qml_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${Qt5Quick_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${Qt5Multimedia_INCLUDE_DIRS})
if (ANDROID)
    INCLUDE_DIRECTORIES(${Qt5AndroidExtras_INCLUDE_DIRS})
endif()
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})

set(bitrixforumreader_LIBS)

if(ANDROID)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fexceptions")
endif()

# Qt5 stuff
# QtCore
list(APPEND bitrixforumreader_LIBS Core)
# QtNetwork
list(APPEND bitrixforumreader_LIBS Network)
# QtWidgets
list(APPEND bitrixforumreader_LIBS Widgets)
# QtQml
list(APPEND bitrixforumreader_LIBS Qml)
# QtQuick
list(APPEND bitrixforumreader_LIBS Quick)
# QtMultimedia
list(APPEND bitrixforumreader_LIBS Multimedia)
# QtAndroidExtras
if (ANDROID)
    list(APPEND bitrixforumreader_LIBS AndroidExtras)
endif()

SET(qml_frontend_RESOURCES qml.qrc)

# Let cmake chew Qt5's translations and resource files
# Note: header files are MOC-ed automatically by cmake
#IF(UPDATE_TRANSLATIONS)
#    QT5_CREATE_TRANSLATION(cockatrice_QM ${translate_SRCS} ${cockatrice_TS})
#ELSE()
#    QT5_ADD_TRANSLATION(cockatrice_QM ${cockatrice_TS})
#ENDIF()

QT5_ADD_RESOURCES(qml_frontend_RESOURCES_RCC ${qml_frontend_RESOURCES})

# Guess plugins and libraries directory
set(QT_PLUGINS_DIR "${Qt5Widgets_DIR}/../../../plugins")
get_target_property(QT_LIBRARY_DIR Qt5::Core LOCATION)
get_filename_component(QT_LIBRARY_DIR ${QT_LIBRARY_DIR} PATH)

# Build binary and link it
if (ANDROID)
    ADD_LIBRARY(bitrixforumreader_qml_frontend SHARED
        ${qml_frontend_SOURCES}
        ${qml_frontend_HEADERS}
        ${qml_frontend_QM}
        ${qml_frontend_RESOURCES_RCC}
        ${qml_frontend_MOC_SRCS}
    )
else()
    ADD_EXECUTABLE(bitrixforumreader_qml_frontend WIN32 MACOSX_BUNDLE
        ${qml_frontend_SOURCES}
        ${qml_frontend_HEADERS}
        ${qml_frontend_QM}
        ${qml_frontend_RESOURCES_RCC}
        ${qml_frontend_MOC_SRCS}
    )
endif()

if(MSVC)
    TARGET_LINK_LIBRARIES(bitrixforumreader_qml_frontend bitrixforumreader_common gumbo-parser bitrixforumreader_website_backend Qt5::WinMain)
else()
    TARGET_LINK_LIBRARIES(bitrixforumreader_qml_frontend bitrixforumreader_common gumbo-parser bitrixforumreader_website_backend)
endif()
qt5_use_modules(bitrixforumreader_qml_frontend ${bitrixforumreader_LIBS})

if(ANDROID)
    include(../cmake/AddQtAndroidApk.cmake)
    add_qt_android_apk(bitrixforumreader_qml_frontend_apk bitrixforumreader_qml_frontend
        NAME "Bitrix forum viewer"
        VERSION_CODE 1
        PACKAGE_NAME "name.eraxillan.bitrixforumreader"
    )
endif()

