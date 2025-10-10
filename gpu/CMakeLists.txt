# 收集 gpu 目录下全部源码
file(GLOB_RECURSE GPU_SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
)

# 生成静态库 gpuLib
add_library(gpuLib STATIC ${GPU_SRC})

# 头文件搜索路径
 target_include_directories(gpuLib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}        # gpu 目录
    ${CMAKE_CURRENT_SOURCE_DIR}/../global
)