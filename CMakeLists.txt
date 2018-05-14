cmake_minimum_required (VERSION 2.8.11)

#set(CMAKE_C_FLAGS "-std=c89 -pedantic-errors") 

project (Tarp C)

find_package(OpenGL REQUIRED)

include_directories (${CMAKE_CURRENT_SOURCE_DIR} /usr/local/include)

SET(TARPINC
Tarp/Tarp.h
Tarp/TarpArray.h
)

SET(TARPDEPS
${OPENGL_LIBRARIES}
)

add_subdirectory (Examples)
