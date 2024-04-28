add_library(${LIBRARY_NAME} STATIC 
    src/lang.cpp
    src/lexer.cpp
    src/parser.cpp
)

target_include_directories(${LIBRARY_NAME} 
    PUBLIC "include"
)