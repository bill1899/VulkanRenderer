#项目根目录CMake文件

#指定CMake版本
cmake_minimum_required(VERSION 3.18)

#项目名称
project(VulkanEngine LANGUAGES CXX)

#设置C++语言特性
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#添加子目录
add_subdirectory(thirdparty)
add_subdirectory(src)