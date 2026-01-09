cmake_minimum_required(VERSION 3.21)

project(
    "lddw"
    VERSION 0.0.1
    DESCRIPTION "lddw"
    HOMEPAGE_URL "https://github.com/soltys/lddw"
    LANGUAGES C
)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(COMPILE_WARNING_AS_ERROR ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(PROJECT_IS_TOP_LEVEL)
    if(CMAKE_C_COMPILER_ID MATCHES "Clang")
        if(EXISTS ${CMAKE_BINARY_DIR}/compile_commands.json)
            # Create symlink to compile_commands.json for IDE to pick it up
            execute_process(
                COMMAND
                    ${CMAKE_COMMAND} -E create_symlink
                    ${CMAKE_BINARY_DIR}/compile_commands.json
                    ${CMAKE_CURRENT_SOURCE_DIR}/compile_commands.json
            )
        endif()
    endif()
endif()

add_library(liblddw
   "src/dict.c" 
    "src/ldd.c"
    "src/log.c"
    "src/args.c"
    "src/ldd_args.c"
)
add_library(lddw::lib ALIAS liblddw)

add_executable(
    lddw
    "src/main.c" 
)
target_link_libraries(lddw lddw::lib)

if(MSVC)
    set_property(        
       DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        PROPERTY VS_STARTUP_PROJECT lddw
    )
    set_property(
        TARGET lddw
        PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    )
endif()

enable_testing()
add_subdirectory("tests")