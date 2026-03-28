#include "services/SalesReportService.h"

#include <algorithm>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <utility>

SalesReportService::SalesReportService(OrderRepository orderRepository,
                                       ProductRepository productRepository)
    : orderRepository_(std::move(orderRepository)),
      productRepository_(std::move(productRepository)) {}

std::string SalesReportService::today() {
    std::time_t t = std::time(nullptr);
    std::tm tm = *std::localtime(&t);
    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%d");
    return oss.str();
}

SalesReport SalesReportService::generate(const std::string& startDate,
                                          const std::string& endDate) const {
    // 加载筛选范围内的订单
    std::vector<Order> filteredOrders;
    if (!startDate.empty() && !endDate.empty()) {
        filteredOrders = orderRepository_.findByDateRange(startDate, endDate);
    } else {
        filteredOrders = orderRepository_.loadAll();
    }

    // 加载商品分类映射
    auto products = productRepository_.load();
    std::map<std::string, std::string> categoryMap;
    for (const auto& p : products) {
        categoryMap[p.id] = p.category;
    }

    SalesReport report;
    report.orderCount = static_cast<int>(filteredOrders.size());

    // 今日销售额：始终基于当天，不受日期过滤影响
    const std::string todayStr = today();
    auto allOrders = orderRepository_.loadAll();
    for (const auto& o : allOrders) {
        std::string date = o.timestamp.size() >= 10 ? o.timestamp.substr(0, 10) : o.timestamp;
        if (date == todayStr) {
            report.todayRevenue += o.payableAmount;
        }
    }

    // 累计销售额、Top 5、分类汇总
    std::map<std::string, std::pair<std::string, int>> quantityMap; // productId -> (name, qty)
    std::map<std::string, double> revenueByProduct;

    for (const auto& o : filteredOrders) {
        report.totalRevenue += o.payableAmount;
        for (const auto& item : o.items) {
            quantityMap[item.productId].first = item.productName;
            quantityMap[item.productId].second += item.quantity;
            revenueByProduct[item.productId] += item.subtotal;

            std::string category = "其他";
            auto it = categoryMap.find(item.productId);
            if (it != categoryMap.end()) {
                category = it->second;
            }
            report.revenueByCategory[category] += item.subtotal;
        }
    }

    // 按销量排序取 Top 5
    std::vector<TopProduct> topProducts;
    topProducts.reserve(quantityMap.size());
    for (const auto& kv : quantityMap) {
        TopProduct tp;
        tp.productId = kv.first;
        tp.productName = kv.second.first;
        tp.totalQuantity = kv.second.second;
        tp.totalRevenue = revenueByProduct[kv.first];
        topProducts.push_back(tp);
    }
    std::sort(topProducts.begin(), topProducts.end(),
              [](const TopProduct& a, const TopProduct& b) {
                  return a.totalQuantity > b.totalQuantity;
              });
    if (topProducts.size() > 5) {
        topProducts.resize(5);
    }
    report.top5Products = std::move(topProducts);

    return report;
}
