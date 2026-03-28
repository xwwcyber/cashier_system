#include "repositories/OrderRepository.h"

#include <cassert>
#include <filesystem>
#include <iostream>
#include <string>

namespace fs = std::filesystem;

// 构造一个示例订单
Order makeSampleOrder(const std::string& id, const std::string& date) {
    Order o;
    o.orderId        = id;
    o.timestamp      = date + " 10:00:00";
    o.cashierId      = "E002";
    o.originTotal    = 100.00;
    o.discountAmount = 10.00;
    o.payableAmount  = 90.00;
    o.paidAmount     = 100.00;
    o.changeAmount   = 10.00;

    OrderItem item;
    item.productId   = "P001";
    item.productName = "可口可乐";
    item.price       = 3.50;
    item.quantity    = 2;
    item.subtotal    = 7.00;
    o.items.push_back(item);

    return o;
}

// 测试：文件不存在时 loadAll() 返回空列表
void testLoadAllEmpty() {
    const std::string path = "tmp_order_empty.csv";
    fs::remove(path);

    OrderRepository repo(path);
    auto orders = repo.loadAll();

    assert(orders.empty() && "loadAll() on missing file should return empty");
    assert(!fs::exists(path) && "Missing file should NOT be created by loadAll()");

    std::cout << "[PASS] testLoadAllEmpty\n";
}

// 测试：append 写入后可以 loadAll 读回
void testAppendAndLoadAll() {
    const std::string path = "tmp_order_append.csv";
    fs::remove(path);

    OrderRepository repo(path);
    Order o = makeSampleOrder("ORD-001", "2026-03-28");
    repo.append(o);

    auto orders = repo.loadAll();
    assert(orders.size() == 1 && "Should load exactly 1 order");

    const auto& loaded = orders[0];
    assert(loaded.orderId        == "ORD-001");
    assert(loaded.cashierId      == "E002");
    assert(loaded.originTotal    == 100.00);
    assert(loaded.discountAmount == 10.00);
    assert(loaded.payableAmount  == 90.00);
    assert(loaded.paidAmount     == 100.00);
    assert(loaded.changeAmount   == 10.00);
    assert(loaded.items.size()   == 1);
    assert(loaded.items[0].productId   == "P001");
    assert(loaded.items[0].productName == "可口可乐");
    assert(loaded.items[0].quantity    == 2);

    fs::remove(path);
    std::cout << "[PASS] testAppendAndLoadAll\n";
}

// 测试：多条追加写入，顺序保留
void testMultipleAppend() {
    const std::string path = "tmp_order_multi.csv";
    fs::remove(path);

    OrderRepository repo(path);
    repo.append(makeSampleOrder("ORD-001", "2026-03-28"));
    repo.append(makeSampleOrder("ORD-002", "2026-03-28"));
    repo.append(makeSampleOrder("ORD-003", "2026-03-29"));

    auto orders = repo.loadAll();
    assert(orders.size() == 3 && "Should load 3 orders");
    assert(orders[0].orderId == "ORD-001");
    assert(orders[1].orderId == "ORD-002");
    assert(orders[2].orderId == "ORD-003");

    fs::remove(path);
    std::cout << "[PASS] testMultipleAppend\n";
}

// 测试：按 orderId 查询
void testFindById() {
    const std::string path = "tmp_order_findbyid.csv";
    fs::remove(path);

    OrderRepository repo(path);
    repo.append(makeSampleOrder("ORD-001", "2026-03-28"));
    repo.append(makeSampleOrder("ORD-002", "2026-03-28"));

    auto found = repo.findById("ORD-002");
    assert(found.size() == 1 && "Should find exactly 1 order by id");
    assert(found[0].orderId == "ORD-002");

    auto notFound = repo.findById("ORD-999");
    assert(notFound.empty() && "Non-existent id should return empty");

    fs::remove(path);
    std::cout << "[PASS] testFindById\n";
}

// 测试：按日期范围查询
void testFindByDateRange() {
    const std::string path = "tmp_order_daterange.csv";
    fs::remove(path);

    OrderRepository repo(path);
    repo.append(makeSampleOrder("ORD-001", "2026-03-27"));
    repo.append(makeSampleOrder("ORD-002", "2026-03-28"));
    repo.append(makeSampleOrder("ORD-003", "2026-03-29"));
    repo.append(makeSampleOrder("ORD-004", "2026-03-30"));

    auto results = repo.findByDateRange("2026-03-28", "2026-03-29");
    assert(results.size() == 2 && "Should find 2 orders in date range");
    assert(results[0].orderId == "ORD-002");
    assert(results[1].orderId == "ORD-003");

    fs::remove(path);
    std::cout << "[PASS] testFindByDateRange\n";
}

int main() {
    testLoadAllEmpty();
    testAppendAndLoadAll();
    testMultipleAppend();
    testFindById();
    testFindByDateRange();

    std::cout << "All order repository tests passed.\n";
    return 0;
}
