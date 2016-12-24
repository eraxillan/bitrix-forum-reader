# CMakeLists for gumbo-parser directory
#
# provides the gumbo-parser library
cmake_minimum_required(VERSION 3.1)

# Allow VERSION option in PROJECT statement
if(POLICY CMP0048)
    cmake_policy(SET CMP0048 NEW)
endif()

PROJECT(gumbo-parser VERSION 0.10.1)

SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "lib/")

SET(gumbo_HEADERS
    src/attribute.h
    src/char_ref.h
    src/error.h
    src/gumbo.h
    src/insertion_mode.h
    src/parser.h
    src/string_buffer.h
    src/string_piece.h
    src/tag_enum.h
    src/tag_gperf.h
    src/tag_strings.h
    src/token_type.h
    src/tokenizer.h
    src/utf8.h
    src/util.h
    src/vector.h
)

SET(gumbo_SOURCES
    src/attribute.c
    src/char_ref.c
    src/error.c
    src/parser.c
    src/string_buffer.c
    src/string_piece.c
    src/tag.c
    src/tokenizer.c
    src/utf8.c
    src/util.c
    src/vector.c
)

INCLUDE_DIRECTORIES(src)
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})
if(WIN32)
    INCLUDE_DIRECTORIES(visualc/include)
endif()

add_library(gumbo-parser STATIC ${gumbo_HEADERS} ${gumbo_SOURCES} ${gumbo_MOC_SRCS})
