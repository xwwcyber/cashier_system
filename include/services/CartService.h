#pragma once

#include <optional>
#include <string>
#include <vector>

#include "models/Order.h"
#include "services/ProductService.h"

/**
 * 购物车服务，负责在内存中维护当前收银购物车条目。
 * 购物车条目复用 OrderItem 结构，便于后续结账直接生成订单。
 */
class CartService {
public:
    explicit CartService(ProductService& productService);

    /** 返回当前购物车全部条目。 */
    std::vector<OrderItem> getItems() const;

    /** 按商品编号查找购物车条目；未找到返回 std::nullopt。 */
    std::optional<OrderItem> findItem(const std::string& productId) const;

    /**
     * 按商品编号将商品加入购物车。
     * 若商品已在购物车中，则累计数量。
     * @throws std::invalid_argument 若商品不存在、已下架、库存不足或 quantity <= 0。
     */
    void addItem(const std::string& productId, int quantity);

    /**
     * 修改购物车中某商品的数量。
     * @throws std::invalid_argument 若商品不存在于购物车、已下架、库存不足或 quantity <= 0。
     */
    void updateItemQuantity(const std::string& productId, int quantity);

    /**
     * 从购物车移除指定商品。
     * @throws std::invalid_argument 若商品不存在于购物车。
     */
    void removeItem(const std::string& productId);

    /** 清空当前购物车。 */
    void clear();

    /** 返回购物车商品总件数（数量之和）。 */
    int getTotalQuantity() const;

    /** 返回购物车应付总额（各条目小计之和）。 */
    double getTotalAmount() const;

    /** 当前购物车是否为空。 */
    bool empty() const;

private:
    ProductService& productService_;
    std::vector<OrderItem> items_;
};
