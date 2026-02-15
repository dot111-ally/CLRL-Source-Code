cmake_minimum_required(VERSION 3.18)
project(CLRL VERSION 1.0.0 LANGUAGES C RC)

# 强制设置C99标准
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# ========== 关键修复：用绝对路径指定头文件目录 ==========
set(ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(INCLUDE_DIR ${ROOT_DIR}/include)
include_directories(${INCLUDE_DIR})

# ========== 收集源码文件（核心修复：拆分资源文件，避免重复） ==========
# 动态库（clrl.dll）的源码：仅用version_dll.rc
set(LIB_SOURCES
    ${ROOT_DIR}/src/runtime/clrl_runtime.c
    ${ROOT_DIR}/src/version_dll.rc  # DLL专属版本资源
)

# 可执行文件（clrlc.exe）的源码：仅用version.rc
set(EXE_SOURCES
    ${ROOT_DIR}/src/cli/clrlc.c
    ${ROOT_DIR}/src/version.rc  # EXE专属版本资源
)

# ========== 构建动态库 ==========
add_library(clrl SHARED ${LIB_SOURCES})
target_include_directories(clrl PRIVATE ${INCLUDE_DIR})
set_target_properties(clrl PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${ROOT_DIR}/bin
    LIBRARY_OUTPUT_DIRECTORY ${ROOT_DIR}/bin
    # 可选：隐藏DLL的符号警告（MinGW特有）
    LINK_FLAGS "-Wl,--enable-stdcall-fixup"
)

# ========== 构建可执行文件 ==========
add_executable(clrlc ${EXE_SOURCES})
target_include_directories(clrlc PRIVATE ${INCLUDE_DIR})
target_link_libraries(clrlc PRIVATE clrl)
set_target_properties(clrlc PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${ROOT_DIR}/bin
    # 可选：设置EXE图标（如果有icon.ico）
    # LINK_FLAGS "-Wl,--subsystem,windows -mwindows icon.ico"
)

# ========== 编译选项：关闭无关警告（兜底） ==========
if(MINGW)
    # 新增 -Wno-stringop-truncation 关闭 strncpy 截断警告
    target_compile_options(clrl PRIVATE -Wall -Wextra -O2 -Wno-unused-parameter -Wno-stringop-truncation)
    target_compile_options(clrlc PRIVATE -Wall -Wextra -O2 -Wno-unused-parameter -Wno-stringop-truncation)
    set_target_properties(clrl PROPERTIES LINK_FLAGS "-Wl,--allow-multiple-definition")
    set_target_properties(clrlc PROPERTIES LINK_FLAGS "-Wl,--allow-multiple-definition")
endif()