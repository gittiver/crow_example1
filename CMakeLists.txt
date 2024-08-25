cmake_minimum_required(VERSION 3.15)

enable_language(C)
enable_language(CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if (POLICY CMP0077)
    cmake_policy(SET CMP0077 NEW)
endif ()

project(crow-example1 VERSION 0.0.1 LANGUAGES CXX)

option(${PROJECT_NAME}_ENABLE_TESTS "enable testing" OFF)
include(FetchContent)
#
# crow project Configuration variables
#
set(CROW_BUILD_EXAMPLES Off)
set(CROW_BUILD_TOOLS Off)
set(CROW_BUILD_TESTS Off)
set(CROW_BUILD_DOCS Off)

# add crow project to the build
FetchContent_Declare(crow
        GIT_REPOSITORY https://github.com/CrowCpp/Crow.git
        GIT_TAG v1.2.0
        )

set(EXPECTED_BUILD_TESTS OFF)
if (NOT crow_POPULATED)
    FetchContent_Populate(crow)
    add_subdirectory(${crow_SOURCE_DIR} ${crow_BINARY_DIR})
endif (NOT crow_POPULATED)

# add tartan lamas expected library
FetchContent_Declare(expected
        GIT_REPOSITORY https://github.com/TartanLlama/expected.git
        GIT_TAG master
        )
if (NOT expected_POPULATED)
    FetchContent_Populate(expected)
    add_subdirectory(${expected_SOURCE_DIR})
endif (NOT expected_POPULATED)

include(CTest)
enable_testing()

set(CATCH_INSTALL_DOCS Off)
set(CATCH_INSTALL_EXTRAS Off)
FetchContent_Declare(
        Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG v3.3.1
)

FetchContent_MakeAvailable(Catch2)

add_library(lib-${PROJECT_NAME}
        src/model.cpp
        src/model.hpp
        src/auth.cpp
        src/auth.hpp
        src/auth.cpp
)
target_link_libraries(lib-${PROJECT_NAME} PUBLIC tl::expected Crow::Crow)

add_executable(${PROJECT_NAME} src/main.cpp src/bp_user_registration.cpp src/bp_user_registration.hpp)
target_link_libraries(${PROJECT_NAME}
        PRIVATE
        lib-${PROJECT_NAME}
        )
target_include_directories(${PROJECT_NAME} PRIVATE ${expected_SOURCE_DIR}/include)

add_executable(bp_main src/bp_main.cpp src/bp_user_registration.cpp src/bp_user_registration.hpp)
target_link_libraries(bp_main PRIVATE lib-${PROJECT_NAME})

add_executable(web_main src/web_main.cpp)
target_link_libraries(web_main PRIVATE lib-${PROJECT_NAME})
