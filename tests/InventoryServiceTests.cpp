#include "services/InventoryService.h"

#include "repositories/ProductRepository.h"

#include <cassert>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>

namespace {
std::filesystem::path testDir() {
    return std::filesystem::temp_directory_path() / "inventory_service_tests";
}

void cleanup() {
    std::filesystem::remove_all(testDir());
}

ProductRepository makeRepository() {
    return ProductRepository((testDir() / "products.csv").string());
}
}

static void test_default_threshold_when_config_missing() {
    cleanup();
    InventoryService service(makeRepository(), (testDir() / "config.txt").string());
    assert(service.getLowStockThreshold() == 10);
    cleanup();
    std::cout << "[PASS] test_default_threshold_when_config_missing" << std::endl;
}

static void test_reads_threshold_from_text_config() {
    cleanup();
    std::filesystem::create_directories(testDir());
    std::ofstream config(testDir() / "config.txt");
    config << "low_stock_threshold=70\n";
    config.close();

    InventoryService service(makeRepository(), (testDir() / "config.txt").string());
    assert(service.getLowStockThreshold() == 70);
    auto lowStock = service.getLowStockProducts();
    assert(lowStock.size() == 2);
    assert(lowStock[0].id == "P004");
    assert(lowStock[1].id == "P003");

    cleanup();
    std::cout << "[PASS] test_reads_threshold_from_text_config" << std::endl;
}

static void test_reads_threshold_from_json_config() {
    cleanup();
    std::filesystem::create_directories(testDir());
    std::ofstream config(testDir() / "config.json");
    config << "{\n  \"low_stock_threshold\": 50\n}\n";
    config.close();

    InventoryService service(makeRepository(), (testDir() / "config.txt").string());
    assert(service.getLowStockThreshold() == 50);
    auto lowStock = service.getLowStockProducts();
    assert(lowStock.size() == 1);
    assert(lowStock[0].id == "P004");

    cleanup();
    std::cout << "[PASS] test_reads_threshold_from_json_config" << std::endl;
}

static void test_all_products_sorted_by_stock() {
    cleanup();
    InventoryService service(makeRepository(), (testDir() / "config.txt").string());
    auto products = service.getAllSortedByStock();
    assert(products.size() >= 5);
    for (size_t i = 1; i < products.size(); ++i) {
        assert(products[i - 1].stock <= products[i].stock);
    }
    assert(products.front().id == "P004");
    assert(products.back().id == "P001");
    cleanup();
    std::cout << "[PASS] test_all_products_sorted_by_stock" << std::endl;
}

static void test_restock_updates_stock_and_persists() {
    cleanup();
    InventoryService service(makeRepository(), (testDir() / "config.txt").string());
    service.restock("P004", 15);

    auto products = service.getAllSortedByStock();
    bool found = false;
    for (const auto& product : products) {
        if (product.id == "P004") {
            assert(product.stock == 60);
            found = true;
        }
    }
    assert(found);

    ProductRepository reloadedRepo((testDir() / "products.csv").string());
    auto reloaded = reloadedRepo.load();
    found = false;
    for (const auto& product : reloaded) {
        if (product.id == "P004") {
            assert(product.stock == 60);
            found = true;
        }
    }
    assert(found);

    cleanup();
    std::cout << "[PASS] test_restock_updates_stock_and_persists" << std::endl;
}

static void test_restock_rejects_invalid_quantity() {
    cleanup();
    InventoryService service(makeRepository(), (testDir() / "config.txt").string());
    bool threw = false;
    try {
        service.restock("P001", 0);
    } catch (const std::invalid_argument&) {
        threw = true;
    }
    assert(threw);
    cleanup();
    std::cout << "[PASS] test_restock_rejects_invalid_quantity" << std::endl;
}

int main() {
    test_default_threshold_when_config_missing();
    test_reads_threshold_from_text_config();
    test_reads_threshold_from_json_config();
    test_all_products_sorted_by_stock();
    test_restock_updates_stock_and_persists();
    test_restock_rejects_invalid_quantity();
    std::cout << "All inventory service tests passed." << std::endl;
    return 0;
}
