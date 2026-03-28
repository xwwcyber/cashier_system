#pragma once

#include "services/AuthService.h"

/**
 * 控制台主菜单，根据登录员工角色显示对应的功能菜单。
 * 管理员看到完整功能菜单，收银员只看到收银相关功能。
 */
class MainMenu {
public:
    explicit MainMenu(AuthService& auth);

    /** 启动主菜单循环，直到用户退出登录为止。 */
    void run();

private:
    AuthService& auth_;

    /** 显示并处理管理员主菜单，返回 false 表示退出登录。 */
    bool showAdminMenu();

    /** 显示并处理收银员主菜单，返回 false 表示退出登录。 */
    bool showCashierMenu();

    // --- 子菜单处理 ---
    void handleStartCheckout();
    void handleProductManagement();
    void handleInventoryManagement();
    void handleCouponManagement();
    void handleEmployeeManagement();
    void handleSalesReport();
    void handleBrowseProducts();
    void handleQueryOrders();
};
