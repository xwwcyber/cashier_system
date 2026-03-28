#include <iostream>
#include <string>

#include "repositories/EmployeeRepository.h"
#include "services/AuthService.h"

int main() {
    std::cout << "==============================" << std::endl;
    std::cout << "      AI 收银系统 v1.0        " << std::endl;
    std::cout << "==============================" << std::endl;

    AuthService auth(EmployeeRepository("data/employees.csv"));

    while (!auth.isLoggedIn()) {
        std::cout << "\n请输入工号：";
        std::string employeeId;
        std::getline(std::cin, employeeId);

        std::cout << "请输入密码：";
        std::string password;
        std::getline(std::cin, password);

        if (auth.login(employeeId, password)) {
            const auto& emp = auth.getCurrentEmployee();
            std::cout << "登录成功！欢迎，" << emp.name
                      << "（" << emp.role << "）" << std::endl;
        } else if (auth.getFailedAttempts() >= AuthService::maxAttempts) {
            std::cout << "连续 " << AuthService::maxAttempts
                      << " 次登录失败，系统退出。" << std::endl;
            return 1;
        } else {
            std::cout << "工号或密码错误，还有 "
                      << (AuthService::maxAttempts - auth.getFailedAttempts())
                      << " 次机会。" << std::endl;
        }
    }

    std::cout << "\n系统已就绪。按 Enter 退出。" << std::endl;
    std::string dummy;
    std::getline(std::cin, dummy);
    return 0;
}
