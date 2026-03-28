#include "repositories/ProductRepository.h"

#include <filesystem>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <utility>

namespace {
std::vector<std::string> splitCsvLine(const std::string& line) {
    std::vector<std::string> fields;
    std::stringstream stream(line);
    std::string field;

    while (std::getline(stream, field, ',')) {
        fields.push_back(field);
    }

    return fields;
}
}  // namespace

ProductRepository::ProductRepository(std::string filePath) : filePath_(std::move(filePath)) {}

std::vector<Product> ProductRepository::load() const {
    std::ifstream input(filePath_);
    if (!input.is_open()) {
        auto sampleProducts = buildSampleProducts();
        save(sampleProducts);
        return sampleProducts;
    }

    std::vector<Product> products;
    std::string line;

    if (std::getline(input, line)) {
        if (line != "id,name,category,price,stock,status") {
            auto headerFields = splitCsvLine(line);
            if (headerFields.size() != 6) {
                throw std::runtime_error("Invalid products header: " + filePath_);
            }

            products.push_back(Product{
                headerFields[0],
                headerFields[1],
                headerFields[2],
                std::stod(headerFields[3]),
                std::stoi(headerFields[4]),
                headerFields[5],
            });
        }
    }

    while (std::getline(input, line)) {
        if (line.empty()) {
            continue;
        }

        auto fields = splitCsvLine(line);
        if (fields.size() != 6) {
            throw std::runtime_error("Invalid product row: " + line);
        }

        products.push_back(Product{
            fields[0],
            fields[1],
            fields[2],
            std::stod(fields[3]),
            std::stoi(fields[4]),
            fields[5],
        });
    }

    return products;
}

void ProductRepository::save(const std::vector<Product>& products) const {
    const std::filesystem::path filePath(filePath_);
    if (filePath.has_parent_path()) {
        std::filesystem::create_directories(filePath.parent_path());
    }

    std::ofstream output(filePath_);
    if (!output.is_open()) {
        throw std::runtime_error("Unable to open products file: " + filePath_);
    }

    output << "id,name,category,price,stock,status\n";
    output << std::fixed << std::setprecision(2);

    for (const auto& product : products) {
        output << product.id << ','
               << product.name << ','
               << product.category << ','
               << product.price << ','
               << product.stock << ','
               << product.status << '\n';
    }
}

std::vector<Product> ProductRepository::buildSampleProducts() const {
    return {
        {"P001", "可口可乐", "饮料", 3.50, 120, "active"},
        {"P002", "纯牛奶", "乳品", 6.80, 80, "active"},
        {"P003", "薯片", "零食", 7.50, 60, "active"},
        {"P004", "洗洁精", "日用品", 12.90, 45, "active"},
        {"P005", "抽纸", "日用品", 9.90, 100, "active"},
    };
}
