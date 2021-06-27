cmake_minimum_required (VERSION 3.5)
project(SoftRenderer)

# version
set (SoftRenderer_VERSION_MAJOR 1)
set (SoftRenderer_VERSION_MINOR 0)

# C++ 11 is required
set(CMAKE_CXX_STANDARD 11)

include_directories(include)
include_directories(${PROJECT_SOURCE_DIR}/external/include)

IF (CMAKE_SYSTEM_NAME MATCHES "Windows")
	link_directories(${PROJECT_SOURCE_DIR}/external/libs)
ELSEIF (CMAKE_SYSTEM_NAME MATCHES "Linux")
	find_package(SDL2 REQUIRED)

	# check if boost was found
	if(SDL2_FOUND)
	    message ("SDL2 found")
	else()
	    message (FATAL_ERROR "Cannot find SDL2")
	endif()
ENDIF()

file(GLOB_RECURSE SRCS ./src/*.cpp)
file(GLOB_RECURSE HEADERS include/*.h)


# # Create the executable
# add_executable(${PROJECT_NAME} ${SRCS} ${HEADERS})

add_library(${PROJECT_NAME} ${SRCS} ${HEADERS})
add_library(SoftRenderer::renderer ALIAS ${PROJECT_NAME})

# link the target with the SDL2
target_link_libraries( ${PROJECT_NAME} 
    SDL2
	SDL2main
	assimp
)


# 创建并指定可执行程序输出目录
file(MAKE_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin) 


# 将模型文件移至可执行文件目录下
# file(COPY obj DESTINATION ${PROJECT_SOURCE_DIR}/bin)


# 将dll文件移至可执行文件目录下
IF (CMAKE_SYSTEM_NAME MATCHES "Windows")
	file(MAKE_DIRECTORY ${PROJECT_SOURCE_DIR}/bin/Debug)
	file(COPY external/dlls/ DESTINATION ${PROJECT_SOURCE_DIR}/bin/Debug)
ELSEIF (CMAKE_SYSTEM_NAME MATCHES "Linux")
	file(COPY external/dlls/ DESTINATION ${PROJECT_SOURCE_DIR}/bin)
ENDIF()



# Add sub directories
add_subdirectory(examples/01_PureColor)
add_subdirectory(examples/02_UV)
add_subdirectory(examples/03_Phong) 
add_subdirectory(examples/04_Texture) 
add_subdirectory(examples/05_BumpedNormal) 
add_subdirectory(examples/06_ShadowMap) 
add_subdirectory(examples/08_PointLight) 