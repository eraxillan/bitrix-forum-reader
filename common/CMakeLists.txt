# CMakeLists for common directory
#
# provides the common library

SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "lib/")

SET(bitrixforumreader_common_SOURCES
    filedownloader.cpp
)

SET(bitrixforumreader_common_HEADERS
    filedownloader.h
    logger.h
)

include_directories("spdlog/include")
INCLUDE_DIRECTORIES(${Qt5Core_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${Qt5Network_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})

ADD_LIBRARY(bitrixforumreader_common STATIC
    ${bitrixforumreader_common_SOURCES}
    ${bitrixforumreader_common_HEADERS}
    ${bitrixforumreader_common_MOC_SRCS}
)

