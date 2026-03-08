cmake_minimum_required(VERSION 3.20)
project(AlienMultiplicity CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(FetchContent)
FetchContent_Declare(
  litegdk
  GIT_REPOSITORY https://github.com/77smith-norm/LiteGDK.git
  GIT_TAG        main
)
FetchContent_MakeAvailable(litegdk)

add_executable(alien_multiplicity
    src/main.cpp
)
target_link_libraries(alien_multiplicity PRIVATE litegdk_entrypoint)
target_include_directories(alien_multiplicity PRIVATE src)

file(COPY ${CMAKE_SOURCE_DIR}/assets DESTINATION ${CMAKE_BINARY_DIR})

if(EMSCRIPTEN)
    set_target_properties(alien_multiplicity PROPERTIES SUFFIX ".html")
    target_link_options(alien_multiplicity PRIVATE
        -sASYNCIFY=1
        -sASYNCIFY_STACK_SIZE=131072
        -sALLOW_MEMORY_GROWTH=1
        -sUSE_GLFW=3
        -sWASM=1
        --shell-file ${CMAKE_SOURCE_DIR}/web/shell.html
        --preload-file ${CMAKE_SOURCE_DIR}/assets/@/assets
    )
endif()
