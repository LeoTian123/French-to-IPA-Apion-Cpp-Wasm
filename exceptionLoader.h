#pragma once
#include <string>
#include <unordered_map>
#include <fstream>
#include <vector>
#include <cstdint>
#include <iostream>

class ExceptionMap {
public:
    std::unordered_map<std::string, std::string> data;
    bool loaded = false;

    // 加载函数：传入二进制文件路径
    bool load(const std::string& filepath) {
        std::ifstream file(filepath, std::ios::binary);
        if (!file.is_open()) {
            std::cerr << "无法打开文件: " << filepath << std::endl;
            return false;
        }

        // 1. 读取总条目数
        uint32_t count = 0;
        file.read(reinterpret_cast<char*>(&count), sizeof(count));
        if (!file) {
            std::cerr << "读取条目数失败" << std::endl;
            return false;
        }

        // 预分配内存，减少 rehash 开销
        data.reserve(count);

        // 2. 循环读取每个条目
        for (uint32_t i = 0; i < count; ++i) {
            // 读取 Key 长度
            uint16_t key_len = 0;
            file.read(reinterpret_cast<char*>(&key_len), sizeof(key_len));
            
            // 读取 Key 内容
            std::string key(key_len, '\0');
            file.read(&key[0], key_len);

            // 读取 Value 长度
            uint16_t val_len = 0;
            file.read(reinterpret_cast<char*>(&val_len), sizeof(val_len));

            // 读取 Value 内容
            std::string val(val_len, '\0');
            file.read(&val[0], val_len);

            if (!file) {
                std::cerr << "读取数据中途失败，索引: " << i << std::endl;
                return false;
            }

            data[key] = val;
        }

        loaded = true;
        std::cout << "成功加载 " << count << " 条例外规则到 unordered_map." << std::endl;
        return true;
    }

    // 查找函数
    std::string find(const std::string& key) const {
        if (!loaded) return "";
        auto it = data.find(key);
        if (it != data.end()) {
            return it->second;
        }
        return "";
    }
};

// 全局单例实例 (方便调用)
static ExceptionMap g_exception_map;

// 初始化辅助函数 (在 main 或 wasm 入口调用一次)
inline void init_exceptions(const std::string& bin_path = "exceptions.bin") {
    if (!g_exception_map.loaded) {
        g_exception_map.load(bin_path);
    }
}

// 快速查找接口
inline std::string get_exception(const std::string& word) {
    return g_exception_map.find(word);
}