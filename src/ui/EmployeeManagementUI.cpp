#include "ui/EmployeeManagementUI.h"

#include <iomanip>
#include <iostream>
#include <string>
#include <utility>

EmployeeManagementUI::EmployeeManagementUI(EmployeeManagementService service,
                                           std::string currentOperatorId)
    : service_(std::move(service)),
      currentOperatorId_(std::move(currentOperatorId)) {}

void EmployeeManagementUI::handleListEmployees() {
    auto employees = service_.getAll();
    std::cout << "\n";
    std::cout << std::left
              << std::setw(10) << "编号"
              << std::setw(14) << "姓名"
              << "角色" << std::endl;
    std::cout << std::string(36, '-') << std::endl;
    for (const auto& e : employees) {
        std::cout << std::left
                  << std::setw(10) << e.id
                  << std::setw(14) << e.name
                  << e.role << std::endl;
    }
}

void EmployeeManagementUI::handleAddEmployee() {
    std::cout << "请输入员工编号：";
    std::string id;
    std::getline(std::cin, id);

    std::cout << "请输入员工姓名：";
    std::string name;
    std::getline(std::cin, name);

    std::cout << "请输入角色（admin/cashier）：";
    std::string role;
    std::getline(std::cin, role);

    std::cout << "请输入初始密码：";
    std::string password;
    std::getline(std::cin, password);

    try {
        service_.addEmployee(id, name, role, password);
        std::cout << "员工添加成功。" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "添加失败：" << e.what() << std::endl;
    }
}

void EmployeeManagementUI::handleResetPassword() {
    std::cout << "请输入员工编号：";
    std::string id;
    std::getline(std::cin, id);

    std::cout << "请输入新密码：";
    std::string newPassword;
    std::getline(std::cin, newPassword);

    try {
        service_.resetPassword(id, newPassword);
        std::cout << "密码重置成功。" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "重置失败：" << e.what() << std::endl;
    }
}

void EmployeeManagementUI::handleDeleteEmployee() {
    std::cout << "请输入要删除的员工编号：";
    std::string id;
    std::getline(std::cin, id);

    try {
        service_.deleteEmployee(id, currentOperatorId_);
        std::cout << "员工删除成功。" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "删除失败：" << e.what() << std::endl;
    }
}

void EmployeeManagementUI::run() {
    while (true) {
        std::cout << "\n============================" << std::endl;
        std::cout << "        员工管理" << std::endl;
        std::cout << "============================" << std::endl;
        std::cout << "  1. 查看员工列表" << std::endl;
        std::cout << "  2. 新增员工" << std::endl;
        std::cout << "  3. 重置密码" << std::endl;
        std::cout << "  4. 删除员工" << std::endl;
        std::cout << "  0. 返回上级菜单" << std::endl;
        std::cout << "============================" << std::endl;
        std::cout << "请输入菜单编号：";

        std::string line;
        if (!std::getline(std::cin, line)) break;
        int choice = -1;
        try { choice = std::stoi(line); } catch (...) {}

        switch (choice) {
            case 1: handleListEmployees();  break;
            case 2: handleAddEmployee();    break;
            case 3: handleResetPassword();  break;
            case 4: handleDeleteEmployee(); break;
            case 0: return;
            default: std::cout << "无效输入，请重新选择。" << std::endl; break;
        }
    }
}
