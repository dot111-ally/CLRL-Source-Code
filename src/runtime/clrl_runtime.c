#include "clrl/clrl_runtime.h"
#include <ctype.h>

// ========================
// 1. 超大整数（larnum）四则运算核心函数
// ========================

// 辅助：比较两个超大整数大小（a > b 返回1，a == b 返回0，a < b 返回-1）
static int big_int_compare(const char* a, const char* b) {
    int len_a = strlen(a);
    int len_b = strlen(b);
    if (len_a != len_b) return len_a - len_b;
    return strcmp(a, b);
}

// 辅助：超大整数加法
static void big_int_add(const char* a, const char* b, char* result) {
    char a_rev[BIG_INT_MAX_LEN] = {0}, b_rev[BIG_INT_MAX_LEN] = {0};
    char res_rev[BIG_INT_MAX_LEN + 1] = {0};
    int len_a = strlen(a), len_b = strlen(b);
    int max_len = len_a > len_b ? len_a : len_b;
    int carry = 0, i = 0;

    // 反转字符串方便从低位计算
    for (i = 0; i < len_a; i++) a_rev[i] = a[len_a - 1 - i];
    for (i = 0; i < len_b; i++) b_rev[i] = b[len_b - 1 - i];

    // 逐位相加
    for (i = 0; i < max_len; i++) {
        int digit_a = i < len_a ? (a_rev[i] - '0') : 0;
        int digit_b = i < len_b ? (b_rev[i] - '0') : 0;
        int sum = digit_a + digit_b + carry;
        res_rev[i] = (sum % 10) + '0';
        carry = sum / 10;
    }
    if (carry > 0) res_rev[i++] = carry + '0';

    // 反转结果
    for (int j = 0; j < i; j++) result[j] = res_rev[i - 1 - j];
    result[i] = '\0';
}

// 辅助：超大整数减法（确保a >= b）
static void big_int_sub(const char* a, const char* b, char* result) {
    char a_rev[BIG_INT_MAX_LEN] = {0}, b_rev[BIG_INT_MAX_LEN] = {0};
    char res_rev[BIG_INT_MAX_LEN] = {0};
    int len_a = strlen(a), len_b = strlen(b);
    int borrow = 0, i = 0;

    // 反转字符串
    for (i = 0; i < len_a; i++) a_rev[i] = a[len_a - 1 - i];
    for (i = 0; i < len_b; i++) b_rev[i] = b[len_b - 1 - i];

    // 逐位相减
    for (i = 0; i < len_a; i++) {
        int digit_a = a_rev[i] - '0';
        int digit_b = i < len_b ? (b_rev[i] - '0') : 0;
        digit_a -= borrow;
        borrow = 0;

        if (digit_a < digit_b) {
            digit_a += 10;
            borrow = 1;
        }
        res_rev[i] = (digit_a - digit_b) + '0';
    }

    // 去除前导0
    int res_len = len_a;
    while (res_len > 1 && res_rev[res_len - 1] == '0') res_len--;

    // 反转结果
    for (int j = 0; j < res_len; j++) result[j] = res_rev[res_len - 1 - j];
    result[res_len] = '\0';
}

// 辅助：超大整数乘法
static void big_int_mul(const char* a, const char* b, char* result) {
    int len_a = strlen(a), len_b = strlen(b);
    int res[BIG_INT_MAX_LEN * 2] = {0};

    // 逐位相乘
    for (int i = len_a - 1; i >= 0; i--) {
        for (int j = len_b - 1; j >= 0; j--) {
            int digit_a = a[i] - '0';
            int digit_b = b[j] - '0';
            res[i + j + 1] += digit_a * digit_b;
            res[i + j] += res[i + j + 1] / 10;
            res[i + j + 1] %= 10;
        }
    }

    // 转换为字符串（跳过前导0）
    int idx = 0;
    while (idx < len_a + len_b && res[idx] == 0) idx++;
    if (idx == len_a + len_b) {
        strcpy(result, "0");
        return;
    }

    int res_idx = 0;
    for (; idx < len_a + len_b; idx++) result[res_idx++] = res[idx] + '0';
    result[res_idx] = '\0';
}

