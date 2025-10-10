# 设置当前目录下的源文件与头文件
set(SOURCE_FILES application.cpp)
set(HEADER_FILES application.h)

# 生成静态库 applicationLib
add_library(applicationLib STATIC ${SOURCE_FILES} ${HEADER_FILES})