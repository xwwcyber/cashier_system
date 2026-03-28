#include "ui/InventoryManagementUI.h"

#include <iomanip>
#include <iostream>
#include <string>

InventoryManagementUI::InventoryManagementUI(InventoryService& service) : service_(service) {}

void InventoryManagementUI::showList(const std::vector<Product>& products) const {
    if (products.empty()) {
        std::cout << "（无商品）" << std::endl;
        return;
    }
    std::cout << std::left
              << std::setw(8)  << "编号"
              << std::setw(16) << "名称"
              << std::setw(10) << "分类"
              << std::setw(8)  << "库存"
              << "状态" << std::endl;
    std::cout << std::string(50, '-') << std::endl;
    for (const auto& p : products) {
        std::cout << std::left
                  << std::setw(8)  << p.id
                  << std::setw(16) << p.name
                  << std::setw(10) << p.category
                  << std::setw(8)  << p.stock
                  << p.status << std::endl;
    }
}

void InventoryManagementUI::run() {
    while (true) {
        int lowCount = static_cast<int>(service_.getLowStockProducts().size());
        std::cout << "\n============================" << std::endl;
        std::cout << "        库存管理" << std::endl;
        if (lowCount > 0) {
            std::cout << "  [警告] 低库存商品：" << lowCount << " 件" << std::endl;
        }
        std::cout << "  阈值：" << service_.getLowStockThreshold() << std::endl;
        std::cout << "============================" << std::endl;
        std::cout << "  1. 查看全部库存（按库存升序）" << std::endl;
        std::cout << "  2. 查看低库存商品" << std::endl;
        std::cout << "  3. 补货" << std::endl;
        std::cout << "  0. 返回上级菜单" << std::endl;
        std::cout << "============================" << std::endl;
        std::cout << "请输入菜单编号：";

        std::string line;
        if (!std::getline(std::cin, line)) break;
        int choice = -1;
        try { choice = std::stoi(line); } catch (...) {}

        switch (choice) {
            case 1: handleListAll();      break;
            case 2: handleListLowStock(); break;
            case 3: handleRestock();      break;
            case 0: return;
            default:
                std::cout << "无效菜单编号，请重新输入。" << std::endl;
                break;
        }
    }
}

void InventoryManagementUI::handleListAll() {
    showList(service_.getAllSortedByStock());
}

void InventoryManagementUI::handleListLowStock() {
    auto products = service_.getLowStockProducts();
    if (products.empty()) {
        std::cout << "当前无低库存商品（阈值：" << service_.getLowStockThreshold() << "）。" << std::endl;
        return;
    }
    std::cout << "低库存商品（库存 < " << service_.getLowStockThreshold() << "）：" << std::endl;
    showList(products);
}

void InventoryManagementUI::handleRestock() {
    std::cout << "请输入商品编号：";
    std::string id;
    std::getline(std::cin, id);

    std::cout << "请输入补货数量：";
    std::string qtyStr;
    std::getline(std::cin, qtyStr);

    int qty = 0;
    try { qty = std::stoi(qtyStr); } catch (...) {}

    try {
        service_.restock(id, qty);
        std::cout << "补货成功。" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "补货失败：" << e.what() << std::endl;
    }
}
