#include "services/CheckoutService.h"

#include <algorithm>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <utility>

CheckoutService::CheckoutService(ProductService& productService,
                                 CouponRepository couponRepository,
                                 OrderRepository orderRepository)
    : productService_(productService),
      couponRepository_(std::move(couponRepository)),
      orderRepository_(std::move(orderRepository)) {}

std::optional<Coupon> CheckoutService::findCoupon(const std::string& code) const {
    auto coupons = couponRepository_.load();
    for (const auto& c : coupons) {
        if (c.code == code) return c;
    }
    return std::nullopt;
}

double CheckoutService::calculateDiscount(double originTotal,
                                          const std::string& couponCode) const {
    if (couponCode.empty()) return 0.0;

    auto coupon = findCoupon(couponCode);
    if (!coupon.has_value()) {
        throw std::invalid_argument("优惠券不存在: " + couponCode);
    }
    if (!coupon->enabled) {
        throw std::invalid_argument("优惠券已禁用: " + couponCode);
    }
    if (coupon->used) {
        throw std::invalid_argument("优惠券已使用: " + couponCode);
    }
    if (originTotal < coupon->minAmount) {
        throw std::invalid_argument("未达到优惠券最低使用金额: " +
                                    std::to_string(coupon->minAmount));
    }

    double discount = 0.0;
    if (coupon->type == "fixed") {
        discount = coupon->value;
    } else if (coupon->type == "percent") {
        discount = originTotal * coupon->value / 100.0;
    } else {
        throw std::invalid_argument("未知优惠券类型: " + coupon->type);
    }

    // 折扣不能超过原价
    return std::min(discount, originTotal);
}

std::string CheckoutService::generateOrderId() const {
    auto now = std::chrono::system_clock::now();
    auto t   = std::chrono::system_clock::to_time_t(now);
    std::tm tm_val{};
#if defined(_WIN32) || defined(_WIN64)
    localtime_s(&tm_val, &t);
#else
    localtime_r(&t, &tm_val);
#endif
    std::ostringstream oss;
    oss << "ORD-"
        << std::put_time(&tm_val, "%Y%m%d%H%M%S");
    return oss.str();
}

std::string CheckoutService::currentTimestamp() const {
    auto now = std::chrono::system_clock::now();
    auto t   = std::chrono::system_clock::to_time_t(now);
    std::tm tm_val{};
#if defined(_WIN32) || defined(_WIN64)
    localtime_s(&tm_val, &t);
#else
    localtime_r(&t, &tm_val);
#endif
    std::ostringstream oss;
    oss << std::put_time(&tm_val, "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

Order CheckoutService::checkout(const std::vector<OrderItem>& items,
                                const std::string& cashierId,
                                const std::string& couponCode,
                                double paidAmount) {
    if (items.empty()) {
        throw std::invalid_argument("购物车不能为空");
    }

    // 计算原价
    double originTotal = 0.0;
    for (const auto& item : items) {
        originTotal += item.subtotal;
    }

    // 计算折扣
    double discountAmount = calculateDiscount(originTotal, couponCode);
    double payableAmount  = originTotal - discountAmount;

    // 校验实付金额
    if (paidAmount < payableAmount - 1e-9) {
        throw std::invalid_argument("实付金额不足，应付: " +
                                    std::to_string(payableAmount));
    }

    double changeAmount = paidAmount - payableAmount;

    // 构建订单
    Order order;
    order.orderId        = generateOrderId();
    order.timestamp      = currentTimestamp();
    order.cashierId      = cashierId;
    order.items          = items;
    order.originTotal    = originTotal;
    order.discountAmount = discountAmount;
    order.payableAmount  = payableAmount;
    order.paidAmount     = paidAmount;
    order.changeAmount   = changeAmount;

    // 扣减库存
    for (const auto& item : items) {
        auto product = productService_.findById(item.productId);
        if (!product.has_value()) {
            throw std::invalid_argument("结账时商品不存在: " + item.productId);
        }
        Product updated = product.value();
        updated.stock -= item.quantity;
        productService_.update(updated);
    }

    // 核销优惠券
    if (!couponCode.empty()) {
        auto coupons = couponRepository_.load();
        for (auto& c : coupons) {
            if (c.code == couponCode) {
                c.used = true;
                break;
            }
        }
        couponRepository_.save(coupons);
    }

    // 保存订单
    orderRepository_.append(order);

    return order;
}
