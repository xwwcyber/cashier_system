#pragma once

#include "services/InventoryService.h"

/**
 * 库存管理控制台界面，提供库存查看和补货子菜单。
 */
class InventoryManagementUI {
public:
    explicit InventoryManagementUI(InventoryService& service);

    /** 启动库存管理子菜单循环，直到用户选择返回。 */
    void run();

private:
    InventoryService& service_;

    void showList(const std::vector<Product>& products) const;
    void handleListAll();
    void handleListLowStock();
    void handleRestock();
};