// 辅助：超大整数除法（返回商，余数通过参数输出）
static void big_int_div(const char* a, const char* b, char* quotient, char* remainder) {
    char dividend[BIG_INT_MAX_LEN] = {0};
    char temp[BIG_INT_MAX_LEN] = {0};
    int len_a = strlen(a), len_b = strlen(b);
    int q_idx = 0;

    // 除数为0直接返回0
    if (strcmp(b, "0") == 0) {
        strcpy(quotient, "0");
        strcpy(remainder, a);
        return;
    }

    // 被除数小于除数，商为0，余数为被除数
    if (big_int_compare(a, b) < 0) {
        strcpy(quotient, "0");
        strcpy(remainder, a);
        return;
    }

    // 逐位计算商
    for (int i = 0; i < len_a; i++) {
        // 追加当前位到被除数
        int div_len = strlen(dividend);
        dividend[div_len] = a[i];
        dividend[div_len + 1] = '\0';

        // 去除被除数前导0
        int trim = 0;
        while (dividend[trim] == '0' && trim < strlen(dividend) - 1) trim++;
        if (trim > 0) memmove(dividend, dividend + trim, strlen(dividend) - trim + 1);

        // 计算当前位商
        int digit = 0;
        while (big_int_compare(dividend, b) >= 0) {
            big_int_sub(dividend, b, temp);
            strcpy(dividend, temp);
            digit++;
        }

        // 记录商（跳过前导0）
        if (digit > 0 || q_idx > 0) quotient[q_idx++] = digit + '0';
    }

    // 处理商为空的情况
    if (q_idx == 0) quotient[q_idx++] = '0';
    quotient[q_idx] = '\0';
    strcpy(remainder, dividend);
}

// ========================
// 2. 精确小数（precise）四则运算核心函数
// ========================

// 辅助：解析小数为整数部分+小数部分
static void parse_precise(const char* str, long long* int_part, long long* frac_part, int* frac_len) {
    *int_part = 0;
    *frac_part = 0;
    *frac_len = 0;

    char* dot = strchr(str, '.');
    if (dot) {
        char int_str[32] = {0}, frac_str[32] = {0};
        strncpy(int_str, str, dot - str);
        strcpy(frac_str, dot + 1);
        
        *int_part = atoll(int_str);
        *frac_part = atoll(frac_str);
        *frac_len = strlen(frac_str);

        // 去除小数部分末尾0
        while (*frac_len > 0 && *frac_part % 10 == 0) {
            *frac_part /= 10;
            (*frac_len)--;
        }
    } else {
        *int_part = atoll(str);
    }
}

// 辅助：精确小数加法
static void precise_add(const char* a, const char* b, char* result) {
    long long a_int, a_frac, b_int, b_frac;
    int a_frac_len, b_frac_len;

    parse_precise(a, &a_int, &a_frac, &a_frac_len);
    parse_precise(b, &b_int, &b_frac, &b_frac_len);

    // 统一小数位数
    int max_frac = a_frac_len > b_frac_len ? a_frac_len : b_frac_len;
    long long a_frac_pad = a_frac * (long long)pow(10, max_frac - a_frac_len);
    long long b_frac_pad = b_frac * (long long)pow(10, max_frac - b_frac_len);

    // 计算
    long long frac_sum = a_frac_pad + b_frac_pad;
    long long int_sum = a_int + b_int + (frac_sum / (long long)pow(10, max_frac));
    long long frac_final = frac_sum % (long long)pow(10, max_frac);

    // 处理末尾0
    int trim_len = max_frac;
    while (trim_len > 0 && frac_final % 10 == 0) {
        frac_final /= 10;
        trim_len--;
    }

    // 拼接结果
    if (trim_len > 0 && frac_final > 0) {
        snprintf(result, PRECISE_MAX_LEN, "%lld.%0*lld", int_sum, trim_len, frac_final);
    } else {
        snprintf(result, PRECISE_MAX_LEN, "%lld", int_sum);
    }
}

