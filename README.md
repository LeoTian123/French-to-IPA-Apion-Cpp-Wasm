# France to IPA: Apion-Cpp-Wasm

本项目可以看做是(Apion)[https://github.com/coontail/Apion]的一个 .cpp to .wasm 实现，用于将法语文本转换为IPA音标。它使用C++编写，使用Emscripten编译为WebAssembly，以便在浏览器方便地调用运行。

本项目开发时运行在Windows系统下，Linux系统应该进行修改。

核心逻辑是在一个转换字典中进行查找，如果查找失败，使用一套正则规则推理出IPA。

使用方法：

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
4. 编写一个 .html 网页，其引入`apion_core.js`的方法已经示例在`web/index.html`中。
5. 运行一个服务器，例如使用 Python 的 `http.server` 模块：
    ```bash
    python -m http.server 8080
    ```
    然后在浏览器中打开 `http://localhost:8080`，即可查看效果。


