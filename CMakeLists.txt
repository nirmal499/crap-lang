cmake_minimum_required(VERSION 3.22.1)
project(lang)

set(CMAKE_BUILD_TYPE Debug)

set(EXECUTABLE_NAME "executable")

find_package(LLVM REQUIRED CONFIG)
message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

include_directories(${LLVM_INCLUDE_DIRS})
separate_arguments(LLVM_DEFINITIONS_LIST NATIVE_COMMAND ${LLVM_DEFINITIONS})
add_definitions(${LLVM_DEFINITIONS_LIST})

###### Find the libraries that correspond to the LLVM components that we wish to use
llvm_map_components_to_libnames(llvm_libs support core irreader)

add_executable(${EXECUTABLE_NAME}
    
    lang/main.cpp

    src/lang.cpp
    src/lexer.cpp
    src/parser.cpp
    src/generator.cpp
)

target_include_directories(${EXECUTABLE_NAME}
    PRIVATE "include"
)

target_link_libraries(${EXECUTABLE_NAME} 
    PRIVATE ${llvm_libs}
)

if(0)
    ##### Invoke llvm-config to get compiler flags, linker flags, system libraries, and core LLVM libraries
    execute_process(
        COMMAND llvm-config-14 --cxxflags --ldflags --system-libs --libs core
        OUTPUT_VARIABLE LLVM_CONFIG_OUTPUT
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )

    ##### Split the output by lines
    string(REPLACE " " ";" LLVM_CONFIG_NO_SPACE "${LLVM_CONFIG_OUTPUT}")
    string(REPLACE "\n" ";" LLVM_CONFIG_FLAGS "${LLVM_CONFIG_NO_SPACE}")

    get_target_property(LIBRARY_NAME_SOURCES ${EXECUTABLE_NAME} SOURCES)
    set(SOURCES_REQUIRE_LLVM "lang/main.cpp" "src/lang.cpp" "src/lexer.cpp" "src/parser.cpp" "src/generator.cpp")

    foreach(source ${LIBRARY_NAME_SOURCES})
        list(FIND SOURCES_REQUIRE_LLVM ${source} generator_index)

        if(generator_index GREATER -1)

            foreach(libs ${LLVM_CONFIG_FLAGS})

                if(NOT "${libs}" STREQUAL "-std=c++14")
                    target_compile_options(${EXECUTABLE_NAME} PRIVATE ${libs} -DLLVM_DISABLE_ABI_BREAKING_CHECKS_ENFORCING=1)
                    # message("${libs}")
                endif()

            endforeach()

            message("SOURCES REQUIRE LLVM ${source} DURING COMPILATION")

        else()

            target_compile_options(${EXECUTABLE_NAME} PRIVATE "-std=c++17")

            message("SOURCES DO NOT REQUIRE LLVM ${source} DURING COMPILATION")

        endif()

    endforeach()
endif()