cmake_minimum_required (VERSION 3.5)

project(12_AmbientOcclusion)

# C++ 11 is required
set(CMAKE_CXX_STANDARD 11)

include_directories(../../include)
include_directories(../../external/include)

# 创建并指定可执行程序输出目录
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/../bin)
SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/../bin) 

# Create the executable
add_executable(${PROJECT_NAME} main.cpp)

target_link_libraries(${PROJECT_NAME}
    SoftRenderer::renderer
)
