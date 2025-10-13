# 需求的最低cmake程序版本
cmake_minimum_required(VERSION 3.12)

# 本工程的名字
project(Graphic)

# 本工程支持的C++版本
set(CMAKE_CXX_STANDARD 17)

# 资源拷贝
file(GLOB copyResources "./assets")
file(COPY "${CMAKE_SOURCE_DIR}/assets" DESTINATION "${CMAKE_BINARY_DIR}")
# file(COPY ${copyResources} DESTINATION ${CMAKE_BINARY_DIR})

add_subdirectory(application)
add_subdirectory(gpu)

# 本工程主程序文件及输出程序名称
add_executable(softRenderer "main.cpp")
add_executable(testMath "testMath.cpp")

target_link_libraries(softRenderer applicationLib gpuLib)