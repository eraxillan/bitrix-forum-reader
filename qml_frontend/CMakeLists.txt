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
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})

set(RUBANKIREADER_LIBS)

# Qt5 stuff
# QtCore
list(APPEND RUBANKIREADER_LIBS Core)
# QtNetwork
list(APPEND RUBANKIREADER_LIBS Network)
# QtWidgets
list(APPEND RUBANKIREADER_LIBS Widgets)
# QtQml
list(APPEND RUBANKIREADER_LIBS Qml)
# QtQuick
list(APPEND RUBANKIREADER_LIBS Quick)
# QtMultimedia
list(APPEND RUBANKIREADER_LIBS Multimedia)

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
ADD_EXECUTABLE(rubankireader_qml_frontend WIN32 MACOSX_BUNDLE
    ${qml_frontend_SOURCES}
    ${qml_frontend_HEADERS}
    ${qml_frontend_QM}
    ${qml_frontend_RESOURCES_RCC}
    ${qml_frontend_MOC_SRCS}
)

if(MSVC)
    TARGET_LINK_LIBRARIES(rubankireader_qml_frontend rubankireader_common gumbo-parser rubankireader_website_backend Qt5::WinMain)
else()
    TARGET_LINK_LIBRARIES(rubankireader_qml_frontend rubankireader_common gumbo-parser rubankireader_website_backend)
endif()
qt5_use_modules(rubankireader_qml_frontend ${RUBANKIREADER_LIBS})
