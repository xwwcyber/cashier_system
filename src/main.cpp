#include <iostream>
#include <string>

#ifdef _WIN32
#include <windows.h>
#endif

#include "repositories/EmployeeRepository.h"
#include "services/AuthService.h"
#include "ui/MainMenu.h"

int main() {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif
    std::cout << "==============================" << std::endl;
    std::cout << "      AI 收银系统 v1.0        " << std::endl;
    std::cout << "==============================" << std::endl;

    EmployeeRepository empRepo("data/employees.csv");
    AuthService auth(empRepo);

    while (true) {
        // 登录循环
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

        // 进入主菜单（退出登录后回到登录循环）
        MainMenu menu(auth);
        menu.run();

        // 如果 stdin 已关闭则退出
        if (std::cin.eof()) {
            break;
        }
    }

    return 0;
}
