#pragma once

#include <string>

/**
 * 员工数据模型，描述系统用户的基础信息与角色。
 */
struct Employee {
    std::string id;
    std::string name;
    std::string passwordHash;
    std::string role;  // "admin" 或 "cashier"
};
