#include "clrl/clrl_runtime.h"
#include <stdio.h>
#include <string.h>

// REPL交互模式
static void repl_mode() {
    char buf[MAX_CODE_BUFFER] = {0};
    char line[MAX_INPUT_LEN];
    int ln = 1;

    // 设置控制台UTF-8编码
    set_console_utf8();

    // 英文提示
    printf("CLRL REPL\n");
    printf("Supports:\n");
    printf("  - larnum: Arbitrary large integer (+, -, *, /) - no overflow\n");
    printf("  - precise: Decimal (+, -, *, /) - no floating point error\n");
    printf("  - return: Output multiple variables (format: return var1,var2,var3)\n");
    printf("Commands: 'system:fast'=instant output | 'system:compile'=compiled output | 'system:clear'=clear buffer | 'system:exit'=exit\n");
    
    while (1) {
        printf("%s(%d) ", PROMPT, ln);
        fflush(stdout);
        if (!fgets(line, sizeof(line), stdin)) break;

        line[strcspn(line, "\r\n")] = 0;

        // 核心命令
        if (strcmp(line, "system:exit") == 0) break;
        if (strcmp(line, "system:clear") == 0) {
            memset(buf, 0, sizeof(buf));
            ln = 1;
            continue;
        }

        // 极速模式
        if (strcmp(line, "system:fast") == 0) {
            char result[BIG_INT_MAX_LEN * 2] = {0}; // 足够大的缓冲区
            if (clrl_calculate_direct(buf, result, sizeof(result)) == 0) {
                printf("\n--- INSTANT OUTPUT ---\n%s\n----------------------\n", result);
            } else {
                printf("Error: Calculation failed! Check your code syntax.\n");
            }
            memset(buf, 0, sizeof(buf));
            ln = 1;
            continue;
        }

        // 编译模式
        if (strcmp(line, "system:compile") == 0) {
            char fcl[PATH_BUF_SIZE], fc[PATH_BUF_SIZE], fex[PATH_BUF_SIZE];
            clrl_get_temp_path(fcl, sizeof(fcl), ".clrl");
            clrl_get_temp_path(fc, sizeof(fc), ".c");
            clrl_get_temp_path(fex, sizeof(fex), EXE_EXT);

            // 写入CLRL代码到临时文件
            FILE* f = fopen(fcl, "w");
            if (f) { 
                fputs(buf, f); 
                fclose(f); 
            }

            // 生成并编译C代码
            if (clrl_generate_c_code(fcl, fc) == 0 && clrl_compile_c_code(buf, fc, fex) == 0) {
                printf("\n--- COMPILED OUTPUT ---\n");
#ifdef _WIN32
                char run_cmd[PATH_BUF_SIZE + 4];
                snprintf(run_cmd, sizeof(run_cmd) - 1, "\"%s\"", fex);
                system(run_cmd);
#else
                system(fex);
#endif
                printf("------------------------\n");
                clrl_ask_cleanup(fcl, fc, fex);
            } else {
                printf("Error: Compilation failed! Check your code syntax.\n");
            }
            memset(buf, 0, sizeof(buf));
            ln = 1;
            continue;
        }

        // 追加代码到缓冲区
        strcat(buf, line);
        strcat(buf, "\n");
        ln++;
    }
}

// 文件运行模式
static int file_mode(const char* filename, const char* mode) {
    FILE* f = fopen(filename, "r");
    if (!f) {
        printf("Error: Cannot open file '%s'\n", filename);
        return -1;
    }

    // 读取文件内容
    char code[MAX_CODE_BUFFER] = {0};
    char line[MAX_INPUT_LEN];
    while (fgets(line, sizeof(line), f)) {
        strcat(code, line);
    }
    fclose(f);

    // 极速模式
    if (strcmp(mode, "--fast") == 0) {
        char result[BIG_INT_MAX_LEN * 2] = {0};
        if (clrl_calculate_direct(code, result, sizeof(result)) == 0) {
            printf("Instant Output:\n%s\n", result);
            return 0;
        } else {
            printf("Error: Calculation failed for file '%s'\n", filename);
            return -1;
        }
    }

    // 编译模式
    if (strcmp(mode, "--compile") == 0) {
        char fc[PATH_BUF_SIZE], fex[PATH_BUF_SIZE];
        clrl_get_temp_path(fc, sizeof(fc), ".c");
        clrl_get_temp_path(fex, sizeof(fex), EXE_EXT);

        if (clrl_generate_c_code(filename, fc) == 0 && clrl_compile_c_code(code, fc, fex) == 0) {
            printf("Compiled Output:\n");
#ifdef _WIN32
            char run_cmd[PATH_BUF_SIZE + 4];
            snprintf(run_cmd, sizeof(run_cmd) - 1, "\"%s\"", fex);
            system(run_cmd);
#else
            system(fex);
#endif
            clrl_ask_cleanup(NULL, fc, fex);
            return 0;
        } else {
            printf("Error: Compilation failed for file '%s'\n", filename);
            return -1;
        }
    }

    // 无效模式
    printf("Error: Invalid mode '%s'\n", mode);
    printf("Valid modes: --fast (instant output) | --compile (compiled output)\n");
    return -1;
}

// 显示帮助
static void show_help() {
    printf("CLRL Language Interpreter v1.0.0\n");
    printf("Usage:\n");
    printf("  clrlc                     - Start interactive REPL mode\n");
    printf("  clrlc <file.clrl> --fast  - Run CLRL file in instant mode (no compilation)\n");
    printf("  clrlc <file.clrl> --compile - Run CLRL file in compiled mode\n");
    printf("  clrlc --help              - Show this help message\n");
    printf("\nFeatures:\n");
    printf("  - larnum: Arbitrary large integer arithmetic (+, -, *, /) - no overflow\n");
    printf("  - precise: Decimal arithmetic (+, -, *, /) - no floating point error\n");
    printf("  - return: Output multiple variables (format: return var1,var2)\n");
}

int main(int argc, char* argv[]) {
    // 设置控制台UTF-8
    set_console_utf8();

    // 解析命令行参数
    if (argc == 1) {
        repl_mode();
    } else if (argc == 2) {
        if (strcmp(argv[1], "--help") == 0) {
            show_help();
        } else {
            printf("Error: Missing mode argument\n");
            printf("Use 'clrlc --help' for usage information\n");
            return -1;
        }
    } else if (argc == 3) {
        file_mode(argv[1], argv[2]);
    } else {
        printf("Error: Too many arguments\n");
        printf("Use 'clrlc --help' for usage information\n");
        return -1;
    }

    return 0;
}