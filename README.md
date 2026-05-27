# 💻 myshell — A High-Performance Unix Shell in C++

[![C++](https://img.shields.io/badge/Language-C%2B%2B17-blue.svg)](https://isocpp.org/)
[![Platform](https://img.shields.io/badge/Platform-Linux%20%7C%20WSL-orange.svg)](https://www.kernel.org/)
[![License](https://img.shields.io/badge/License-MIT-green.svg)](https://opensource.org/licenses/MIT)

A robust, custom POSIX-compliant Unix shell engineered from scratch in C++. This project demonstrates low-level systems programming proficiency, focusing on process lifecycle control, manual file descriptor manipulation, and inter-process communication (IPC).

---

## 🚀 Core Architectural Pipeline

1. **REPL Engine:** Continuous Read-Evaluate-Print Loop tracking user inputs.
2. **Tokenizer:** `std::stringstream` scanner parsing commands into dynamic token streams.
3. **Built-in Router:** Evaluates state-changing context commands execution prior to system branching.
4. **Subprocess Spawning:** Parallel process cloning utilizing low-level kernel routines.

---

## ✨ Features Implemented

* **System Command Interfacing:** Seamlessly delegates execution to standard binaries (`ls`, `cat`, `grep`, `top`).
* **Inter-Process Communication (Piping):** Implements `|` streams to redirect data directly through memory buffers between independent child processes.
* **I/O Redirection Engine:** Overrides default standard streams using `<` (input) and `>` (output truncation) operators.
* **Asynchronous Background Execution:** Appending `&` handles detached tasks natively without stalling main program evaluation.
* **Stateful History Log:** Maintains a ring-buffer sequence tracking the last 100 entries.

---

## 🛠️ Build & Installation

An automated `Makefile` compilation workflow is provided.

```bash
# Clone the repository
git clone [https://github.com/Anushka-Sharma-tech/myshell.git](https://github.com/Anushka-Sharma-tech/myshell.git)
cd myshell

# Compile utilizing the Makefile
make

# Launch the interactive terminal environment
./myshell
