import qbs
import qbs.FileInfo

StaticLibrary {
    name: "gumbo-parser"
    version: "0.10.1"

    // QT -= gui    
    Depends { name: "cpp" }
    
    Group {
        name: "C++ source code"
        files: [
            "src/attribute.c",
            "src/char_ref.c",
            "src/error.c",
            "src/parser.c",
            "src/string_buffer.c",
            "src/string_piece.c",
            "src/tag.c",
            "src/tokenizer.c",
            "src/utf8.c",
            "src/util.c",
            "src/vector.c"
        ]
    }
    
    Group {
        name: "C++ headers"
        files: [
            "src/attribute.h",
            "src/char_ref.h",
            "src/error.h",
            "src/gumbo.h",
            "src/insertion_mode.h",
            "src/parser.h",
            "src/string_buffer.h",
            "src/string_piece.h",
            "src/tag_enum.h",
            "src/tag_gperf.h",
            "src/tag_strings.h",
            "src/token_type.h",
            "src/tokenizer.h",
            "src/utf8.h",
            "src/util.h",
            "src/vector.h"
        ]
    }

    Properties {
        condition: qbs.targetOS.contains("win32")
        cpp.includePaths: outer.concat(["visualc/include"])
    }

    Properties {
        condition: qbs.targetOS.contains("android")

        cpp.cxxFlags: outer.concat(["-Wformat"])
        cpp.cLanguageVersion: "c11"

        //Depends { name: "Android.sdk" }
        Depends { name: "Android.ndk" }
        Android.ndk.appStl: "gnustl_shared"
    }
    
    Export {
        Depends { name: "cpp" }
        cpp.includePaths: [product.sourceDirectory]
    }
}
