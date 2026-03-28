#pragma once

#include <string>
#include <vector>

#include "models/Employee.h"

/**
 * 员工文件仓储，负责从 CSV 文件加载和保存员工数据。
 * 文件中仅存储密码哈希值，不保存明文密码。
 */
class EmployeeRepository {
public:
    /**
     * 使用给定文件路径创建员工仓储。
     */
    explicit EmployeeRepository(std::string filePath = "data/employees.csv");

    /**
     * 加载员工列表；当文件不存在时自动生成示例数据并落盘。
     */
    std::vector<Employee> load() const;

    /**
     * 将员工列表完整写回到 CSV 文件。
     */
    void save(const std::vector<Employee>& employees) const;

private:
    std::string filePath_;

    std::vector<Employee> buildSampleEmployees() const;
};