// 辅助：精确小数减法
static void precise_sub(const char* a, const char* b, char* result) {
    long long a_int, a_frac, b_int, b_frac;
    int a_frac_len, b_frac_len;
    int is_negative = 0;

    parse_precise(a, &a_int, &a_frac, &a_frac_len);
    parse_precise(b, &b_int, &b_frac, &b_frac_len);

    // 统一小数位数
    int max_frac = a_frac_len > b_frac_len ? a_frac_len : b_frac_len;
    long long a_total = a_int * (long long)pow(10, max_frac) + a_frac * (long long)pow(10, max_frac - a_frac_len);
    long long b_total = b_int * (long long)pow(10, max_frac) + b_frac * (long long)pow(10, max_frac - b_frac_len);

    // 确保大数减小数
    if (a_total < b_total) {
        long long temp = a_total;
        a_total = b_total;
        b_total = temp;
        is_negative = 1;
    }

    // 计算
    long long total_diff = a_total - b_total;
    long long int_part = total_diff / (long long)pow(10, max_frac);
    long long frac_part = total_diff % (long long)pow(10, max_frac);

    // 处理末尾0
    int trim_len = max_frac;
    while (trim_len > 0 && frac_part % 10 == 0) {
        frac_part /= 10;
        trim_len--;
    }

    // 拼接结果
    if (is_negative) {
        if (trim_len > 0 && frac_part > 0) {
            snprintf(result, PRECISE_MAX_LEN, "-%lld.%0*lld", int_part, trim_len, frac_part);
        } else {
            snprintf(result, PRECISE_MAX_LEN, "-%lld", int_part);
        }
    } else {
        if (trim_len > 0 && frac_part > 0) {
            snprintf(result, PRECISE_MAX_LEN, "%lld.%0*lld", int_part, trim_len, frac_part);
        } else {
            snprintf(result, PRECISE_MAX_LEN, "%lld", int_part);
        }
    }
}

// 辅助：精确小数乘法
static void precise_mul(const char* a, const char* b, char* result) {
    long long a_int, a_frac, b_int, b_frac;
    int a_frac_len, b_frac_len;

    parse_precise(a, &a_int, &a_frac, &a_frac_len);
    parse_precise(b, &b_int, &b_frac, &b_frac_len);

    // 转换为整数计算
    long long a_total = a_int * (long long)pow(10, a_frac_len) + a_frac;
    long long b_total = b_int * (long long)pow(10, b_frac_len) + b_frac;
    long long product = a_total * b_total;

    // 总小数位数
    int total_frac = a_frac_len + b_frac_len;
    long long int_part = product / (long long)pow(10, total_frac);
    long long frac_part = product % (long long)pow(10, total_frac);

    // 处理末尾0
    int trim_len = total_frac;
    while (trim_len > 0 && frac_part % 10 == 0) {
        frac_part /= 10;
        trim_len--;
    }

    // 拼接结果
    if (trim_len > 0 && frac_part > 0) {
        snprintf(result, PRECISE_MAX_LEN, "%lld.%0*lld", int_part, trim_len, frac_part);
    } else {
        snprintf(result, PRECISE_MAX_LEN, "%lld", int_part);
    }
}

