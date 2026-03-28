#pragma once

#include <string>
#include <vector>

#include "models/Order.h"

/**
 * 订单文件仓储，负责订单的追加写入与按条件查询。
 */
class OrderRepository {
public:
    /**
     * 使用给定文件路径创建订单仓储。
     */
    explicit OrderRepository(std::string filePath = "data/orders.csv");

    /**
     * 将单笔订单追加写入 CSV 文件（文件不存在时自动创建并写入表头）。
     */
    void append(const Order& order) const;

    /**
     * 加载所有订单。
     */
    std::vector<Order> loadAll() const;

    /**
     * 按 orderId 查询订单；未找到时返回空 optional。
     */
    std::vector<Order> findById(const std::string& orderId) const;

    /**
     * 按日期范围查询订单（包含 startDate 和 endDate 当天，格式 YYYY-MM-DD）。
     */
    std::vector<Order> findByDateRange(const std::string& startDate,
                                       const std::string& endDate) const;

private:
    std::string filePath_;

    // 将订单的 items 序列化为单个 CSV 字段（分号分隔）
    std::string serializeItems(const std::vector<OrderItem>& items) const;

    // 从单个 CSV 字段反序列化 items（分号分隔）
    std::vector<OrderItem> deserializeItems(const std::string& field) const;
};
