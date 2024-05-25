cmake_minimum_required(VERSION 3.16)
project(physics-simulation-engine LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
option(BUILD_SHARED_LIBS "Build shared libraries" OFF)

#include(FetchContent)
#FetchContent_Declare(SFML
    #GIT_REPOSITORY https://github.com/SFML/SFML.git
    #GIT_TAG 2.6.x)
#FetchContent_MakeAvailable(SFML)

include_directories(libs)
set(SOURCES src/main.cpp src/Engine.cpp src/Renderer.cpp src/VerletObject.cpp src/Link.cpp
			libs/Engine.hpp libs/Renderer.hpp libs/VerletObject.hpp libs/Link.hpp)
add_executable(${PROJECT_NAME} ${SOURCES})

find_package(TGUI 1 REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE TGUI::TGUI)

find_package(SFML COMPONENTS graphics window system audio REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE sfml-graphics)

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)

# if(WIN32)
#     add_custom_command(
#         TARGET ${PROJECT_NAME}
#         COMMENT "Copy OpenAL DLL"
#         PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${SFML_SOURCE_DIR}/extlibs/bin/$<IF:$<EQUAL:${CMAKE_SIZEOF_VOID_P},8>,x64,x86>/openal32.dll $<TARGET_FILE_DIR:CMakeSFMLProject>
#         VERBATIM)
# endif()

install(TARGETS ${PROJECT_NAME})