// 辅助：精确小数除法（保留10位小数）
static void precise_div(const char* a, const char* b, char* result) {
    long long a_int, a_frac, b_int, b_frac;
    int a_frac_len, b_frac_len;
    const int PRECISION = 10; // 保留10位小数

    parse_precise(a, &a_int, &a_frac, &a_frac_len);
    parse_precise(b, &b_int, &b_frac, &b_frac_len);

    // 除数为0返回错误
    if (b_int == 0 && b_frac == 0) {
        strcpy(result, "Error: Division by zero");
        return;
    }

    // 转换为整数计算（放大10^PRECISION倍）
    long long a_total = (a_int * (long long)pow(10, a_frac_len) + a_frac) * (long long)pow(10, PRECISION);
    long long b_total = b_int * (long long)pow(10, b_frac_len) + b_frac;
    long long total_frac = a_frac_len;

    long long quotient = a_total / (b_total * (long long)pow(10, total_frac));
    long long remainder = a_total % (b_total * (long long)pow(10, total_frac));

    // 计算整数部分和小数部分
    long long int_part = quotient / (long long)pow(10, PRECISION);
    long long frac_part = quotient % (long long)pow(10, PRECISION);

    // 四舍五入
    if (remainder * 2 >= b_total * (long long)pow(10, total_frac)) {
        frac_part++;
        if (frac_part >= (long long)pow(10, PRECISION)) {
            frac_part = 0;
            int_part++;
        }
    }

    // 处理末尾0
    int trim_len = PRECISION;
    while (trim_len > 0 && frac_part % 10 == 0) {
        frac_part /= 10;
        trim_len--;
    }

    // 拼接结果
    if (trim_len > 0 && frac_part > 0) {
        snprintf(result, PRECISE_MAX_LEN, "%lld.%0*lld", int_part, trim_len, frac_part);
    } else {
        snprintf(result, PRECISE_MAX_LEN, "%lld", int_part);
    }
}

// ========================
// 3. 通用辅助函数
// ========================

// 检查字符串是否为纯数字
static int is_digit_string(const char* str) {
    if (str[0] == '-' || str[0] == '+') str++;
    for (int i = 0; str[i]; i++) {
        if (!isdigit(str[i]) && str[i] != '.') return 0;
    }
    return 1;
}

// 查找变量索引
static int find_var_index(const char* name, const char var_names[10][32], int var_count) {
    for (int i = 0; i < var_count; i++) {
        if (strcmp(var_names[i], name) == 0) return i;
    }
    return -1;
}

// 分割逗号分隔的字符串
static int split_comma_string(const char* str, char** tokens, int max_tokens) {
    char temp[256];
    strncpy(temp, str, sizeof(temp)-1);
    temp[sizeof(temp)-1] = '\0';
    
    int count = 0;
    char* token = strtok(temp, ",");
    while (token != NULL && count < max_tokens) {
        // 去除空格
        char* start = token;
        while (*start == ' ') start++;
        char* end = start + strlen(start) - 1;
        while (end > start && *end == ' ') end--;
        *(end+1) = '\0';
        
        tokens[count] = malloc(strlen(start)+1);
        strcpy(tokens[count], start);
        count++;
        token = strtok(NULL, ",");
    }
    return count;
}

