#pragma once

#include <string>
#include <vector>

#include "models/Coupon.h"

/**
 * 优惠券文件仓储，负责从 CSV 文件加载和保存优惠券数据。
 */
class CouponRepository {
public:
    /**
     * 使用给定文件路径创建优惠券仓储。
     */
    explicit CouponRepository(std::string filePath = "data/coupons.csv");

    /**
     * 加载优惠券列表；当文件不存在时自动生成示例数据并落盘。
     */
    std::vector<Coupon> load() const;

    /**
     * 将优惠券列表完整写回到 CSV 文件。
     */
    void save(const std::vector<Coupon>& coupons) const;

private:
    std::string filePath_;

    std::vector<Coupon> buildSampleCoupons() const;
};
