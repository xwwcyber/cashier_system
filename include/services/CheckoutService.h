#pragma once

#include <optional>
#include <string>
#include <vector>

#include "models/Order.h"
#include "repositories/CouponRepository.h"
#include "repositories/OrderRepository.h"
#include "services/ProductService.h"

/**
 * 结账服务，负责优惠计算、找零校验、订单持久化、库存扣减与优惠券核销。
 */
class CheckoutService {
public:
    CheckoutService(ProductService& productService,
                    CouponRepository couponRepository,
                    OrderRepository orderRepository);

    /**
     * 根据原始金额和优惠券码计算折扣金额。
     * 若 couponCode 为空，返回 0.0。
     * @throws std::invalid_argument 若优惠券不存在、未启用、已使用或不满足最低金额。
     */
    double calculateDiscount(double originTotal, const std::string& couponCode) const;

    /**
     * 完成结账：校验支付金额、生成订单、扣减库存、核销优惠券。
     * @param items       购物车条目（不得为空）
     * @param cashierId   当前收银员 ID
     * @param couponCode  优惠券码（为空表示不使用优惠券）
     * @param paidAmount  顾客实付金额
     * @return 已保存的订单
     * @throws std::invalid_argument 若购物车为空、优惠券无效或实付金额不足。
     */
    Order checkout(const std::vector<OrderItem>& items,
                   const std::string& cashierId,
                   const std::string& couponCode,
                   double paidAmount);

private:
    ProductService& productService_;
    CouponRepository couponRepository_;
    OrderRepository orderRepository_;

    // 生成唯一订单编号（基于时间戳）
    std::string generateOrderId() const;

    // 获取当前时间字符串，格式 YYYY-MM-DD HH:MM:SS
    std::string currentTimestamp() const;

    // 按 code 查找优惠券；未找到返回 std::nullopt
    std::optional<Coupon> findCoupon(const std::string& code) const;
};
