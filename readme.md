# CLRL - Calculator Language
A lightweight, high-performance calculator language interpreter with support for arbitrary-precision integer arithmetic and decimal arithmetic without floating-point errors. Built with C (MinGW) and CMake, designed for cross-platform compatibility (Windows/Linux/macOS).

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Platform](https://img.shields.io/badge/Platform-Windows%20%7C%20Linux%20%7C%20macOS-blue)](https://github.com/your-username/clrl)
[![Language](https://img.shields.io/badge/Language-C99-green)](https://github.com/your-username/clrl)

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

## ✨ Features
- **Arbitrary-Precision Integers**: Perform addition, subtraction, multiplication, and division on extremely large integers without overflow
- **Precise Decimal Arithmetic**: Calculate decimals without floating-point errors (e.g., `0.1 + 0.2 = 0.3` exactly)
- **Two Execution Modes**:
  - REPL (Interactive Mode): Real-time calculation with instant feedback
  - Compiled Mode: Generate optimized C code and compile for maximum performance
- **Cross-Platform**: Works on Windows (MinGW), Linux, and macOS
- **Lightweight**: No external dependencies (statically linked)
- **Multiple Return Values**: Return and display multiple variables in one command
- **Cache System**: Reuse compiled binaries for repeated calculations

## 🚀 Getting Started

### Prerequisites
- **Compiler**: GCC/MinGW (C99 compatible)
- **Build System**: CMake 3.18+
- **Platform**: Windows (64-bit), Linux, or macOS

### Build from Source
#### 1. Clone the Repository
```bash
git clone https://github.com/your-username/clrl.git
cd clrl