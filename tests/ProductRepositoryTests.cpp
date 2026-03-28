#include "repositories/ProductRepository.h"

#include <chrono>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

namespace fs = std::filesystem;

class TempDirectory {
public:
    TempDirectory() : path_(fs::temp_directory_path() / ("cashier_product_repository_tests_" + std::to_string(std::chrono::steady_clock::now().time_since_epoch().count()))) {
        fs::create_directories(path_);
    }

    ~TempDirectory() {
        std::error_code errorCode;
        fs::remove_all(path_, errorCode);
    }

    const fs::path& path() const {
        return path_;
    }

private:
    fs::path path_;
};

void assertTrue(bool condition, const std::string& message) {
    if (!condition) {
        throw std::runtime_error(message);
    }
}

void assertDoubleEquals(double actual, double expected, const std::string& message) {
    if (std::fabs(actual - expected) > 1e-9) {
        throw std::runtime_error(message);
    }
}

void testLoadCreatesSampleProductsWhenFileMissing() {
    TempDirectory tempDirectory;
    const auto filePath = tempDirectory.path() / "products.csv";

    ProductRepository repository(filePath.string());
    const auto products = repository.load();

    assertTrue(products.size() >= 5, "Expected repository to create at least five sample products.");
    assertTrue(fs::exists(filePath), "Expected repository to create products.csv when missing.");
    assertTrue(products.front().id == "P001", "Expected first sample product id to be P001.");
}

void testSaveAndLoadRoundTrip() {
    TempDirectory tempDirectory;
    const auto filePath = tempDirectory.path() / "products.csv";

    ProductRepository repository(filePath.string());
    const std::vector<Product> expectedProducts = {
        {"SKU-001", "矿泉水", "饮料", 2.50, 30, "active"},
        {"SKU-002", "笔记本", "文具", 12.00, 15, "inactive"},
    };

    repository.save(expectedProducts);
    const auto loadedProducts = repository.load();

    assertTrue(loadedProducts.size() == expectedProducts.size(), "Expected loaded product count to match saved product count.");
    assertTrue(loadedProducts[0].id == expectedProducts[0].id, "Expected first product id to match.");
    assertTrue(loadedProducts[0].name == expectedProducts[0].name, "Expected first product name to match.");
    assertDoubleEquals(loadedProducts[0].price, expectedProducts[0].price, "Expected first product price to match.");
    assertTrue(loadedProducts[1].stock == expectedProducts[1].stock, "Expected second product stock to match.");
    assertTrue(loadedProducts[1].status == expectedProducts[1].status, "Expected second product status to match.");
}

int main() {
    try {
        testLoadCreatesSampleProductsWhenFileMissing();
        testSaveAndLoadRoundTrip();
        std::cout << "All ProductRepository tests passed." << std::endl;
        return 0;
    } catch (const std::exception& exception) {
        std::cerr << exception.what() << std::endl;
        return 1;
    }
}
