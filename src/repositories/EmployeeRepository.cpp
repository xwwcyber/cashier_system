#include "repositories/EmployeeRepository.h"

#include <filesystem>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <utility>

#include "utils/PasswordHash.h"

namespace {
std::vector<std::string> splitCsvLine(const std::string& line) {
    std::vector<std::string> fields;
    std::stringstream stream(line);
    std::string field;

    while (std::getline(stream, field, ',')) {
        fields.push_back(field);
    }

    return fields;
}
}  // namespace

EmployeeRepository::EmployeeRepository(std::string filePath) : filePath_(std::move(filePath)) {}

std::vector<Employee> EmployeeRepository::load() const {
    std::ifstream input(filePath_);
    if (!input.is_open()) {
        auto sampleEmployees = buildSampleEmployees();
        save(sampleEmployees);
        return sampleEmployees;
    }

    std::vector<Employee> employees;
    std::string line;

    // 跳过表头
    if (std::getline(input, line)) {
        if (line != "id,name,passwordHash,role") {
            auto headerFields = splitCsvLine(line);
            if (headerFields.size() != 4) {
                throw std::runtime_error("Invalid employees header: " + filePath_);
            }
            employees.push_back(Employee{
                headerFields[0],
                headerFields[1],
                headerFields[2],
                headerFields[3],
            });
        }
    }

    while (std::getline(input, line)) {
        if (line.empty()) {
            continue;
        }

        auto fields = splitCsvLine(line);
        if (fields.size() != 4) {
            throw std::runtime_error("Invalid employee row: " + line);
        }

        employees.push_back(Employee{
            fields[0],
            fields[1],
            fields[2],
            fields[3],
        });
    }

    return employees;
}

void EmployeeRepository::save(const std::vector<Employee>& employees) const {
    const std::filesystem::path filePath(filePath_);
    if (filePath.has_parent_path()) {
        std::filesystem::create_directories(filePath.parent_path());
    }

    std::ofstream output(filePath_);
    if (!output.is_open()) {
        throw std::runtime_error("Unable to open employees file: " + filePath_);
    }

    output << "id,name,passwordHash,role\n";

    for (const auto& employee : employees) {
        output << employee.id << ','
               << employee.name << ','
               << employee.passwordHash << ','
               << employee.role << '\n';
    }
}

std::vector<Employee> EmployeeRepository::buildSampleEmployees() const {
    return {
        {"E001", "管理员", utils::hashPassword("admin123"), "admin"},
        {"E002", "收银员", utils::hashPassword("cashier123"), "cashier"},
    };
}
