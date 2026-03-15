# French to IPA: Apion-Cpp-Wasm

## English

### Intro

This project can be regarded as a **.cpp to .wasm implementation** of [Apion](https://github.com/coontail/Apion), used to convert French text into IPA phonetic transcription.

It is written in C++ and compiled into WebAssembly using Emscripten so that it can be conveniently invoked and executed in a web browser.

The project was developed on a Windows system; some modifications may be required to run it on Linux.

The core logic performs lookups in a conversion dictionary. If a lookup fails, a set of regular-expression rules is used to infer the corresponding IPA transcription.

A number of basic optimizations have already been implemented in the code. For example, the rules are compiled only once and stored in advance. Further optimizations such as bucketing or building automata are possible, but they do not seem necessary.

### Usage

1. Ensure that Emscripten is installed (this typically also requires Python).

2. The data portion has already been processed and requires no additional steps.
   For reference, the data processing workflow is as follows:
   The data comes from the original Apion project: a dictionary `data/dict.json` and a set of regex rules `data/conversion.csv`.
   Run:
   ```powershell
   python deal-conversion.py
   ```
   to process `data/conversion.csv` and generate `data/conversionRules.h`.
   Then run:
   ```powershell
   python deal-dict.py
   ```
   to process `data/dict.json` and generate `data/exceptions.bin`.
   Copy `data/conversionRules.h` and `data/exceptions.bin` to the project root directory.

3. Run `asmb.bat` to compile the project.
   This will generate three files under `web/`:
   * `apion_core.js`
   * `apion_core.wasm`
   * `apion_core.data`

4. Create an HTML page.
   An example showing how to include `apion_core.js` is provided in `web/least_index.html`.

5. Run a local server, for example using Python’s `http.server` module:
   ```bash
   python -m http.server 8080
   ```
   Then open `http://localhost:8080` in a browser to see the result.

--------------

## 简中

### 介绍

本项目可以看做是 [Apion](https://github.com/coontail/Apion) 的一个 .cpp to .wasm 实现，用于将法语文本转换为IPA音标。它使用C++编写，使用Emscripten编译为WebAssembly，以便在浏览器方便地调用运行。

本项目开发时运行在Windows系统下，Linux系统应该进行修改。

核心逻辑是在一个转换字典中进行查找，如果查找失败，使用一套正则规则推理出IPA。

这个代码基本的优化做了不少，例如规则只编译一次，且提前存储这些规则。进一步的优化就是分桶、上自动机，感觉没有必要了。

### 使用方法

1. 确保已安装Emscripten，这通常还需要安装Python。
2. 数据部分已经处理好，无需额外操作。附上其处理过程如下：
    数据来源是原项目Apionx的一个字典`data/dict.json`和一套正则规则`data/conversion.csv`，运行
    ```powershell
    python deal-conversion.py
    ```
    处理`data/conversion.csv`得到`data/conversionRules.h`。运行
    ```powershell
    python deal-dict.py
    ```
    处理`data/dict.json`得到`data/exceptions.bin`。
    将`data/conversionRules.h`和`data/exceptions.bin`复制到根目录。
3. 运行`asmb.bat`进行编译，得到`web/`下的`apion_core.js, apion_core.wasm/ apion_core.data`三个文件。
4. 编写一个 .html 网页，其引入`apion_core.js`的方法已经示例在`web/least_index.html`中。
5. 运行一个服务器，例如使用 Python 的 `http.server` 模块：
    ```bash
    python -m http.server 8080
    ```
    然后在浏览器中打开 `http://localhost:8080`，即可查看效果。


