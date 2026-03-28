#pragma once

#include <string>
#include <vector>

#include "models/Product.h"

/**
 * 商品文件仓储，负责从 CSV 文件加载和保存商品数据。
 */
class ProductRepository {
public:
    /**
     * 使用给定文件路径创建商品仓储。
     */
    explicit ProductRepository(std::string filePath = "data/products.csv");

    /**
     * 加载商品列表；当文件不存在时自动生成示例数据并落盘。
     */
    std::vector<Product> load() const;

    /**
     * 将商品列表完整写回到 CSV 文件。
     */
    void save(const std::vector<Product>& products) const;

private:
    std::string filePath_;

    std::vector<Product> buildSampleProducts() const;
};
