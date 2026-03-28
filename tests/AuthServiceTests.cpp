#include "services/AuthService.h"
#include "repositories/EmployeeRepository.h"
#include "utils/PasswordHash.h"

#include <chrono>
#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <string>

namespace fs = std::filesystem;

class TempDirectory {
public:
    TempDirectory() : path_(fs::temp_directory_path() / ("cashier_auth_service_tests_" + std::to_string(std::chrono::steady_clock::now().time_since_epoch().count()))) {
        fs::create_directories(path_);
    }

    ~TempDirectory() {
        std::error_code errorCode;
        fs::remove_all(path_, errorCode);
    }

    const fs::path& path() const {
        return path_;
    }

private:
    fs::path path_;
};

void assertTrue(bool condition, const std::string& message) {
    if (!condition) {
        throw std::runtime_error(message);
    }
}

// 在临时目录准备包含已知员工的 employees.csv
std::string setupEmployeeFile(const fs::path& dir) {
    const auto filePath = dir / "employees.csv";
    EmployeeRepository repo(filePath.string());

    const std::string adminHash = utils::hashPassword("admin123");
    const std::string cashierHash = utils::hashPassword("cashier456");
    repo.save({
        {"E001", "管理员", adminHash, "admin"},
        {"E002", "收银员", cashierHash, "cashier"},
    });
    return filePath.string();
}

void testLoginSuccessWithCorrectCredentials() {
    TempDirectory tempDirectory;
    const auto filePath = setupEmployeeFile(tempDirectory.path());

    EmployeeRepository repo(filePath);
    AuthService auth(repo);
    assertTrue(!auth.isLoggedIn(), "Expected not logged in initially.");

    const bool result = auth.login("E001", "admin123");
    assertTrue(result, "Expected login to succeed with correct credentials.");
    assertTrue(auth.isLoggedIn(), "Expected isLoggedIn() to be true after successful login.");
    assertTrue(auth.getCurrentEmployee().id == "E001", "Expected current employee id to be E001.");
    assertTrue(auth.getCurrentEmployee().role == "admin", "Expected current employee role to be admin.");
    assertTrue(auth.getFailedAttempts() == 0, "Expected failed attempts to reset to 0 after successful login.");
}

void testLoginFailsWithWrongPassword() {
    TempDirectory tempDirectory;
    const auto filePath = setupEmployeeFile(tempDirectory.path());

    EmployeeRepository repo(filePath);
    AuthService auth(repo);
    const bool result = auth.login("E001", "wrongpassword");
    assertTrue(!result, "Expected login to fail with wrong password.");
    assertTrue(!auth.isLoggedIn(), "Expected not logged in after failed attempt.");
    assertTrue(auth.getFailedAttempts() == 1, "Expected failed attempts to be 1.");
}

void testLoginFailsWithWrongEmployeeId() {
    TempDirectory tempDirectory;
    const auto filePath = setupEmployeeFile(tempDirectory.path());

    EmployeeRepository repo(filePath);
    AuthService auth(repo);
    const bool result = auth.login("E999", "admin123");
    assertTrue(!result, "Expected login to fail with unknown employee id.");
    assertTrue(!auth.isLoggedIn(), "Expected not logged in after failed attempt.");
}

void testLocksAfterMaxAttempts() {
    TempDirectory tempDirectory;
    const auto filePath = setupEmployeeFile(tempDirectory.path());

    EmployeeRepository repo(filePath);
    AuthService auth(repo);
    for (int i = 0; i < AuthService::maxAttempts; ++i) {
        auth.login("E001", "bad");
    }
    assertTrue(auth.getFailedAttempts() == AuthService::maxAttempts, "Expected failed attempts to equal maxAttempts.");

    // 即使密码正确也应被拒绝
    const bool result = auth.login("E001", "admin123");
    assertTrue(!result, "Expected login to be rejected after exceeding max attempts.");
    assertTrue(!auth.isLoggedIn(), "Expected not logged in when locked.");
}

void testLogoutClearsSession() {
    TempDirectory tempDirectory;
    const auto filePath = setupEmployeeFile(tempDirectory.path());

    EmployeeRepository repo(filePath);
    AuthService auth(repo);
    auth.login("E002", "cashier456");
    assertTrue(auth.isLoggedIn(), "Expected logged in before logout.");

    auth.logout();
    assertTrue(!auth.isLoggedIn(), "Expected not logged in after logout.");
    assertTrue(auth.getFailedAttempts() == 0, "Expected failed attempts to reset after logout.");
}

void testLogoutResetsFailedAttempts() {
    TempDirectory tempDirectory;
    const auto filePath = setupEmployeeFile(tempDirectory.path());

    EmployeeRepository repo(filePath);
    AuthService auth(repo);
    auth.login("E001", "bad");
    auth.login("E001", "bad");
    assertTrue(auth.getFailedAttempts() == 2, "Expected two failed attempts.");

    auth.logout();
    assertTrue(auth.getFailedAttempts() == 0, "Expected failed attempts reset by logout.");

    // 退出后可重新登录
    const bool result = auth.login("E001", "admin123");
    assertTrue(result, "Expected login to succeed after logout resets failed attempts.");
}

int main() {
    try {
        testLoginSuccessWithCorrectCredentials();
        testLoginFailsWithWrongPassword();
        testLoginFailsWithWrongEmployeeId();
        testLocksAfterMaxAttempts();
        testLogoutClearsSession();
        testLogoutResetsFailedAttempts();
        std::cout << "All AuthService tests passed." << std::endl;
        return 0;
    } catch (const std::exception& exception) {
        std::cerr << exception.what() << std::endl;
        return 1;
    }
}
