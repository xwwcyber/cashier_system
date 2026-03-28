#include "services/SalesReportService.h"

#include "repositories/OrderRepository.h"
#include "repositories/ProductRepository.h"

#include <cassert>
#include <cmath>
#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

namespace {
std::filesystem::path testDir() {
    return std::filesystem::temp_directory_path() / "sales_report_tests";
}

void cleanup() {
    std::filesystem::remove_all(testDir());
}

bool doubleNear(double a, double b) {
    return std::fabs(a - b) < 1e-6;
}

Order makeOrder(const std::string& id,
                const std::string& date,
                const std::vector<OrderItem>& items,
                double payable) {
    Order o;
    o.orderId      = id;
    o.timestamp    = date + " 10:00:00";
    o.cashierId    = "E001";
    o.items        = items;
    o.originTotal  = payable;
    o.discountAmount = 0.0;
    o.payableAmount = payable;
    o.paidAmount   = payable;
    o.changeAmount = 0.0;
    return o;
}

SalesReportService makeService() {
    OrderRepository orderRepo((testDir() / "orders.csv").string());
    ProductRepository productRepo((testDir() / "products.csv").string());
    return SalesReportService(orderRepo, productRepo);
}
}

static void test_empty_report() {
    cleanup();
    auto service = makeService();
    SalesReport report = service.generate();
    assert(report.orderCount == 0);
    assert(doubleNear(report.totalRevenue, 0.0));
    assert(report.top5Products.empty());
    assert(report.revenueByCategory.empty());
    cleanup();
    std::cout << "[PASS] test_empty_report" << std::endl;
}

static void test_total_revenue_and_order_count() {
    cleanup();
    std::filesystem::create_directories(testDir());
    OrderRepository orderRepo((testDir() / "orders.csv").string());

    OrderItem item1{"P001", "可口可乐", 3.50, 2, 7.00};
    OrderItem item2{"P002", "牛奶", 6.80, 1, 6.80};
    orderRepo.append(makeOrder("ORD001", "2024-01-10", {item1}, 7.00));
    orderRepo.append(makeOrder("ORD002", "2024-01-11", {item2}, 6.80));

    ProductRepository productRepo((testDir() / "products.csv").string());
    SalesReportService service(orderRepo, productRepo);
    SalesReport report = service.generate();

    assert(report.orderCount == 2);
    assert(doubleNear(report.totalRevenue, 13.80));
    cleanup();
    std::cout << "[PASS] test_total_revenue_and_order_count" << std::endl;
}

static void test_top5_products() {
    cleanup();
    std::filesystem::create_directories(testDir());
    OrderRepository orderRepo((testDir() / "orders.csv").string());

    // 6 种商品，各有不同销量
    orderRepo.append(makeOrder("ORD001", "2024-01-10",
        {{"P001","商品A",10.0,5,50.0},{"P002","商品B",10.0,3,30.0}}, 80.0));
    orderRepo.append(makeOrder("ORD002", "2024-01-10",
        {{"P003","商品C",10.0,8,80.0},{"P004","商品D",10.0,2,20.0}}, 100.0));
    orderRepo.append(makeOrder("ORD003", "2024-01-10",
        {{"P005","商品E",10.0,6,60.0},{"P006","商品F",10.0,1,10.0}}, 70.0));

    ProductRepository productRepo((testDir() / "products.csv").string());
    SalesReportService service(orderRepo, productRepo);
    SalesReport report = service.generate();

    // Top 5 应按销量降序：C(8) > E(6) > A(5) > B(3) > D(2)，F(1) 不入榜
    assert(report.top5Products.size() == 5);
    assert(report.top5Products[0].productId == "P003");
    assert(report.top5Products[0].totalQuantity == 8);
    assert(report.top5Products[4].productId == "P004");
    assert(report.top5Products[4].totalQuantity == 2);
    cleanup();
    std::cout << "[PASS] test_top5_products" << std::endl;
}

static void test_revenue_by_category() {
    cleanup();
    std::filesystem::create_directories(testDir());
    OrderRepository orderRepo((testDir() / "orders.csv").string());

    orderRepo.append(makeOrder("ORD001", "2024-01-10",
        {{"P001","可乐",3.50,2,7.00}}, 7.00));
    orderRepo.append(makeOrder("ORD002", "2024-01-10",
        {{"P002","牛奶",6.80,1,6.80}}, 6.80));

    // P001 -> 饮料, P002 -> 乳品
    ProductRepository productRepo((testDir() / "products.csv").string());
    // 覆盖自动生成的示例商品，写入我们需要的
    auto products = productRepo.load();
    // 找到 P001 和 P002 并修改分类
    for (auto& p : products) {
        if (p.id == "P001") p.category = "饮料";
        if (p.id == "P002") p.category = "乳品";
    }
    productRepo.save(products);

    SalesReportService service(OrderRepository((testDir() / "orders.csv").string()),
                               productRepo);
    SalesReport report = service.generate();

    assert(report.revenueByCategory.count("饮料") > 0);
    assert(doubleNear(report.revenueByCategory.at("饮料"), 7.00));
    assert(report.revenueByCategory.count("乳品") > 0);
    assert(doubleNear(report.revenueByCategory.at("乳品"), 6.80));
    cleanup();
    std::cout << "[PASS] test_revenue_by_category" << std::endl;
}

static void test_date_range_filter() {
    cleanup();
    std::filesystem::create_directories(testDir());
    OrderRepository orderRepo((testDir() / "orders.csv").string());

    OrderItem item{"P001","商品A",10.0,1,10.0};
    orderRepo.append(makeOrder("ORD001", "2024-01-05", {item}, 10.0));
    orderRepo.append(makeOrder("ORD002", "2024-01-10", {item}, 10.0));
    orderRepo.append(makeOrder("ORD003", "2024-01-20", {item}, 10.0));

    ProductRepository productRepo((testDir() / "products.csv").string());
    SalesReportService service(orderRepo, productRepo);

    // 筛选 01-08 ~ 01-15，只有 ORD002
    SalesReport report = service.generate("2024-01-08", "2024-01-15");
    assert(report.orderCount == 1);
    assert(doubleNear(report.totalRevenue, 10.0));

    // 全部
    SalesReport all = service.generate();
    assert(all.orderCount == 3);
    assert(doubleNear(all.totalRevenue, 30.0));

    cleanup();
    std::cout << "[PASS] test_date_range_filter" << std::endl;
}

int main() {
    test_empty_report();
    test_total_revenue_and_order_count();
    test_top5_products();
    test_revenue_by_category();
    test_date_range_filter();
    std::cout << "All sales report service tests passed." << std::endl;
    return 0;
}
