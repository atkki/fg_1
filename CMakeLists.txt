cmake_minimum_required (VERSION 3.11)

project ("fg_1")

# src
file (GLOB SRC "src/Image.h" "src/Image.cpp" "src/main.cpp")

# conan setup
include(conan.cmake)

conan_cmake_run(
    CONANFILE conanfile.txt
    BASIC_SETUP
    BUILD missing
)

# exec 
add_executable (fg_1 ${SRC})
target_link_libraries(fg_1 ${CONAN_LIBS})

# copy media files 
file(COPY media/ DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/bin/media/)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET fg_1 PROPERTY CXX_STANDARD 20)
endif()