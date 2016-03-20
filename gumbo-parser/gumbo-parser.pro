QT       -= gui

TARGET = gumbo-parser
TEMPLATE = lib
CONFIG  += staticlib
VERSION = 0.10.1

# Build mode (release by default)
buildmode = release
CONFIG(debug, debug|release):buildmode = debug

# Output directories setup
DESTDIR     = $${buildmode}
UI_DIR      = $${buildmode}
OBJECTS_DIR = $${buildmode}
MOC_DIR     = $${buildmode}

HEADERS += \
        src/attribute.h \
        src/char_ref.h \
        src/error.h \
        src/gumbo.h \
        src/insertion_mode.h \
        src/parser.h \
        src/string_buffer.h \
        src/string_piece.h \
        src/tag_enum.h \
        src/tag_gperf.h \
        src/tag_strings.h \
        src/token_type.h \
        src/tokenizer.h \
        src/utf8.h \
        src/util.h \
        src/vector.h

SOURCES += \
        src/attribute.c \
        src/char_ref.c \
        src/error.c \
        src/parser.c \
        src/string_buffer.c \
        src/string_piece.c \
        src/tag.c \
        src/tokenizer.c \
        src/utf8.c \
        src/util.c \
        src/vector.c

win32
{
    INCLUDEPATH += $$PWD/visualc/include
}

unix
{
    target.path = /usr/lib
    INSTALLS += target
}
