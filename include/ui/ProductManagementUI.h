#pragma once

#include "services/ProductService.h"

/**
 * 商品管理控制台界面，提供增删改查子菜单。
 */
class ProductManagementUI {
public:
    explicit ProductManagementUI(ProductService& service);

    /** 启动商品管理子菜单循环，直到用户选择返回。 */
    void run();

private:
    ProductService& service_;

    void showList(const std::vector<Product>& products) const;
    void handleList();
    void handleSearch();
    void handleFilterByCategory();
    void handleAdd();
    void handleUpdate();
    void handleRemove();
};
