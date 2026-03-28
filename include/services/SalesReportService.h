#pragma once

#include <map>
#include <string>
#include <vector>

#include "repositories/OrderRepository.h"
#include "repositories/ProductRepository.h"

struct TopProduct {
    std::string productId;
    std::string productName;
    int totalQuantity = 0;
    double totalRevenue = 0.0;
};

struct SalesReport {
    double todayRevenue = 0.0;     // 今日销售额（不受日期过滤影响）
    double totalRevenue = 0.0;     // 筛选范围内累计销售额
    int orderCount = 0;            // 筛选范围内订单总数
    std::vector<TopProduct> top5Products;          // 销量 Top 5
    std::map<std::string, double> revenueByCategory; // 按分类汇总销售额
};

/**
 * 销售报表服务，提供销售汇总、Top 商品、分类销售额等统计。
 */
class SalesReportService {
public:
    SalesReportService(OrderRepository orderRepository,
                       ProductRepository productRepository);

    /**
     * 生成销售报表，可按日期范围（YYYY-MM-DD）过滤。
     * 空字符串表示不限制（统计全部订单）。
     */
    SalesReport generate(const std::string& startDate = "",
                         const std::string& endDate = "") const;

    /** 返回今日日期字符串（YYYY-MM-DD）。 */
    static std::string today();

private:
    OrderRepository orderRepository_;
    ProductRepository productRepository_;
};
