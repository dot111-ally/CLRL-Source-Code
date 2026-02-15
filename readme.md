# CLRL - Calculator Language
A lightweight, high-performance calculator language interpreter with support for arbitrary-precision integer arithmetic and decimal arithmetic without floating-point errors. Built with C (MinGW) and CMake, designed for cross-platform compatibility (Windows/Linux/macOS).

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Platform](https://img.shields.io/badge/Platform-Windows%20%7C%20Linux%20%7C%20macOS-blue)](https://github.com/dot111-ally/CLRL-Source-Code)
[![Language](https://img.shields.io/badge/Language-C99-green)](https://github.com/dot111-ally/CLRL-Source-Code)

[简体中文](https://github.com/dot111-ally/CLRL-Source-Code/blob/main/doc/zh-cn.md) | English

## 📋 Table of Contents
- [Features](#features)
- [Getting Started](#getting-started)
  - [Prerequisites](#prerequisites)
  - [Build from Source](#build-from-source)
  - [Usage](#usage)
- [Language Syntax](#language-syntax)
  - [Arbitrary-Precision Integers (larnum)](#arbitrary-precision-integers-larnum)
  - [Precise Decimals (precise)](#precise-decimals-precise)
  - [Return Statement](#return-statement)
  - [System Commands](#system-commands)
- [Project Structure](#project-structure)
- [License](#license)
- [Contributing](#contributing)

## Features
- **Arbitrary-Precision Integers**: Perform addition, subtraction, multiplication, and division on extremely large integers without overflow
- **Precise Decimal Arithmetic**: Calculate decimals without floating-point errors (e.g., `0.1 + 0.2 = 0.3` exactly)
- **Two Execution Modes**:
  - REPL (Interactive Mode): Real-time calculation with instant feedback
  - Compiled Mode: Generate optimized C code and compile for maximum performance
- **Cross-Platform**: Works on Windows (MinGW), Linux, and macOS
- **Lightweight**: No external dependencies (statically linked)
- **Multiple Return Values**: Return and display multiple variables in one command
- **Cache System**: Reuse compiled binaries for repeated calculations

## Getting Started

### Prerequisites
- **Compiler**: GCC/MinGW (C99 compatible)
- **Build System**: CMake 3.18+
- **Platform**: Windows (64-bit), Linux, or macOS

### Build from Source
#### 1. Clone the Repository
```bash
git clone https://github.com/dot111-ally/CLRL-Source-Code.git
cd clrl
```
#### 2. Build with "Build.bat" (Windows)
```bash
build
```
#### 2.1 Build in Linux and MacOS (Not yet)
### 2. Usage
Only applicable to mathematical calculations; other functions will be gradually added if possible. The next version will add a Python recognition library to read the results of the compiled exe.
#### Language Syntax
In CLRL, you can use this code to create variable:
```clrl
[type] [name] = [value]
```
Now, we will introduce the two existing types.
### Arbitrary Precision Integers Larnum
This type can handle extremely, extremely, extremely large numbers without any calculation errors. However, the next version will add support for adding extremely large numbers with floating-point numbers.
### Precise Decimals Precise
This type is decimal, but adding them won't cause the problem of 0.1 + 0.2 = 0.30000000000000004!
### System Commands
(These commands are only for REPL mode)
- **Compile**: system:compile, only for testing in the REPL, the executable file cannot be found under normal circumstances.
- **Explain**: system:fast, Fast output, using an interpretive method, not compiled
- **Clear**: system:clear
- **Exit**: system:exit
- **Help**: system:help
## Project Structure
```text/plain
CLRL
├── doc
│   └── zh-cn.md
├── include
│   └── clrl_runtime.h
├── src
│   ├── cli
│   ├── runtime
│   ├── version_dll.rc
│   └── version.rc
├── Build.bat
├── CMakeLists.txt
├── demo.clrl
├── icon.ico
├── LICENSE
└── readme.md
```
## License
This project is licensed under the MIT License - see the LICENSE file for details.
Copyright (c) 2026 ツキミレンイトグチ
Permission is hereby granted, free of charge, to any person obtaining a copyof this software and associated documentation files (the "Software"), to dealin the Software without restriction, including without limitation the rightsto use, copy, modify, merge, publish, distribute, sublicense, and/or sellcopies of the Software, and to permit persons to whom the Software isfurnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in allcopies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS ORIMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THEAUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHERLIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THESOFTWARE.
## Contributing
This project is independently developed and maintained by ツキミレンイトグチ (all code/icons are self-created). 
Contributions for bug fixes, documentation improvements, or feature suggestions are welcome. Please fork the repo and submit a Pull Request, or open an Issue to discuss your ideas.