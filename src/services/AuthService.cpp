#include "services/AuthService.h"

#include "utils/PasswordHash.h"

AuthService::AuthService(EmployeeRepository employeeRepository)
    : employeeRepository_(std::move(employeeRepository)) {}

bool AuthService::login(const std::string& employeeId, const std::string& password) {
    if (failedAttempts_ >= maxAttempts) {
        return false;
    }

    const auto employees = employeeRepository_.load();
    const auto hash = utils::hashPassword(password);

    for (const auto& employee : employees) {
        if (employee.id == employeeId && employee.passwordHash == hash) {
            currentEmployee_ = employee;
            failedAttempts_ = 0;
            return true;
        }
    }

    ++failedAttempts_;
    return false;
}

void AuthService::logout() {
    currentEmployee_.reset();
    failedAttempts_ = 0;
}

bool AuthService::isLoggedIn() const {
    return currentEmployee_.has_value();
}

const Employee& AuthService::getCurrentEmployee() const {
    return currentEmployee_.value();
}

int AuthService::getFailedAttempts() const {
    return failedAttempts_;
}
