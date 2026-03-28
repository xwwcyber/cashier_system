#include "repositories/CouponRepository.h"

#include <cassert>
#include <filesystem>
#include <iostream>
#include <string>

namespace fs = std::filesystem;

// 测试：文件不存在时自动生成示例数据（至少 3 条）
void testAutoGenerateSampleData() {
    const std::string path = "tmp_test_coupons_autogen.csv";
    fs::remove(path);

    CouponRepository repo(path);
    auto coupons = repo.load();

    assert(coupons.size() >= 3 && "Should generate at least 3 sample coupons");
    assert(fs::exists(path) && "CSV file should be created after load()");

    fs::remove(path);
    std::cout << "[PASS] testAutoGenerateSampleData\n";
}

// 测试：type 字段只包含 fixed 或 percent
void testCouponTypes() {
    const std::string path = "tmp_test_coupons_types.csv";
    fs::remove(path);

    CouponRepository repo(path);
    auto coupons = repo.load();

    for (const auto& c : coupons) {
        assert((c.type == "fixed" || c.type == "percent") &&
               "Coupon type must be 'fixed' or 'percent'");
    }

    fs::remove(path);
    std::cout << "[PASS] testCouponTypes\n";
}

// 测试：round-trip 保存与加载
void testRoundTrip() {
    const std::string path = "tmp_test_coupons_roundtrip.csv";
    fs::remove(path);

    std::vector<Coupon> original = {
        {"CODE1", "fixed",   15.0, 80.0, false, true},
        {"CODE2", "percent", 10.0,  0.0, true,  false},
    };

    CouponRepository repo(path);
    repo.save(original);

    auto loaded = repo.load();

    assert(loaded.size() == 2 && "Round-trip should preserve coupon count");
    assert(loaded[0].code == "CODE1");
    assert(loaded[0].type == "fixed");
    assert(loaded[0].value == 15.0);
    assert(loaded[0].minAmount == 80.0);
    assert(loaded[0].used == false);
    assert(loaded[0].enabled == true);
    assert(loaded[1].code == "CODE2");
    assert(loaded[1].used == true);
    assert(loaded[1].enabled == false);

    fs::remove(path);
    std::cout << "[PASS] testRoundTrip\n";
}

// 测试：used/enabled 布尔字段正确持久化
void testBooleanFields() {
    const std::string path = "tmp_test_coupons_bool.csv";
    fs::remove(path);

    std::vector<Coupon> coupons = {
        {"A", "fixed", 5.0, 0.0, true,  true},
        {"B", "fixed", 5.0, 0.0, false, false},
    };

    CouponRepository repo(path);
    repo.save(coupons);
    auto loaded = repo.load();

    assert(loaded[0].used == true);
    assert(loaded[0].enabled == true);
    assert(loaded[1].used == false);
    assert(loaded[1].enabled == false);

    fs::remove(path);
    std::cout << "[PASS] testBooleanFields\n";
}

int main() {
    testAutoGenerateSampleData();
    testCouponTypes();
    testRoundTrip();
    testBooleanFields();

    std::cout << "All coupon repository tests passed.\n";
    return 0;
}
