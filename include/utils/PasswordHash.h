#pragma once

#include <cstdint>
#include <iomanip>
#include <sstream>
#include <string>

namespace utils {

/**
 * 使用 FNV-1a 算法对密码进行哈希，返回 16 位十六进制字符串。
 * 该算法跨平台确定性输出，适用于文件持久化场景。
 */
inline std::string hashPassword(const std::string& password) {
    uint64_t hash = 14695981039346656037ULL;
    for (unsigned char c : password) {
        hash ^= c;
        hash *= 1099511628211ULL;
    }
    std::ostringstream oss;
    oss << std::hex << std::setw(16) << std::setfill('0') << hash;
    return oss.str();
}

}  // namespace utils
