cmake_minimum_required(VERSION 3.11)

project(Minesweeper VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Fetch Raylib
include(FetchContent)
FetchContent_Declare(
    raylib
    URL https://github.com/raysan5/raylib/archive/refs/tags/4.5.0.tar.gz
)
FetchContent_MakeAvailable(raylib)

# Add executable
file(GLOB_RECURSE SOURCES "src/*.cpp" "src/*.h")

if(PLATFORM STREQUAL "Web")
    add_executable(${PROJECT_NAME} ${SOURCES})
    set_target_properties(${PROJECT_NAME} PROPERTIES 
        OUTPUT_NAME "index"
        SUFFIX ".html"
        LINK_FLAGS "-s USE_GLFW=3 -s ASYNCIFY --shell-file ${CMAKE_CURRENT_SOURCE_DIR}/src/shell.html --preload-file ${CMAKE_CURRENT_SOURCE_DIR}/flag.ico@flag.ico"
    )
else()
    add_executable(${PROJECT_NAME} WIN32 ${SOURCES} "resources.rc")
    if(MSVC)
        set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "/ENTRY:mainCRTStartup")
    endif()
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE raylib)

# Copy icon to build folder so it can be loaded at runtime
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${CMAKE_CURRENT_SOURCE_DIR}/flag.ico"
    "$<TARGET_FILE_DIR:${PROJECT_NAME}>/flag.ico"
)


# Copy assets if we had them (not yet, but good practice)
# file(COPY assets DESTINATION ${CMAKE_BINARY_DIR})
