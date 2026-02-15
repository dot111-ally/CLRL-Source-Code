# CLRL - Calculator Language
一个轻量级、高性能的计算器语言解释器，支持任意精度整数运算和无浮点误差的小数运算。使用 C（MinGW）和 CMake 构建，旨在实现跨平台兼容（Windows/Linux/macOS）。

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Platform](https://img.shields.io/badge/Platform-Windows%20%7C%20Linux%20%7C%20macOS-blue)](https://github.com/dot111-ally/CLRL-Source-Code)
[![Language](https://img.shields.io/badge/Language-C99-green)](https://github.com/dot111-ally/CLRL-Source-Code)

简体中文 | [English](https://github.com/dot111-ally/CLRL-Source-Code/blob/main/README.md)
## 目录
- [功能](#功能)
- [入门](#入门)
  - [编译前条件](#编译前条件)
  - [从源代码构建](#从源代码构建)
  - [用处](#用处)
- [语法](#语法)
  - [任意精度整数 Larnum](#任意精度整数-larnum)
  - [精确小数 Precise](#精确小数-precise)
  - [返回语句](#返回语句)
  - [系统命令](#系统命令)
- [项目结构](#项目结构)
- [许可](#许可)
- [贡献](#贡献)

## 功能
- **任意精度整数**：对极大整数进行加、减、乘、除运算，不会溢出
- **精确小数运算**：计算小数时无浮点误差（例如，`0.1 + 0.2 = 0.3` 精确无误）
- **两种执行模式**：
  - REPL（交互模式）：实时计算并即时反馈
  - 编译模式：生成优化的 C 代码并编译以获得最大性能
- **跨平台**：支持 Windows（MinGW）、Linux 和 macOS
- **轻量级**：无外部依赖（静态链接）
- **多重返回值**：一次命令返回并显示多个变量
- **缓存系统**：复用已编译的二进制文件以进行重复计算

## 入门

### 编译前条件
- **编译器**：GCC/MinGW（兼容 C99）
- **构建系统**：CMake 3.18
- **平台**：Windows（64位）、Linux 或 macOS

### 从源码构建
#### 1. 克隆仓库
```bash
git clone https://github.com/dot111-ally/CLRL-Source-Code.git
cd clrl
```
#### 2. 使用“Build.bat”构建（Windows）
```bash
build
```
#### 2.1 在 Linux 和 MacOS 上构建（尚未支持）

### 用处
仅适用于数学计算；其他功能将会在可能的情况下逐步添加。下一版本将添加一个 Python 识别库，用于读取已编译 exe 的结果。

## 语言语法
在 CLRL 中，你可以使用以下代码来创建变量：
```clrl
[类型] [名称] = [值]
```
现在，我们将介绍现有的两种类型。
### 任意精度整数 Larnum
这种类型可以处理极其、极其、极其大的数字而不会产生计算错误。然而，下一版本将添加对极大数与浮点数相加的支持。
### 精确小数 Precise
这种类型是小数类型，但相加时不会出现 0.1 + 0.2 = 0.30000000000000004 的问题！
### 系统命令
（这些命令仅适用于 REPL 模式）
- **编译**: system:compile，仅在 REPL 中测试，通常情况下找不到可执行文件。
- **解释**: system:fast，快速输出，使用解释执行方式，而非编译。
- **清除**: system:clear
- **退出**: system:exit
- **帮助**: system:help

## 项目结构
```text/plain
CLRL
├── doc
│   └── zh-cn.md
├── include
│   └── clrl_runtime.h
├── src
│   ├── cli
│   │   └── clrlc.c
│   ├── runtime
│   │   └── clrl_runtime.c
│   ├── version_dll.rc
│   └── version.rc
├── Build.bat
├── CMakeLists.txt
├── demo.clrl
├── icon.ico
├── LICENSE
└── readme.md
```
## 许可证
This project is licensed under the MIT License - see the LICENSE file for details.
Copyright (c) 2026 ツキミレンイトグチ
Permission is hereby granted, free of charge, to any person obtaining a copyof this software and associated documentation files (the "Software"), to dealin the Software without restriction, including without limitation the rightsto use, copy, modify, merge, publish, distribute, sublicense, and/or sellcopies of the Software, and to permit persons to whom the Software isfurnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in allcopies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS ORIMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THEAUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHERLIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THESOFTWARE.
## 贡献
该项目由ツキミレンイトグチ独立开发和维护（所有代码/图标均为自创）。
欢迎为修复漏洞、改进文档或提出功能建议做出贡献。请先 fork 本仓库并提交 Pull Request，或提交 Issue 讨论您的想法。