// ========================
// 4. 核心计算函数
// ========================
CLRL_API int clrl_calculate_direct(const char* clrl_code, char* result, size_t res_len) {
    char line[256];
    char* ptr = (char*)clrl_code;
    
    // 变量存储
    char larnum_vars[10][BIG_INT_MAX_LEN] = {0};  // 超大整数变量
    int larnum_defined[10] = {0};                 // 整数变量是否定义
    char precise_vars[10][PRECISE_MAX_LEN] = {0}; // 精确小数变量
    char var_names[10][32] = {0};                 // 变量名
    int var_count = 0;
    
    // 返回变量
    char* return_vars[10] = {NULL};
    int return_count = 0;

    // 逐行解析代码
    while (sscanf(ptr, "%255[^\n]\n", line) == 1) {
        ptr += strlen(line) + 1;
        if (line[0] == '\0') continue;

        // 解析larnum（超大整数四则运算）
        if (strstr(line, "larnum")) {
            char v[32], a_str[BIG_INT_MAX_LEN], op[4], b_str[BIG_INT_MAX_LEN];
            // 匹配 变量 = 操作数 运算符 操作数
            if (sscanf(line, "larnum %s = %s %s %s", v, a_str, op, b_str) == 4) {
                char a_val[BIG_INT_MAX_LEN] = {0}, b_val[BIG_INT_MAX_LEN] = {0};
                int a_idx = find_var_index(a_str, var_names, var_count);
                int b_idx = find_var_index(b_str, var_names, var_count);

                // 获取操作数的值
                if (a_idx != -1 && larnum_defined[a_idx]) {
                    strcpy(a_val, larnum_vars[a_idx]);
                } else if (is_digit_string(a_str)) {
                    strcpy(a_val, a_str);
                } else {
                    continue;
                }

                if (b_idx != -1 && larnum_defined[b_idx]) {
                    strcpy(b_val, larnum_vars[b_idx]);
                } else if (is_digit_string(b_str)) {
                    strcpy(b_val, b_str);
                } else {
                    continue;
                }

                // 执行四则运算
                int var_idx = find_var_index(v, var_names, var_count);
                if (var_idx == -1) var_idx = var_count++;
                strcpy(var_names[var_idx], v);
                larnum_defined[var_idx] = 1;

                if (strcmp(op, "+") == 0) {
                    big_int_add(a_val, b_val, larnum_vars[var_idx]);
                } else if (strcmp(op, "-") == 0) {
                    if (big_int_compare(a_val, b_val) >= 0) {
                        big_int_sub(a_val, b_val, larnum_vars[var_idx]);
                    } else {
                        char temp[BIG_INT_MAX_LEN];
                        big_int_sub(b_val, a_val, temp);
                        snprintf(larnum_vars[var_idx], BIG_INT_MAX_LEN, "-%s", temp);
                    }
                } else if (strcmp(op, "*") == 0) {
                    big_int_mul(a_val, b_val, larnum_vars[var_idx]);
                } else if (strcmp(op, "/") == 0) {
                    char remainder[BIG_INT_MAX_LEN];
                    big_int_div(a_val, b_val, larnum_vars[var_idx], remainder);
                }
            } 
            // 匹配 变量 = 数值
            else if (sscanf(line, "larnum %s = %s", v, a_str) == 2) {
                if (is_digit_string(a_str)) {
                    int var_idx = find_var_index(v, var_names, var_count);
                    if (var_idx == -1) var_idx = var_count++;
                    strcpy(var_names[var_idx], v);
                    strcpy(larnum_vars[var_idx], a_str);
                    larnum_defined[var_idx] = 1;
                }
            }
        }

        // 解析precise（精确小数四则运算）
        if (strstr(line, "precise")) {
            char v[32], a_str[PRECISE_MAX_LEN], op[4], b_str[PRECISE_MAX_LEN];
            // 匹配 变量 = 操作数 运算符 操作数
            if (sscanf(line, "precise %s = %s %s %s", v, a_str, op, b_str) == 4) {
                char a_val[PRECISE_MAX_LEN] = {0}, b_val[PRECISE_MAX_LEN] = {0};
                int a_idx = find_var_index(a_str, var_names, var_count);
                int b_idx = find_var_index(b_str, var_names, var_count);

                // 获取操作数的值
                if (a_idx != -1) {
                    strcpy(a_val, precise_vars[a_idx]);
                } else if (is_digit_string(a_str)) {
                    strcpy(a_val, a_str);
                } else {
                    continue;
                }

                if (b_idx != -1) {
                    strcpy(b_val, precise_vars[b_idx]);
                } else if (is_digit_string(b_str)) {
                    strcpy(b_val, b_str);
                } else {
                    continue;
                }

                // 执行四则运算
                int var_idx = find_var_index(v, var_names, var_count);
                if (var_idx == -1) var_idx = var_count++;
                strcpy(var_names[var_idx], v);

                if (strcmp(op, "+") == 0) {
                    precise_add(a_val, b_val, precise_vars[var_idx]);
                } else if (strcmp(op, "-") == 0) {
                    precise_sub(a_val, b_val, precise_vars[var_idx]);
                } else if (strcmp(op, "*") == 0) {
                    precise_mul(a_val, b_val, precise_vars[var_idx]);
                } else if (strcmp(op, "/") == 0) {
                    precise_div(a_val, b_val, precise_vars[var_idx]);
                }
            }
            // 匹配 变量 = 数值
            else if (sscanf(line, "precise %s = %s", v, a_str) == 2) {
                if (is_digit_string(a_str)) {
                    int var_idx = find_var_index(v, var_names, var_count);
                    if (var_idx == -1) var_idx = var_count++;
                    strcpy(var_names[var_idx], v);
                    strcpy(precise_vars[var_idx], a_str);
                }
            }
        }

        // 解析return（支持return a,b,c格式）
        if (strstr(line, "return")) {
            // 清空之前的返回变量
            for (int i = 0; i < return_count; i++) {
                free(return_vars[i]);
                return_vars[i] = NULL;
            }
            return_count = 0;

            char return_part[256];
            if (sscanf(line, "return %255[^\n]", return_part) == 1) {
                return_count = split_comma_string(return_part, return_vars, 10);
            }
        }
    }

    // 拼接返回结果
    if (return_count == 0) return -1;
    result[0] = '\0';
    for (int i = 0; i < return_count; i++) {
        if (return_vars[i] == NULL || strlen(return_vars[i]) == 0) continue;
        
        int var_idx = find_var_index(return_vars[i], var_names, var_count);
        if (var_idx == -1) continue;
        
        char temp[BIG_INT_MAX_LEN] = {0};
        if (larnum_defined[var_idx]) {
            strcpy(temp, larnum_vars[var_idx]);
        } else {
            strcpy(temp, precise_vars[var_idx]);
        }
        
        if (strlen(result) > 0) strcat(result, "\n");
        if (strlen(result) + strlen(temp) < res_len) {
            strcat(result, temp);
        }

        free(return_vars[i]);
        return_vars[i] = NULL;
    }

    return 0;
}

