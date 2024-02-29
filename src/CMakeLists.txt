file(GLOB SOURCES *.cpp)
add_library(lox STATIC ${SOURCES})
target_link_libraries(lox 3rd)
target_include_directories(lox PUBLIC ${PROJECT_SOURCE_DIR}/3rd)
