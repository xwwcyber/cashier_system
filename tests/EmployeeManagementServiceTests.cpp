#include "services/EmployeeManagementService.h"

#include "repositories/EmployeeRepository.h"
#include "utils/PasswordHash.h"

#include <cassert>
#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <string>

namespace {
std::filesystem::path testDir() {
    return std::filesystem::temp_directory_path() / "employee_management_tests";
}

void cleanup() {
    std::filesystem::remove_all(testDir());
}

EmployeeManagementService makeService() {
    return EmployeeManagementService(
        EmployeeRepository((testDir() / "employees.csv").string()));
}
}

static void test_get_all_returns_initial_employees() {
    cleanup();
    auto service = makeService();
    auto employees = service.getAll();
    // 仓储缺失文件时自动生成示例数据（至少包含 1 个管理员和 1 个收银员）
    assert(employees.size() >= 2);
    bool hasAdmin = false, hasCashier = false;
    for (const auto& e : employees) {
        if (e.role == "admin")   hasAdmin   = true;
        if (e.role == "cashier") hasCashier = true;
    }
    assert(hasAdmin);
    assert(hasCashier);
    cleanup();
    std::cout << "[PASS] test_get_all_returns_initial_employees" << std::endl;
}

static void test_add_employee_success() {
    cleanup();
    auto service = makeService();
    service.addEmployee("E099", "测试员工", "cashier", "pass123");
    auto employees = service.getAll();
    bool found = false;
    for (const auto& e : employees) {
        if (e.id == "E099") {
            assert(e.name == "测试员工");
            assert(e.role == "cashier");
            assert(e.passwordHash == utils::hashPassword("pass123"));
            found = true;
        }
    }
    assert(found);
    cleanup();
    std::cout << "[PASS] test_add_employee_success" << std::endl;
}

static void test_add_employee_duplicate_id_throws() {
    cleanup();
    auto service = makeService();
    service.addEmployee("E099", "员工A", "cashier", "pass");
    bool threw = false;
    try {
        service.addEmployee("E099", "员工B", "admin", "pass");
    } catch (const std::invalid_argument&) {
        threw = true;
    }
    assert(threw);
    cleanup();
    std::cout << "[PASS] test_add_employee_duplicate_id_throws" << std::endl;
}

static void test_add_employee_invalid_role_throws() {
    cleanup();
    auto service = makeService();
    bool threw = false;
    try {
        service.addEmployee("E099", "员工A", "manager", "pass");
    } catch (const std::invalid_argument&) {
        threw = true;
    }
    assert(threw);
    cleanup();
    std::cout << "[PASS] test_add_employee_invalid_role_throws" << std::endl;
}

static void test_reset_password_success() {
    cleanup();
    auto service = makeService();
    service.addEmployee("E099", "测试员工", "cashier", "oldpass");
    service.resetPassword("E099", "newpass");
    auto employees = service.getAll();
    for (const auto& e : employees) {
        if (e.id == "E099") {
            assert(e.passwordHash == utils::hashPassword("newpass"));
        }
    }
    cleanup();
    std::cout << "[PASS] test_reset_password_success" << std::endl;
}

static void test_reset_password_nonexistent_throws() {
    cleanup();
    auto service = makeService();
    bool threw = false;
    try {
        service.resetPassword("NONEXISTENT", "newpass");
    } catch (const std::invalid_argument&) {
        threw = true;
    }
    assert(threw);
    cleanup();
    std::cout << "[PASS] test_reset_password_nonexistent_throws" << std::endl;
}

static void test_delete_employee_success() {
    cleanup();
    auto service = makeService();
    service.addEmployee("E099", "测试员工", "cashier", "pass");
    service.deleteEmployee("E099", "E001");  // 当前操作员为 E001
    auto employees = service.getAll();
    for (const auto& e : employees) {
        assert(e.id != "E099");
    }
    cleanup();
    std::cout << "[PASS] test_delete_employee_success" << std::endl;
}

static void test_delete_current_operator_throws() {
    cleanup();
    auto service = makeService();
    bool threw = false;
    try {
        service.deleteEmployee("E001", "E001");  // 试图删除自身
    } catch (const std::invalid_argument&) {
        threw = true;
    }
    assert(threw);
    cleanup();
    std::cout << "[PASS] test_delete_current_operator_throws" << std::endl;
}

static void test_delete_nonexistent_throws() {
    cleanup();
    auto service = makeService();
    bool threw = false;
    try {
        service.deleteEmployee("NONEXISTENT", "E001");
    } catch (const std::invalid_argument&) {
        threw = true;
    }
    assert(threw);
    cleanup();
    std::cout << "[PASS] test_delete_nonexistent_throws" << std::endl;
}

static void test_changes_persisted() {
    cleanup();
    std::filesystem::create_directories(testDir());
    std::string path = (testDir() / "employees.csv").string();

    {
        EmployeeRepository repo1(path);
        EmployeeManagementService service(repo1);
        service.addEmployee("E099", "持久化测试", "cashier", "pass");
    }
    // 重新加载
    EmployeeRepository repo2(path);
    EmployeeManagementService service2(repo2);
    auto employees = service2.getAll();
    bool found = false;
    for (const auto& e : employees) {
        if (e.id == "E099") found = true;
    }
    assert(found);
    cleanup();
    std::cout << "[PASS] test_changes_persisted" << std::endl;
}

int main() {
    test_get_all_returns_initial_employees();
    test_add_employee_success();
    test_add_employee_duplicate_id_throws();
    test_add_employee_invalid_role_throws();
    test_reset_password_success();
    test_reset_password_nonexistent_throws();
    test_delete_employee_success();
    test_delete_current_operator_throws();
    test_delete_nonexistent_throws();
    test_changes_persisted();
    std::cout << "All employee management service tests passed." << std::endl;
    return 0;
}
