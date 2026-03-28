#pragma once

#include <optional>
#include <string>
#include <vector>

#include "models/Product.h"
#include "repositories/ProductRepository.h"

/**
 * 商品管理服务，提供增删改查操作，每次变更后立即持久化。
 */
class ProductService {
public:
    explicit ProductService(ProductRepository repository);

    /** 返回全部商品列表。 */
    std::vector<Product> getAll() const;

    /** 按名称模糊查询（大小写不敏感）。 */
    std::vector<Product> searchByName(const std::string& keyword) const;

    /** 按分类精确筛选。 */
    std::vector<Product> filterByCategory(const std::string& category) const;

    /** 按 id 查找商品；未找到返回 std::nullopt。 */
    std::optional<Product> findById(const std::string& id) const;

    /**
     * 新增商品。
     * @throws std::invalid_argument 若 id 已存在、price <= 0 或 stock < 0。
     */
    void add(const Product& product);

    /**
     * 修改商品 name/category/price/stock/status 字段。
     * @throws std::invalid_argument 若 id 不存在、price <= 0 或 stock < 0。
     */
    void update(const Product& product);

    /**
     * 按 id 删除商品。
     * @throws std::invalid_argument 若 id 不存在。
     */
    void remove(const std::string& id);

private:
    ProductRepository repository_;
    std::vector<Product> products_;

    void persist();
};
