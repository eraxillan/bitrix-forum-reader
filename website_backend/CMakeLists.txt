# CMakeLists for common directory
#
# provides the common library

SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "lib/")

SET(rubankireader_website_backend_HEADERS
    gumboparserimpl.h
    html_tag.h
    qtgumbodocument.h
    qtgumbonode.h
    websiteinterface.h
)

SET(rubankireader_website_backend_SOURCES
    gumboparserimpl.cpp
    qtgumbonode.cpp
    websiteinterface.cpp
)

INCLUDE_DIRECTORIES(${Qt5Core_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${Qt5Network_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})

add_library(rubankireader_website_backend STATIC
    ${rubankireader_website_backend_SOURCES}
    ${rubankireader_website_backend_HEADERS}
    ${rubankireader_website_backend_MOC_SRCS}
)
target_link_libraries(rubankireader_website_backend gumbo-parser)

