#include "services/EmployeeManagementService.h"

#include "utils/PasswordHash.h"

#include <algorithm>
#include <stdexcept>
#include <utility>

EmployeeManagementService::EmployeeManagementService(EmployeeRepository repository)
    : repository_(std::move(repository)) {}

std::vector<Employee> EmployeeManagementService::getAll() const {
    return repository_.load();
}

void EmployeeManagementService::addEmployee(const std::string& id,
                                             const std::string& name,
                                             const std::string& role,
                                             const std::string& password) {
    if (id.empty() || name.empty() || password.empty()) {
        throw std::invalid_argument("员工编号、姓名和密码不能为空。");
    }
    if (role != "admin" && role != "cashier") {
        throw std::invalid_argument("角色必须为 admin 或 cashier。");
    }

    auto employees = repository_.load();
    for (const auto& e : employees) {
        if (e.id == id) {
            throw std::invalid_argument("员工编号 " + id + " 已存在。");
        }
    }

    Employee emp;
    emp.id           = id;
    emp.name         = name;
    emp.role         = role;
    emp.passwordHash = utils::hashPassword(password);
    employees.push_back(emp);
    repository_.save(employees);
}

void EmployeeManagementService::resetPassword(const std::string& id,
                                               const std::string& newPassword) {
    if (newPassword.empty()) {
        throw std::invalid_argument("新密码不能为空。");
    }

    auto employees = repository_.load();
    bool found = false;
    for (auto& e : employees) {
        if (e.id == id) {
            e.passwordHash = utils::hashPassword(newPassword);
            found = true;
            break;
        }
    }
    if (!found) {
        throw std::invalid_argument("员工编号 " + id + " 不存在。");
    }
    repository_.save(employees);
}

void EmployeeManagementService::deleteEmployee(const std::string& id,
                                                const std::string& currentOperatorId) {
    if (id == currentOperatorId) {
        throw std::invalid_argument("不能删除当前登录的管理员账号。");
    }

    auto employees = repository_.load();
    auto it = std::remove_if(employees.begin(), employees.end(),
                             [&](const Employee& e) { return e.id == id; });
    if (it == employees.end()) {
        throw std::invalid_argument("员工编号 " + id + " 不存在。");
    }
    employees.erase(it, employees.end());
    repository_.save(employees);
}
