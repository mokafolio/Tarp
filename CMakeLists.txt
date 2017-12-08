cmake_minimum_required (VERSION 2.8.11)
project (Tarp)

find_package(OpenGL REQUIRED)

include_directories (${CMAKE_CURRENT_SOURCE_DIR} /usr/local/include)

SET(TARPINC
Tarp/Tarp.h
)

SET(TARPDEPS
${OPENGL_LIBRARIES}
)

add_subdirectory (Examples)
