#include "services/CartService.h"
#include "repositories/ProductRepository.h"
#include "services/ProductService.h"

#include <cassert>
#include <cmath>
#include <filesystem>
#include <iostream>
#include <stdexcept>

namespace {
std::string tmpDir() {
    auto path = std::filesystem::temp_directory_path() / "cart_service_tests";
    std::filesystem::create_directories(path);
    return (path / "products.csv").string();
}

void cleanup(const std::string& path) {
    std::filesystem::remove(path);
}

bool doubleEquals(double lhs, double rhs) {
    return std::fabs(lhs - rhs) < 1e-9;
}
}

static void test_add_item_and_totals() {
    std::string path = tmpDir();
    cleanup(path);
    ProductRepository repo(path);
    ProductService productService(repo);
    CartService cart(productService);

    cart.addItem("P001", 2);

    auto items = cart.getItems();
    assert(items.size() == 1);
    assert(items[0].productId == "P001");
    assert(items[0].productName == "可口可乐");
    assert(doubleEquals(items[0].price, 3.50));
    assert(items[0].quantity == 2);
    assert(doubleEquals(items[0].subtotal, 7.00));
    assert(cart.getTotalQuantity() == 2);
    assert(doubleEquals(cart.getTotalAmount(), 7.00));

    cleanup(path);
    std::cout << "[PASS] test_add_item_and_totals" << std::endl;
}

static void test_add_same_item_accumulates_quantity() {
    std::string path = tmpDir();
    cleanup(path);
    ProductRepository repo(path);
    ProductService productService(repo);
    CartService cart(productService);

    cart.addItem("P001", 1);
    cart.addItem("P001", 3);

    auto item = cart.findItem("P001");
    assert(item.has_value());
    assert(item->quantity == 4);
    assert(doubleEquals(item->subtotal, 14.00));
    assert(cart.getItems().size() == 1);
    assert(cart.getTotalQuantity() == 4);
    assert(doubleEquals(cart.getTotalAmount(), 14.00));

    cleanup(path);
    std::cout << "[PASS] test_add_same_item_accumulates_quantity" << std::endl;
}

static void test_add_rejects_non_positive_quantity() {
    std::string path = tmpDir();
    cleanup(path);
    ProductRepository repo(path);
    ProductService productService(repo);
    CartService cart(productService);

    cart.addItem("P001", 1);

    bool threw = false;
    try {
        cart.addItem("P001", 0);
    } catch (const std::invalid_argument&) {
        threw = true;
    }
    assert(threw);
    assert(cart.getTotalQuantity() == 1);
    assert(doubleEquals(cart.getTotalAmount(), 3.50));

    threw = false;
    try {
        cart.addItem("P001", -1);
    } catch (const std::invalid_argument&) {
        threw = true;
    }
    assert(threw);
    assert(cart.getTotalQuantity() == 1);
    assert(doubleEquals(cart.getTotalAmount(), 3.50));

    cleanup(path);
    std::cout << "[PASS] test_add_rejects_non_positive_quantity" << std::endl;
}

static void test_add_rejects_inactive_product() {
    std::string path = tmpDir();
    cleanup(path);
    ProductRepository repo(path);
    ProductService productService(repo);
    CartService cart(productService);

    auto product = productService.findById("P001");
    assert(product.has_value());
    Product updated = product.value();
    updated.status = "inactive";
    productService.update(updated);

    bool threw = false;
    try {
        cart.addItem("P001", 1);
    } catch (const std::invalid_argument&) {
        threw = true;
    }
    assert(threw);
    assert(cart.empty());

    cleanup(path);
    std::cout << "[PASS] test_add_rejects_inactive_product" << std::endl;
}

static void test_add_rejects_insufficient_stock() {
    std::string path = tmpDir();
    cleanup(path);
    ProductRepository repo(path);
    ProductService productService(repo);
    CartService cart(productService);

    auto product = productService.findById("P001");
    assert(product.has_value());
    Product updated = product.value();
    updated.stock = 2;
    productService.update(updated);

    bool threw = false;
    try {
        cart.addItem("P001", 3);
    } catch (const std::invalid_argument&) {
        threw = true;
    }
    assert(threw);
    assert(cart.empty());

    cleanup(path);
    std::cout << "[PASS] test_add_rejects_insufficient_stock" << std::endl;
}

static void test_update_quantity_recalculates_totals() {
    std::string path = tmpDir();
    cleanup(path);
    ProductRepository repo(path);
    ProductService productService(repo);
    CartService cart(productService);

    cart.addItem("P001", 2);
    cart.addItem("P002", 1);
    cart.updateItemQuantity("P001", 5);

    auto first = cart.findItem("P001");
    assert(first.has_value());
    assert(first->quantity == 5);
    assert(doubleEquals(first->subtotal, 17.50));
    assert(cart.getTotalQuantity() == 6);
    assert(doubleEquals(cart.getTotalAmount(), 24.30));

    cleanup(path);
    std::cout << "[PASS] test_update_quantity_recalculates_totals" << std::endl;
}

static void test_remove_item_recalculates_totals() {
    std::string path = tmpDir();
    cleanup(path);
    ProductRepository repo(path);
    ProductService productService(repo);
    CartService cart(productService);

    cart.addItem("P001", 2);
    cart.addItem("P002", 1);
    cart.removeItem("P001");

    assert(!cart.findItem("P001").has_value());
    assert(cart.getItems().size() == 1);
    assert(cart.getTotalQuantity() == 1);
    assert(doubleEquals(cart.getTotalAmount(), 6.80));

    cleanup(path);
    std::cout << "[PASS] test_remove_item_recalculates_totals" << std::endl;
}

int main() {
    test_add_item_and_totals();
    test_add_same_item_accumulates_quantity();
    test_add_rejects_non_positive_quantity();
    test_add_rejects_inactive_product();
    test_add_rejects_insufficient_stock();
    test_update_quantity_recalculates_totals();
    test_remove_item_recalculates_totals();
    std::cout << "All cart service tests passed." << std::endl;
    return 0;
}
