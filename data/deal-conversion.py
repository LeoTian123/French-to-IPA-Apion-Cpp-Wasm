# 处理 conversion.csv

rules = []
with open('conversion.csv', 'r', encoding='utf-8') as f:
    for line in f:
        if '#' in line:
            k, v = line.strip().split('#', 1)
            rules.append((k, v))

with open('conversionRules.h', 'w', encoding='utf-8') as f:
    f.write("#pragma once\n#include <vector>\n#include <string>\n")
    f.write("const std::vector<std::pair<const char*, const char*>> CONVERSION_RULES = {\n")
    for k, v in rules:
        # 简单转义
        k_esc = k.replace('\\', '\\\\').replace('"', '\\"')
        v_esc = v.replace('\\', '\\\\').replace('"', '\\"')
        f.write(f'    {{ "{k_esc}", "{v_esc}" }},\n')
    f.write("};\n")
