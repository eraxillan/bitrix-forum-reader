# CMakeLists for common directory
#
# provides the common library

SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "lib/")

SET(rubankireader_common_SOURCES
    filedownloader.cpp
)

SET(rubankireader_common_HEADERS
    filedownloader.h
)

INCLUDE_DIRECTORIES(${Qt5Core_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${Qt5Network_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})

ADD_LIBRARY(rubankireader_common STATIC
    ${rubankireader_common_SOURCES}
    ${rubankireader_common_HEADERS}
    ${rubankireader_common_MOC_SRCS}
)
