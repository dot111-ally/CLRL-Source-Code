#ifndef CLRL_RUNTIME_H
#define CLRL_RUNTIME_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <math.h>
#include <sys/stat.h>

// 跨平台导出/导入宏
#ifdef _WIN32
    #define CLRL_API __declspec(dllexport)
#else
    #define CLRL_API
#endif

// 【核心修复】补充Windows必要头文件（包含_mkdir声明）
#ifdef _WIN32
    #include <windows.h>
    #include <direct.h>  // 必须添加：_mkdir函数的声明头文件
    #define PATH_SEP "\\"
    #define EXE_EXT ".exe"
    #define COMPILER "gcc"
    #define getpid() (unsigned long)GetCurrentProcessId()
    #define getcwd _getcwd
    #define mkdir(dir) _mkdir(dir)
#else
    #define PATH_SEP "/"
    #define EXE_EXT ""
    #define COMPILER "gcc"
#endif

// 全局宏定义（保留）
#define MAX_INPUT_LEN 1024
#define MAX_CODE_BUFFER 4096
#define PROMPT "clrl> "
#define TEMP_PREFIX "C_"
#define PATH_BUF_SIZE 1024
#define CMD_BUF_SIZE 2048
#define CACHE_DIR "clrl_cache"
#define BIG_INT_MAX_LEN 1024
#define PRECISE_MAX_LEN 64

// 简化控制台编码设置（保留）
static inline void set_console_utf8() {
    return;
}

// 核心函数声明（保留）
CLRL_API int clrl_calculate_direct(const char* clrl_code, char* result, size_t res_len);
CLRL_API int clrl_generate_c_code(const char* clrl_file, const char* c_file);
CLRL_API int clrl_compile_c_code(const char* clrl_code, const char* c_file, const char* exe_file);
CLRL_API void clrl_get_temp_path(char* buf, size_t buf_size, const char* suffix);
CLRL_API void clrl_ask_cleanup(const char* clrl, const char* c, const char* exe);

#endif // CLRL_RUNTIME_H