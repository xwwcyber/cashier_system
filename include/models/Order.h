#pragma once

#include <string>
#include <vector>

/**
 * 订单明细行，描述单笔订单中某商品的购买信息。
 */
struct OrderItem {
    std::string productId;
    std::string productName;
    double price = 0.0;
    int quantity = 0;
    double subtotal = 0.0;
};

/**
 * 订单数据模型，描述一笔完整的销售记录。
 */
struct Order {
    std::string orderId;
    std::string timestamp;   // 格式：YYYY-MM-DD HH:MM:SS
    std::string cashierId;
    std::vector<OrderItem> items;
    double originTotal = 0.0;
    double discountAmount = 0.0;
    double payableAmount = 0.0;
    double paidAmount = 0.0;
    double changeAmount = 0.0;
};
