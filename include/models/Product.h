#pragma once

#include <string>

/**
 * 商品数据模型，描述可销售商品的基础信息。
 */
struct Product {
    std::string id;
    std::string name;
    std::string category;
    double price = 0.0;
    int stock = 0;
    std::string status;
};
