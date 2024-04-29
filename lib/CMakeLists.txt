find_package(LLVM REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")

# Invoke llvm-config to get compiler flags, linker flags, system libraries, and core LLVM libraries
execute_process(
    COMMAND llvm-config-14 --cxxflags --ldflags --system-libs --libs core
    OUTPUT_VARIABLE LLVM_CONFIG_OUTPUT
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

# Split the output by lines
string(REPLACE " " ";" LLVM_CONFIG_NO_SPACE "${LLVM_CONFIG_OUTPUT}")
string(REPLACE "\n" ";" LLVM_CONFIG_FLAGS "${LLVM_CONFIG_NO_SPACE}")

add_library(${LIBRARY_NAME} STATIC

    src/lang.cpp
    src/lexer.cpp
    src/parser.cpp
    src/generator.cpp
)

get_target_property(LIBRARY_NAME_SOURCES ${LIBRARY_NAME} SOURCES)
set(SOURCES_REQUIRE_LLVM "src/generator.cpp" "src/lang.cpp" "src/parser.cpp")

foreach(source ${LIBRARY_NAME_SOURCES})
    list(FIND SOURCES_REQUIRE_LLVM ${source} generator_index)

    if(generator_index GREATER -1)

        foreach(libs ${LLVM_CONFIG_FLAGS})

            if(NOT "${libs}" STREQUAL "-std=c++14")
                target_compile_options(${LIBRARY_NAME} PRIVATE ${libs} -DLLVM_DISABLE_ABI_BREAKING_CHECKS_ENFORCING=1)
                # message("${libs}")
            endif()

        endforeach()

        message("SOURCES REQUIRE LLVM ${source} DURING COMPILATION")

    else()

        target_compile_options(${LIBRARY_NAME} PRIVATE "-std=c++17")

        message("SOURCES DO NOT REQUIRE LLVM ${source} DURING COMPILATION")

    endif()

endforeach()


target_include_directories(${LIBRARY_NAME} 
    PUBLIC "include"
)
