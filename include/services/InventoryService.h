#pragma once

#include <string>
#include <vector>

#include "models/Product.h"
#include "repositories/ProductRepository.h"

/**
 * 库存管理服务，提供低库存预警和补货操作。
 * 低库存阈值优先读取 data/config.txt，其次读取 data/config.json，默认值为 10。
 */
class InventoryService {
public:
    explicit InventoryService(ProductRepository repository,
                              std::string configPath = "data/config.txt");

    /** 返回低库存阈值（从配置文件读取，默认 10）。 */
    int getLowStockThreshold() const;

    /** 返回按库存升序排列的全部商品列表。 */
    std::vector<Product> getAllSortedByStock() const;

    /** 返回库存低于阈值的商品列表（按库存升序）。 */
    std::vector<Product> getLowStockProducts() const;

    /**
     * 按商品编号补货，补货数量必须大于 0。
     * @throws std::invalid_argument 若商品编号不存在或数量 <= 0。
     */
    void restock(const std::string& productId, int quantity);

private:
    ProductRepository repository_;
    std::string configPath_;
    std::vector<Product> products_;
    int threshold_;

    int loadThreshold() const;
    void persist();
};