// ========================
// 5. 代码生成与编译函数
// ========================
CLRL_API int clrl_generate_c_code(const char* clrl_file, const char* c_file) {
    FILE* in = fopen(clrl_file, "r");
    FILE* out = fopen(c_file, "w");
    if (!in || !out) return -1;

    fprintf(out, "#include <stdio.h>\n#include <stdlib.h>\n#include <math.h>\n#include <string.h>\n");
    fprintf(out, "typedef unsigned long long larnum;\n");
    fprintf(out, "int main(){\n");

    char var_defs[4096] = {0};
    char output_cmds[2048] = {0};
    char return_vars[10][32] = {0};
    int return_count = 0;

    char line[256];
    while (fgets(line, sizeof(line), in)) {
        if (line[0] == '#' || line[0] == '\n') continue;

        // 解析larnum
        if (strstr(line, "larnum")) {
            char v[32], a[64], op[4], b[64];
            if (sscanf(line, "larnum %s = %s %s %s", v, a, op, b) == 4) {
                snprintf(var_defs + strlen(var_defs), sizeof(var_defs) - strlen(var_defs) - 1,
                    "larnum %s = %s %s %s;\n", v, a, op, b);
            } else if (sscanf(line, "larnum %s = %s", v, a) == 2) {
                snprintf(var_defs + strlen(var_defs), sizeof(var_defs) - strlen(var_defs) - 1,
                    "larnum %s = %s;\n", v, a);
            }
        }

        // 解析precise
        if (strstr(line, "precise")) {
            char v[32], a[32], op[4], b[32];
            if (sscanf(line, "precise %s = %s %s %s", v, a, op, b) == 4) {
                snprintf(var_defs + strlen(var_defs), sizeof(var_defs) - strlen(var_defs) - 1,
                    "double %s = atof(\"%s\") %s atof(\"%s\");\n", v, a, op, b);
            } else if (sscanf(line, "precise %s = %s", v, a) == 2) {
                snprintf(var_defs + strlen(var_defs), sizeof(var_defs) - strlen(var_defs) - 1,
                    "double %s = atof(\"%s\");\n", v, a);
            }
        }

        // 解析return
        if (strstr(line, "return")) {
            char return_part[256];
            if (sscanf(line, "return %255[^\n]", return_part) == 1) {
                char* tokens[10] = {NULL};
                return_count = split_comma_string(return_part, tokens, 10);
                for (int i = 0; i < return_count; i++) {
                    strcpy(return_vars[i], tokens[i]);
                    free(tokens[i]);
                }
            }
        }
    }

    // 生成输出指令
    fprintf(out, "%s", var_defs);
    for (int i = 0; i < return_count; i++) {
        if (strstr(var_defs, return_vars[i]) && strstr(var_defs, "larnum")) {
            snprintf(output_cmds + strlen(output_cmds), sizeof(output_cmds) - strlen(output_cmds) - 1,
                "printf(\"%%llu\\n\", %s);\n", return_vars[i]);
        } else if (strstr(var_defs, return_vars[i])) {
            snprintf(output_cmds + strlen(output_cmds), sizeof(output_cmds) - strlen(output_cmds) - 1,
                "printf(\"%.10f\\n\", %s);\n", return_vars[i]);
        }
    }
    fprintf(out, "%s", output_cmds);
    fprintf(out, "return 0;\n}");

    fclose(in);
    fclose(out);
    return 0;
}

