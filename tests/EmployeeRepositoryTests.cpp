#include "repositories/EmployeeRepository.h"
#include "utils/PasswordHash.h"

#include <chrono>
#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

namespace fs = std::filesystem;

class TempDirectory {
public:
    TempDirectory() : path_(fs::temp_directory_path() / ("cashier_employee_repository_tests_" + std::to_string(std::chrono::steady_clock::now().time_since_epoch().count()))) {
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

void testLoadCreatesSampleEmployeesWhenFileMissing() {
    TempDirectory tempDirectory;
    const auto filePath = tempDirectory.path() / "employees.csv";

    EmployeeRepository repository(filePath.string());
    const auto employees = repository.load();

    assertTrue(employees.size() >= 2, "Expected repository to create at least two sample employees.");
    assertTrue(fs::exists(filePath), "Expected repository to create employees.csv when missing.");

    bool hasAdmin = false;
    bool hasCashier = false;
    for (const auto& e : employees) {
        if (e.role == "admin") hasAdmin = true;
        if (e.role == "cashier") hasCashier = true;
    }
    assertTrue(hasAdmin, "Expected at least one admin employee in sample data.");
    assertTrue(hasCashier, "Expected at least one cashier employee in sample data.");
}

void testPasswordStoredAsHash() {
    TempDirectory tempDirectory;
    const auto filePath = tempDirectory.path() / "employees.csv";

    EmployeeRepository repository(filePath.string());
    const auto employees = repository.load();

    for (const auto& e : employees) {
        assertTrue(e.passwordHash != "admin123", "Password must not be stored as plain text (admin123).");
        assertTrue(e.passwordHash != "cashier123", "Password must not be stored as plain text (cashier123).");
        assertTrue(!e.passwordHash.empty(), "Password hash must not be empty.");
    }
}

void testSaveAndLoadRoundTrip() {
    TempDirectory tempDirectory;
    const auto filePath = tempDirectory.path() / "employees.csv";

    EmployeeRepository repository(filePath.string());
    const std::string hash1 = utils::hashPassword("password1");
    const std::string hash2 = utils::hashPassword("password2");
    const std::vector<Employee> expectedEmployees = {
        {"E010", "张三", hash1, "admin"},
        {"E011", "李四", hash2, "cashier"},
    };

    repository.save(expectedEmployees);
    const auto loadedEmployees = repository.load();

    assertTrue(loadedEmployees.size() == expectedEmployees.size(), "Expected loaded employee count to match saved employee count.");
    assertTrue(loadedEmployees[0].id == expectedEmployees[0].id, "Expected first employee id to match.");
    assertTrue(loadedEmployees[0].name == expectedEmployees[0].name, "Expected first employee name to match.");
    assertTrue(loadedEmployees[0].passwordHash == hash1, "Expected first employee password hash to match.");
    assertTrue(loadedEmployees[0].role == expectedEmployees[0].role, "Expected first employee role to match.");
    assertTrue(loadedEmployees[1].id == expectedEmployees[1].id, "Expected second employee id to match.");
    assertTrue(loadedEmployees[1].role == expectedEmployees[1].role, "Expected second employee role to match.");
}

void testHashPasswordIsDeterministic() {
    const std::string hash1 = utils::hashPassword("testpass");
    const std::string hash2 = utils::hashPassword("testpass");
    assertTrue(hash1 == hash2, "Expected hashPassword to be deterministic for same input.");
    assertTrue(hash1 != "testpass", "Expected hash to differ from plain text.");
    assertTrue(hash1.size() == 16, "Expected FNV-1a hash to be 16 hex characters.");

    const std::string hash3 = utils::hashPassword("otherpass");
    assertTrue(hash1 != hash3, "Expected different passwords to produce different hashes.");
}

int main() {
    try {
        testLoadCreatesSampleEmployeesWhenFileMissing();
        testPasswordStoredAsHash();
        testSaveAndLoadRoundTrip();
        testHashPasswordIsDeterministic();
        std::cout << "All EmployeeRepository tests passed." << std::endl;
        return 0;
    } catch (const std::exception& exception) {
        std::cerr << exception.what() << std::endl;
        return 1;
    }
}
