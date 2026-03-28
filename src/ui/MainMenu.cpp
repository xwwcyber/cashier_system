#include "ui/MainMenu.h"

#include <iostream>
#include <limits>
#include <string>

MainMenu::MainMenu(AuthService& auth) : auth_(auth) {}

void MainMenu::run() {
    while (auth_.isLoggedIn()) {
        const auto& emp = auth_.getCurrentEmployee();
        if (emp.role == "admin") {
            if (!showAdminMenu()) {
                break;
            }
        } else {
            if (!showCashierMenu()) {
                break;
            }
        }
    }
}

bool MainMenu::showAdminMenu() {
    const auto& emp = auth_.getCurrentEmployee();
    std::cout << "\n============================" << std::endl;
    std::cout << "  管理员菜单 [" << emp.name << "]" << std::endl;
    std::cout << "============================" << std::endl;
    std::cout << "  1. 开始收银" << std::endl;
    std::cout << "  2. 商品管理" << std::endl;
    std::cout << "  3. 库存管理" << std::endl;
    std::cout << "  4. 优惠券管理" << std::endl;
    std::cout << "  5. 员工管理" << std::endl;
    std::cout << "  6. 销售报表" << std::endl;
    std::cout << "  0. 退出登录" << std::endl;
    std::cout << "============================" << std::endl;
    std::cout << "请输入菜单编号：";

    int choice = -1;
    std::string line;
    if (!std::getline(std::cin, line)) {
        return false;
    }
    try {
        choice = std::stoi(line);
    } catch (...) {
        choice = -1;
    }

    switch (choice) {
        case 1: handleStartCheckout();       break;
        case 2: handleProductManagement();   break;
        case 3: handleInventoryManagement(); break;
        case 4: handleCouponManagement();    break;
        case 5: handleEmployeeManagement();  break;
        case 6: handleSalesReport();         break;
        case 0:
            auth_.logout();
            std::cout << "已退出登录。" << std::endl;
            return false;
        default:
            std::cout << "无效菜单编号，请重新输入。" << std::endl;
            break;
    }
    return auth_.isLoggedIn();
}

bool MainMenu::showCashierMenu() {
    const auto& emp = auth_.getCurrentEmployee();
    std::cout << "\n============================" << std::endl;
    std::cout << "  收银员菜单 [" << emp.name << "]" << std::endl;
    std::cout << "============================" << std::endl;
    std::cout << "  1. 开始收银" << std::endl;
    std::cout << "  2. 浏览商品" << std::endl;
    std::cout << "  3. 查询订单" << std::endl;
    std::cout << "  0. 退出登录" << std::endl;
    std::cout << "============================" << std::endl;
    std::cout << "请输入菜单编号：";

    int choice = -1;
    std::string line;
    if (!std::getline(std::cin, line)) {
        return false;
    }
    try {
        choice = std::stoi(line);
    } catch (...) {
        choice = -1;
    }

    switch (choice) {
        case 1: handleStartCheckout();  break;
        case 2: handleBrowseProducts(); break;
        case 3: handleQueryOrders();    break;
        case 0:
            auth_.logout();
            std::cout << "已退出登录。" << std::endl;
            return false;
        default:
            std::cout << "无效菜单编号，请重新输入。" << std::endl;
            break;
    }
    return auth_.isLoggedIn();
}

// --- 子菜单占位（功能将在后续 US 实现）---

void MainMenu::handleStartCheckout() {
    std::cout << "[开始收银] 功能即将推出。按 Enter 返回。" << std::endl;
    std::string dummy;
    std::getline(std::cin, dummy);
}

void MainMenu::handleProductManagement() {
    std::cout << "[商品管理] 功能即将推出。按 Enter 返回。" << std::endl;
    std::string dummy;
    std::getline(std::cin, dummy);
}

void MainMenu::handleInventoryManagement() {
    std::cout << "[库存管理] 功能即将推出。按 Enter 返回。" << std::endl;
    std::string dummy;
    std::getline(std::cin, dummy);
}

void MainMenu::handleCouponManagement() {
    std::cout << "[优惠券管理] 功能即将推出。按 Enter 返回。" << std::endl;
    std::string dummy;
    std::getline(std::cin, dummy);
}

void MainMenu::handleEmployeeManagement() {
    std::cout << "[员工管理] 功能即将推出。按 Enter 返回。" << std::endl;
    std::string dummy;
    std::getline(std::cin, dummy);
}

void MainMenu::handleSalesReport() {
    std::cout << "[销售报表] 功能即将推出。按 Enter 返回。" << std::endl;
    std::string dummy;
    std::getline(std::cin, dummy);
}

void MainMenu::handleBrowseProducts() {
    std::cout << "[浏览商品] 功能即将推出。按 Enter 返回。" << std::endl;
    std::string dummy;
    std::getline(std::cin, dummy);
}

void MainMenu::handleQueryOrders() {
    std::cout << "[查询订单] 功能即将推出。按 Enter 返回。" << std::endl;
    std::string dummy;
    std::getline(std::cin, dummy);
}
