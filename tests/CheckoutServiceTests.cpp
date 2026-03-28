#include "services/CheckoutService.h"

#include "repositories/CouponRepository.h"
#include "repositories/OrderRepository.h"
#include "repositories/ProductRepository.h"
#include "services/ProductService.h"

#include <cassert>
#include <cmath>
#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

namespace {
std::filesystem::path testDir() {
    return std::filesystem::temp_directory_path() / "checkout_service_tests";
}

void cleanup() {
    std::filesystem::remove_all(testDir());
}

bool doubleEquals(double lhs, double rhs) {
    return std::fabs(lhs - rhs) < 1e-9;
}

std::vector<OrderItem> makeItems() {
    return {
        {"P001", "可口可乐", 3.50, 2, 7.00},
        {"P002", "纯牛奶", 6.80, 1, 6.80},
    };
}

CheckoutService makeService(ProductService& productService) {
    CouponRepository couponRepository((testDir() / "coupons.csv").string());
    OrderRepository orderRepository((testDir() / "orders.csv").string());
    return CheckoutService(productService, couponRepository, orderRepository);
}
}

static void test_calculate_fixed_discount() {
    cleanup();
    ProductRepository productRepository((testDir() / "products.csv").string());
    ProductService productService(productRepository);
    CheckoutService service = makeService(productService);

    double discount = service.calculateDiscount(80.0, "SAVE10");
    assert(doubleEquals(discount, 10.0));

    cleanup();
    std::cout << "[PASS] test_calculate_fixed_discount" << std::endl;
}

static void test_calculate_percent_discount() {
    cleanup();
    ProductRepository productRepository((testDir() / "products.csv").string());
    ProductService productService(productRepository);
    CheckoutService service = makeService(productService);

    double discount = service.calculateDiscount(150.0, "OFF20");
    assert(doubleEquals(discount, 30.0));

    cleanup();
    std::cout << "[PASS] test_calculate_percent_discount" << std::endl;
}

static void test_calculate_discount_rejects_min_amount_unmet() {
    cleanup();
    ProductRepository productRepository((testDir() / "products.csv").string());
    ProductService productService(productRepository);
    CheckoutService service = makeService(productService);

    bool threw = false;
    try {
        service.calculateDiscount(80.0, "OFF20");
    } catch (const std::invalid_argument&) {
        threw = true;
    }
    assert(threw);

    cleanup();
    std::cout << "[PASS] test_calculate_discount_rejects_min_amount_unmet" << std::endl;
}

static void test_checkout_rejects_insufficient_payment() {
    cleanup();
    ProductRepository productRepository((testDir() / "products.csv").string());
    ProductService productService(productRepository);
    CheckoutService service = makeService(productService);

    bool threw = false;
    try {
        service.checkout(makeItems(), "E002", "VIP5", 8.79);
    } catch (const std::invalid_argument&) {
        threw = true;
    }
    assert(threw);

    OrderRepository orderRepository((testDir() / "orders.csv").string());
    auto orders = orderRepository.loadAll();
    assert(orders.empty());

    auto product = productService.findById("P001");
    assert(product.has_value());
    assert(product->stock == 120);

    CouponRepository couponRepository((testDir() / "coupons.csv").string());
    auto coupons = couponRepository.load();
    for (const auto& coupon : coupons) {
        if (coupon.code == "VIP5") {
            assert(!coupon.used);
        }
    }

    cleanup();
    std::cout << "[PASS] test_checkout_rejects_insufficient_payment" << std::endl;
}

static void test_checkout_persists_order_updates_stock_and_marks_coupon_used() {
    cleanup();
    ProductRepository productRepository((testDir() / "products.csv").string());
    ProductService productService(productRepository);
    CheckoutService service = makeService(productService);

    Order order = service.checkout(makeItems(), "E002", "VIP5", 10.0);

    assert(order.cashierId == "E002");
    assert(order.items.size() == 2);
    assert(doubleEquals(order.originTotal, 13.8));
    assert(doubleEquals(order.discountAmount, 5.0));
    assert(doubleEquals(order.payableAmount, 8.8));
    assert(doubleEquals(order.paidAmount, 10.0));
    assert(doubleEquals(order.changeAmount, 1.2));
    assert(!order.orderId.empty());
    assert(!order.timestamp.empty());

    OrderRepository orderRepository((testDir() / "orders.csv").string());
    auto orders = orderRepository.loadAll();
    assert(orders.size() == 1);
    assert(orders[0].orderId == order.orderId);
    assert(doubleEquals(orders[0].payableAmount, 8.8));

    auto cola = productService.findById("P001");
    auto milk = productService.findById("P002");
    assert(cola.has_value());
    assert(milk.has_value());
    assert(cola->stock == 118);
    assert(milk->stock == 79);

    CouponRepository couponRepository((testDir() / "coupons.csv").string());
    auto coupons = couponRepository.load();
    bool couponUsed = false;
    for (const auto& coupon : coupons) {
        if (coupon.code == "VIP5") {
            couponUsed = true;
            assert(coupon.used);
        }
    }
    assert(couponUsed);

    bool threw = false;
    try {
        service.calculateDiscount(20.0, "VIP5");
    } catch (const std::invalid_argument&) {
        threw = true;
    }
    assert(threw);

    cleanup();
    std::cout << "[PASS] test_checkout_persists_order_updates_stock_and_marks_coupon_used" << std::endl;
}

int main() {
    test_calculate_fixed_discount();
    test_calculate_percent_discount();
    test_calculate_discount_rejects_min_amount_unmet();
    test_checkout_rejects_insufficient_payment();
    test_checkout_persists_order_updates_stock_and_marks_coupon_used();
    std::cout << "All checkout service tests passed." << std::endl;
    return 0;
}
