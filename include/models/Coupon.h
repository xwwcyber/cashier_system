#pragma once

#include <string>

/**
 * 优惠券数据模型，描述一张可在结账时使用的折扣凭证。
 * type=fixed  表示固定减免金额（元）
 * type=percent 表示百分比折扣（0 < value <= 100）
 */
struct Coupon {
    std::string code;      // 优惠券码，唯一标识
    std::string type;      // "fixed" 或 "percent"
    double value = 0.0;    // 折扣值
    double minAmount = 0.0; // 最低使用金额
    bool used = false;     // 是否已使用
    bool enabled = true;   // 是否启用
};
