#pragma once

#include <string>
#include <vector>

#include "models/Employee.h"
#include "repositories/EmployeeRepository.h"

/**
 * 员工管理服务，支持新增、重置密码和删除员工。
 * 所有变更操作后立即持久化到 CSV 文件。
 */
class EmployeeManagementService {
public:
    explicit EmployeeManagementService(EmployeeRepository repository);

    /** 返回所有员工列表。 */
    std::vector<Employee> getAll() const;

    /**
     * 新增员工。
     * @throws std::invalid_argument 若 id 已存在、字段为空，或 role 不是 "admin"/"cashier"。
     */
    void addEmployee(const std::string& id, const std::string& name,
                     const std::string& role, const std::string& password);

    /**
     * 重置指定员工密码。
     * @throws std::invalid_argument 若员工不存在或新密码为空。
     */
    void resetPassword(const std::string& id, const std::string& newPassword);

    /**
     * 删除指定员工。
     * @throws std::invalid_argument 若员工不存在，或试图删除当前操作员自身。
     */
    void deleteEmployee(const std::string& id, const std::string& currentOperatorId);

private:
    EmployeeRepository repository_;
};
