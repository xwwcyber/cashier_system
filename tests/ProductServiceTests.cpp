#include "services/ProductService.h"
#include "repositories/ProductRepository.h"

#include <cassert>
#include <filesystem>
#include <iostream>
#include <stdexcept>

static std::string tmpDir() {
    auto p = std::filesystem::temp_directory_path() / "ps_tests";
    std::filesystem::create_directories(p);
    return (p / "products.csv").string();
}

static void cleanup(const std::string& path) {
    std::filesystem::remove(path);
}

// 新增商品并查询
static void test_add_and_find() {
    std::string path = tmpDir();
    cleanup(path);
    ProductRepository repo(path);
    ProductService svc(repo);

    // 清空示例数据，重新构造一个空仓储用于测试新增
    // 直接使用 add 验证逻辑
    size_t before = svc.getAll().size();
    Product p{"T001", "测试商品", "测试", 9.9, 10, "active"};
    svc.add(p);
    assert(svc.getAll().size() == before + 1);
    auto found = svc.findById("T001");
    assert(found.has_value());
    assert(found->name == "测试商品");
    cleanup(path);
    std::cout << "[PASS] test_add_and_find" << std::endl;
}

// 重复 id 应抛出异常
static void test_add_duplicate_id() {
    std::string path = tmpDir();
    cleanup(path);
    ProductRepository repo(path);
    ProductService svc(repo);

    // 取第一个示例商品 id
    auto all = svc.getAll();
    assert(!all.empty());
    Product dup = all[0];
    dup.price = 1.0;
    bool threw = false;
    try {
        svc.add(dup);
    } catch (const std::invalid_argument&) {
        threw = true;
    }
    assert(threw);
    cleanup(path);
    std::cout << "[PASS] test_add_duplicate_id" << std::endl;
}

// 价格 <= 0 应抛出异常
static void test_add_invalid_price() {
    std::string path = tmpDir();
    cleanup(path);
    ProductRepository repo(path);
    ProductService svc(repo);

    Product p{"T002", "invalid", "cat", 0.0, 5, "active"};
    bool threw = false;
    try { svc.add(p); } catch (const std::invalid_argument&) { threw = true; }
    assert(threw);
    cleanup(path);
    std::cout << "[PASS] test_add_invalid_price" << std::endl;
}

// 库存 < 0 应抛出异常
static void test_add_negative_stock() {
    std::string path = tmpDir();
    cleanup(path);
    ProductRepository repo(path);
    ProductService svc(repo);

    Product p{"T003", "invalid", "cat", 5.0, -1, "active"};
    bool threw = false;
    try { svc.add(p); } catch (const std::invalid_argument&) { threw = true; }
    assert(threw);
    cleanup(path);
    std::cout << "[PASS] test_add_negative_stock" << std::endl;
}

// 更新商品
static void test_update() {
    std::string path = tmpDir();
    cleanup(path);
    ProductRepository repo(path);
    ProductService svc(repo);

    auto all = svc.getAll();
    assert(!all.empty());
    Product p = all[0];
    p.name = "已修改";
    p.price = 99.9;
    svc.update(p);
    auto found = svc.findById(p.id);
    assert(found.has_value());
    assert(found->name == "已修改");
    assert(found->price == 99.9);
    cleanup(path);
    std::cout << "[PASS] test_update" << std::endl;
}

// 删除商品
static void test_remove() {
    std::string path = tmpDir();
    cleanup(path);
    ProductRepository repo(path);
    ProductService svc(repo);

    auto all = svc.getAll();
    assert(!all.empty());
    std::string id = all[0].id;
    size_t before = all.size();
    svc.remove(id);
    assert(svc.getAll().size() == before - 1);
    assert(!svc.findById(id).has_value());
    cleanup(path);
    std::cout << "[PASS] test_remove" << std::endl;
}

// 按名称搜索
static void test_search_by_name() {
    std::string path = tmpDir();
    cleanup(path);
    ProductRepository repo(path);
    ProductService svc(repo);

    // 示例数据中有「可口可乐」，搜索「可乐」应命中
    auto result = svc.searchByName("可乐");
    assert(!result.empty());
    assert(result[0].name.find("可乐") != std::string::npos);
    cleanup(path);
    std::cout << "[PASS] test_search_by_name" << std::endl;
}

// 按分类筛选
static void test_filter_by_category() {
    std::string path = tmpDir();
    cleanup(path);
    ProductRepository repo(path);
    ProductService svc(repo);

    auto result = svc.filterByCategory("饮料");
    assert(!result.empty());
    for (const auto& p : result) {
        assert(p.category == "饮料");
    }
    cleanup(path);
    std::cout << "[PASS] test_filter_by_category" << std::endl;
}

int main() {
    test_add_and_find();
    test_add_duplicate_id();
    test_add_invalid_price();
    test_add_negative_stock();
    test_update();
    test_remove();
    test_search_by_name();
    test_filter_by_category();
    std::cout << "All product service tests passed." << std::endl;
    return 0;
}
