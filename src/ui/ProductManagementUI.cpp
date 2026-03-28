#include "ui/ProductManagementUI.h"

#include <iomanip>
#include <iostream>
#include <string>

ProductManagementUI::ProductManagementUI(ProductService& service) : service_(service) {}

void ProductManagementUI::run() {
    while (true) {
        std::cout << "\n============================" << std::endl;
        std::cout << "        商品管理" << std::endl;
        std::cout << "============================" << std::endl;
        std::cout << "  1. 查看全部商品" << std::endl;
        std::cout << "  2. 按名称搜索" << std::endl;
        std::cout << "  3. 按分类筛选" << std::endl;
        std::cout << "  4. 新增商品" << std::endl;
        std::cout << "  5. 修改商品" << std::endl;
        std::cout << "  6. 删除商品" << std::endl;
        std::cout << "  0. 返回上级菜单" << std::endl;
        std::cout << "============================" << std::endl;
        std::cout << "请输入菜单编号：";

        std::string line;
        if (!std::getline(std::cin, line)) break;
        int choice = -1;
        try { choice = std::stoi(line); } catch (...) {}

        switch (choice) {
            case 1: handleList();             break;
            case 2: handleSearch();           break;
            case 3: handleFilterByCategory(); break;
            case 4: handleAdd();              break;
            case 5: handleUpdate();           break;
            case 6: handleRemove();           break;
            case 0: return;
            default:
                std::cout << "无效菜单编号，请重新输入。" << std::endl;
                break;
        }
    }
}

void ProductManagementUI::showList(const std::vector<Product>& products) const {
    if (products.empty()) {
        std::cout << "（无商品）" << std::endl;
        return;
    }
    std::cout << std::left
              << std::setw(8)  << "编号"
              << std::setw(16) << "名称"
              << std::setw(10) << "分类"
              << std::setw(10) << "价格"
              << std::setw(8)  << "库存"
              << "状态" << std::endl;
    std::cout << std::string(60, '-') << std::endl;
    for (const auto& p : products) {
        std::cout << std::left
                  << std::setw(8)  << p.id
                  << std::setw(16) << p.name
                  << std::setw(10) << p.category
                  << std::setw(10) << std::fixed << std::setprecision(2) << p.price
                  << std::setw(8)  << p.stock
                  << p.status << std::endl;
    }
}

void ProductManagementUI::handleList() {
    showList(service_.getAll());
}

void ProductManagementUI::handleSearch() {
    std::cout << "请输入搜索关键词：";
    std::string kw;
    std::getline(std::cin, kw);
    showList(service_.searchByName(kw));
}

void ProductManagementUI::handleFilterByCategory() {
    std::cout << "请输入分类名称：";
    std::string cat;
    std::getline(std::cin, cat);
    showList(service_.filterByCategory(cat));
}

void ProductManagementUI::handleAdd() {
    Product p;
    std::cout << "商品编号："; std::getline(std::cin, p.id);
    std::cout << "商品名称："; std::getline(std::cin, p.name);
    std::cout << "商品分类："; std::getline(std::cin, p.category);

    std::string val;
    std::cout << "价格："; std::getline(std::cin, val);
    try { p.price = std::stod(val); } catch (...) { p.price = 0.0; }

    std::cout << "库存："; std::getline(std::cin, val);
    try { p.stock = std::stoi(val); } catch (...) { p.stock = -1; }

    std::cout << "状态 (active/inactive)："; std::getline(std::cin, p.status);

    try {
        service_.add(p);
        std::cout << "商品已新增。" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "新增失败：" << e.what() << std::endl;
    }
}

void ProductManagementUI::handleUpdate() {
    std::cout << "请输入要修改的商品编号：";
    std::string id;
    std::getline(std::cin, id);

    auto opt = service_.findById(id);
    if (!opt.has_value()) {
        std::cout << "商品编号不存在。" << std::endl;
        return;
    }
    Product p = opt.value();
    std::cout << "当前信息：" << p.name << " | " << p.category
              << " | " << p.price << " | " << p.stock << " | " << p.status << std::endl;
    std::cout << "（直接回车保留原值）" << std::endl;

    std::string val;
    std::cout << "名称 [" << p.name << "]："; std::getline(std::cin, val);
    if (!val.empty()) p.name = val;

    std::cout << "分类 [" << p.category << "]："; std::getline(std::cin, val);
    if (!val.empty()) p.category = val;

    std::cout << "价格 [" << p.price << "]："; std::getline(std::cin, val);
    if (!val.empty()) { try { p.price = std::stod(val); } catch (...) {} }

    std::cout << "库存 [" << p.stock << "]："; std::getline(std::cin, val);
    if (!val.empty()) { try { p.stock = std::stoi(val); } catch (...) {} }

    std::cout << "状态 [" << p.status << "]："; std::getline(std::cin, val);
    if (!val.empty()) p.status = val;

    try {
        service_.update(p);
        std::cout << "商品已更新。" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "更新失败：" << e.what() << std::endl;
    }
}

void ProductManagementUI::handleRemove() {
    std::cout << "请输入要删除的商品编号：";
    std::string id;
    std::getline(std::cin, id);

    auto opt = service_.findById(id);
    if (!opt.has_value()) {
        std::cout << "商品编号不存在。" << std::endl;
        return;
    }
    const auto& p = opt.value();
    std::cout << "确认删除商品 [" << p.id << "] " << p.name << "？(y/N)：";
    std::string confirm;
    std::getline(std::cin, confirm);
    if (confirm != "y" && confirm != "Y") {
        std::cout << "已取消。" << std::endl;
        return;
    }

    try {
        service_.remove(id);
        std::cout << "商品已删除。" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "删除失败：" << e.what() << std::endl;
    }
}
