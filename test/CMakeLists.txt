enable_testing()

include(FetchContent)

FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.14.0
)

FetchContent_MakeAvailable(googletest)

include(GoogleTest)

set(TEST_ROOT ${PROJECT_SOURCE_DIR}/test)
add_compile_definitions(TEST_ROOT="${TEST_ROOT}")

add_library(test_util STATIC test_util.cpp)
target_link_libraries(test_util gtest)
target_include_directories(test_util PUBLIC ${PROJECT_SOURCE_DIR}/src)

file(GLOB TESTS *_test.cpp)

foreach (file ${TESTS})
    get_filename_component(filename ${file} NAME_WE)
    add_executable(${filename} ${file})
    target_link_libraries(${filename} gtest_main lox test_util)
    gtest_discover_tests(${filename})
    add_test(NAME ${filename} COMMAND ${filename})
endforeach ()
