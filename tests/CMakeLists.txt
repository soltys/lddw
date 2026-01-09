add_library(soltystest STATIC "soltystest/soltystest.c")
add_library(soltystest::soltystest ALIAS soltystest)
target_include_directories(soltystest PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(test_args "test_args.c" )
target_link_libraries(test_args PRIVATE soltystest::soltystest lddw::lib)
target_include_directories(test_args PRIVATE ${CMAKE_SOURCE_DIR})
add_test(NAME test_args COMMAND test_args)