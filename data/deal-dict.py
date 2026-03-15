# 处理 dict.json

import json
import struct

def generate_binary_file(json_path, output_path):
    # 1. 读取 JSON
    with open(json_path, 'r', encoding='utf-8') as f:
        data = json.load(f)
    
    items = list(data.items())
    print(f"正在处理 {len(items)} 条数据...")

    # 2. 写入二进制文件
    with open(output_path, 'wb') as f:
        # 写入总条目数 (使用 4 字节无符号整数)
        f.write(struct.pack('<I', len(items)))
        
        for key, value in items:
            # 编码为 UTF-8 字节
            key_bytes = key.encode('utf-8')
            val_bytes = value.encode('utf-8')
            
            key_len = len(key_bytes)
            val_len = len(val_bytes)
            
            # 检查长度是否超过 2 字节 (65535)，通常单词不会这么长
            if key_len > 65535 or val_len > 65535:
                raise ValueError(f"字符串过长: {key}")
            
            # 写入：[KeyLen(2)] [Key] [ValLen(2)] [Val]
            # '<H' 表示小端序 2 字节无符号整数
            f.write(struct.pack('<H', key_len))
            f.write(key_bytes)
            
            f.write(struct.pack('<H', val_len))
            f.write(val_bytes)

    print(f"成功生成二进制文件: {output_path}")
    print(f"文件大小: {os.path.getsize(output_path) / 1024:.2f} KB")

if __name__ == "__main__":
    import os
    generate_binary_file('dict.json', 'exceptions.bin')
