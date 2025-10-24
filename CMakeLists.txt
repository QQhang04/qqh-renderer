# 需求的最低cmake程序版本
cmake_minimum_required(VERSION 3.12)

# 本工程的名字
project(Graphic)

if (MSVC)
    add_compile_options(/utf-8)
endif()

# 本工程支持的C++版本
set(CMAKE_CXX_STANDARD 17)

# 资源拷贝
file(GLOB copyResources "./assets")
file(COPY "${CMAKE_SOURCE_DIR}/assets" DESTINATION "${CMAKE_BINARY_DIR}")
file(GLOB copyResources "./assets" "./3rdParty/bin/assimp/assimp-vc143-mtd.dll")
 file(COPY ${copyResources} DESTINATION ${CMAKE_BINARY_DIR})

# 表示要从下述文件夹下寻找头文件
include_directories(
        SYSTEM ${CMAKE_CURRENT_SOURCE_DIR}/3rdParty/include
)

# 表示要从下述文件夹下寻找静态链接库
link_directories(
        SYSTEM ${CMAKE_CURRENT_SOURCE_DIR}/3rdParty/lib/assimp
)

add_subdirectory(application)
add_subdirectory(gpu)

# 本工程主程序文件及输出程序名称
add_executable(softRenderer "main.cpp")
add_executable(testMath "testMath.cpp")

## 将assimp动态链接库进行连接操作
target_link_libraries(softRenderer applicationLib gpuLib assimp-vc143-mtd.lib)
#target_link_libraries(softRenderer applicationLib gpuLib)