# 收集 gpu 目录下全部源码
file(GLOB_RECURSE APP_SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
)

# 生成静态库 gpuLib
add_library(applicationLib STATIC ${APP_SRC})