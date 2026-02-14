cmake_minimum_required(VERSION 3.18)
project(clrl LANGUAGES C RC)  # 【核心】添加RC语言支持

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# 静态链接依赖（保留）
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -static -static-libgcc")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static -static-libgcc")
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -static-libgcc")

# 输出目录（保留）
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/lib")

# Windows RC文件编码配置（恢复）
if(WIN32)
    set(CMAKE_RC_FLAGS "${CMAKE_RC_FLAGS} -c65001")  # UTF-8编码
endif()

# 1. 编译DLL（恢复RC文件）
add_library(clrl SHARED
    src/runtime/clrl_runtime.c
    src/version_dll.rc  # 恢复DLL的版本资源文件
)

target_include_directories(clrl
    PUBLIC ${CMAKE_SOURCE_DIR}/include
    PRIVATE ${CMAKE_SOURCE_DIR}/src/runtime
)

# 链接数学库（保留）
target_link_libraries(clrl m)

# DLL属性（保留）
set_target_properties(clrl PROPERTIES
    WINDOWS_EXPORT_ALL_SYMBOLS ON
    OUTPUT_NAME "clrl"
    SUFFIX ".dll"
)

# 2. 编译EXE（恢复RC文件）
add_executable(clrlc
    src/cli/clrlc.c
    src/version.rc  # 恢复EXE的版本资源文件
)

# 链接clrl编译目标（保留）
target_link_libraries(clrlc clrl m)

target_include_directories(clrlc
    PUBLIC ${CMAKE_SOURCE_DIR}/include
    PRIVATE ${CMAKE_SOURCE_DIR}/src/cli
)

# EXE属性（保留）
set_target_properties(clrlc PROPERTIES
    OUTPUT_NAME "clrlc"
    SUFFIX ".exe"
)