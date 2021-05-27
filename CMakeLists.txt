cmake_minimum_required (VERSION 3.5)
project(SoftRenderer)

# version
set (SoftRenderer_VERSION_MAJOR 1)
set (SoftRenderer_VERSION_MINOR 0)

# C++ 11 is required
set(CMAKE_CXX_STANDARD 11)


include_directories(include)

file(GLOB_RECURSE SRCS ./src/*.cpp)
file(GLOB_RECURSE HEADERS include/*.h)


# Create the executable
add_executable(${PROJECT_NAME} ${SRCS} ${HEADERS})

# 创建并指定可执行程序输出目录
file(MAKE_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin) 



file(COPY obj DESTINATION ${PROJECT_SOURCE_DIR}/bin)





