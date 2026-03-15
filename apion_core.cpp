#include <emscripten/bind.h>
#include <string>
#include <vector>
#include <regex>
#include <algorithm>
#include <cctype>
#include <sstream>

// 引入生成的数据文件
#include "conversionRules.h"
#include "exceptionLoader.h"

using namespace emscripten;

const std::regex CLEANUP_REGEX(
    "[0-9•—–,?!\\r’°“”…«»\\\\/\\[\\]\\(\\)<>=+%$&#;:*{}\\'`\\-]",
    std::regex_constants::optimize
);

// 定义一个结构体来持有编译后的正则和对应的替换字符串
struct CompiledRule {
    std::regex pattern;
    std::string replacement;
    
    // 构造函数：使用移动语义，避免复制昂贵的 regex 对象
    CompiledRule(std::regex&& p, std::string r) 
        : pattern(std::move(p)), replacement(std::move(r)) {}
};

// 全局静态变量：存储所有预编译的规则
static std::vector<CompiledRule> g_compiled_rules;
static bool g_rules_initialized = false;

// 初始化函数：只在第一次调用时执行，编译所有规则
void init_compiled_rules() {
    if (g_rules_initialized) return;

    const auto& raw_rules = CONVERSION_RULES;
    g_compiled_rules.reserve(raw_rules.size());

    for (const auto& rule : raw_rules) {
        const std::string& pattern_str = rule.first;
        const std::string& replacement = rule.second;
        
        try {
            // 一次性编译正则，而不是在循环里，添加 optimize 标志提示编译器优化
            std::regex re(pattern_str, std::regex_constants::ECMAScript | std::regex_constants::optimize);
            g_compiled_rules.emplace_back(std::move(re), replacement);
        } catch (const std::regex_error& e) {
            std::cerr << "Regex compile error for pattern: " << pattern_str << std::endl;
        }
    }
    g_rules_initialized = true;
}


std::vector<std::string> apion_process(const std::string& input_text) {

    // 搞定 g_rules_initialized
    init_compiled_rules();
    // 搞定 g_exception_map，不过其实没用到，有接口
    init_exceptions("exceptions.bin");

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

    // 返回结果，假设日常用语每个单词平均4~5个字母
    std::vector<std::string> result_list;
    result_list.reserve(input_text.size() / 5);

    // 复用 match 对象
    std::smatch match_result;
    
    while (iss >> mot) {
        std::string current_word = mot;
        std::string result_buffer = "";

        // 4. 检查例外词典 
        result_buffer = get_exception(current_word);
        
        // 5. 常规转换循环 (对应 until mot.empty?)
        if (result_buffer.empty()) {
            while (!current_word.empty()) {
                bool matched = false;

                for (const auto& compiled_rule : g_compiled_rules) {
                    // 使用 regex_search 查找匹配
                    if (std::regex_search(current_word, match_result, compiled_rule.pattern)) {
                        
                        // 手动截断字符串
                        size_t match_len = match_result.length(0);
                        
                        if (match_len >= current_word.length()) {
                            current_word = ""; // 匹配了整个单词
                        } else {
                            // 直接截取剩余部分，避免创建临时字符串的开销
                            current_word = current_word.substr(match_len);
                        }

                        result_buffer += compiled_rule.replacement;
                        matched = true;
                        break; // 找到第一个匹配，跳出规则循环，继续处理剩余单词
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