#include <emscripten/bind.h>
#include <string>
#include <vector>
#include <regex>
#include <algorithm>
#include <cctype>
#include <sstream>

// 引入生成的数据文件
#include "rules_conversion.h"
// #include "rules_exceptions.h"
#include "r_exc.h"

using namespace emscripten;

const std::regex CLEANUP_REGEX(
    "[0-9•—–,?!\\r’°“”…\\u00a0«»\\\\/\\[\\]\\(\\)<>=+%$&#;:*{}\\'`\\-]"
);

std::vector<std::string> apion_process(const std::string& input_text) {
    std::vector<std::string> result_list;

    // 1. 预处理：转小写
    std::string text = input_text;
    std::transform(text.begin(), text.end(), text.begin(), 
                   [](unsigned char c){ return std::tolower(c); });

    // 2. 移除特殊字符 (对应 SPE.sub("", texte))
    // 使用 regex_replace 将所有匹配 SPE 的字符替换为空
    std::string cleaned = std::regex_replace(text, CLEANUP_REGEX, "");

    // 3. 分词 (按空格分割)
    std::istringstream iss(cleaned);
    std::string mot;
    
    // 预加载规则引用，避免重复查找
    const auto& rules = CONVERSION_RULES;
    const auto& exceptions = EXCEPTIONS;

    while (iss >> mot) {
        std::string current_word = mot;
        std::string result_buffer = "";
        bool is_exception = false;

        // 4. 检查例外词典 (线性查找)
        for (const auto& ex : exceptions) {
            if (ex.first == current_word) {
                result_buffer = ex.second;
                is_exception = true;
                break;
            }
        }

        if (!is_exception) {
            // 5. 常规转换循环 (对应 until mot.empty?)
            while (!current_word.empty()) {
                bool matched = false;

                // 遍历规则 (对应 rules.each)
                for (const auto& rule : rules) {
                    const std::string& pattern = rule.first;
                    const std::string& replacement = rule.second;

                    try {
                        // 编译正则 (规则本身已包含 ^)
                        std::regex re(pattern, std::regex_constants::ECMAScript);

                        // 检查是否匹配 (对应 mot =~ /#{regle}/)
                        if (std::regex_search(current_word, re)) {
                            // 匹配成功，执行替换并移除 (对应 mot.sub!)
                            // regex_replace 返回替换后的字符串，我们将匹配部分替换为 ""
                            std::string new_word = std::regex_replace(current_word, re, "", std::regex_constants::format_first_only);
                            
                            // 只有当字符串确实变短了（说明发生了替换），才视为成功
                            if (new_word.length() < current_word.length()) {
                                current_word = new_word;
                                result_buffer += replacement;
                                matched = true;
                                break; // 找到第一个匹配，跳出规则循环，重新开始 while(!current_word.empty())
                            }
                        }
                    } catch (const std::regex_error& e) {
                        // 忽略无效正则，继续下一条
                        continue;
                    }
                }

                // 如果遍历完所有规则都没有匹配，说明无法继续转换
                // 对应原逻辑中如果没有匹配会报错或卡死，这里我们 break 防止死循环
                if (!matched) {
                    break; 
                }
            }
        }
        
        result_list.push_back(result_buffer);
    }

    return result_list;
}

// 绑定到 JS
// 包装函数，将 vector<string> 转换为 JS 数组
val apion_process_js(const std::string& input_text) {
    auto result = apion_process(input_text);
    val js_array = val::array();
    for (const auto& str : result) {
        js_array.call<void>("push", str);
    }
    return js_array;
}

EMSCRIPTEN_BINDINGS(apion_module) {
    function("apion_process", &apion_process_js);
}