// 生成代码哈希
static void get_code_hash(const char* code, char* hash, size_t hash_len) {
    unsigned long long sum = 0;
    for (int i = 0; code[i]; i++) {
        sum = sum * 31 + code[i];
    }
    snprintf(hash, hash_len, "%llu", sum);
}

// 检查缓存
static int check_cache(const char* code, char* exe_path, size_t exe_len) {
    mkdir(CACHE_DIR);
    char hash[64];
    get_code_hash(code, hash, sizeof(hash));
    snprintf(exe_path, exe_len, "%s/%s.exe", CACHE_DIR, hash);
    struct stat st;
    return (stat(exe_path, &st) == 0) ? 1 : 0;
}

CLRL_API int clrl_compile_c_code(const char* clrl_code, const char* c_file, const char* exe_file) {
    // 检查缓存
    char cache_exe[PATH_BUF_SIZE];
    if (check_cache(clrl_code, cache_exe, sizeof(cache_exe))) {
#ifdef _WIN32
        CopyFileA(cache_exe, exe_file, FALSE);
#else
        char cmd[CMD_BUF_SIZE];
        snprintf(cmd, sizeof(cmd), "cp %s %s", cache_exe, exe_file);
        system(cmd);
#endif
        return 0;
    }

    // 编译C代码
    if (!strstr(c_file, ".c") || !strstr(exe_file, ".exe")) return -1;
    if (strcmp(c_file, exe_file) == 0) return -1;

    char cmd[CMD_BUF_SIZE];
#ifdef _WIN32
    snprintf(cmd, sizeof(cmd) - 1, "%s -O3 -s -w -ffast-math -march=native -o \"%s\" \"%s\"", COMPILER, exe_file, c_file);
#else
    snprintf(cmd, sizeof(cmd) - 1, "%s -O3 -s -w -ffast-math -march=native -o %s %s", COMPILER, exe_file, c_file);
#endif
    cmd[sizeof(cmd)-1] = '\0';
    printf("[Compile] %s\n", cmd);
    if (system(cmd) != 0) return -1;

    // 写入缓存
#ifdef _WIN32
    CopyFileA(exe_file, cache_exe, FALSE);
#else
    snprintf(cmd, sizeof(cmd), "cp %s %s", exe_file, cache_exe);
    system(cmd);
#endif
    return 0;
}

// ========================
// 6. 工具函数
// ========================
CLRL_API void clrl_get_temp_path(char* buf, size_t buf_size, const char* suffix) {
#ifdef _WIN32
    char temp_dir[256];
    GetTempPathA(sizeof(temp_dir), temp_dir);
    snprintf(buf, buf_size - 1, "%s%s%lu_%lu%s", 
        temp_dir, TEMP_PREFIX, (unsigned long)getpid(), (unsigned long)time(NULL), suffix);
#else
    snprintf(buf, buf_size - 1, "/tmp/%s%lu_%lu%s", 
        TEMP_PREFIX, (unsigned long)getpid(), (unsigned long)time(NULL), suffix);
#endif
    buf[buf_size-1] = '\0';
}

CLRL_API void clrl_ask_cleanup(const char* clrl, const char* c, const char* exe) {
    printf("\nDelete temp files? (y/n): ");
    char ch = getchar();
    getchar(); // 吃掉换行符
    if (ch == 'y' || ch == 'Y') {
        if (clrl) remove(clrl);
        if (c) remove(c);
        if (exe) remove(exe);
        printf("Cleaned.\n");
    }
}