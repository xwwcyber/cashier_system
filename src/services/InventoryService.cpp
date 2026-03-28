#include "services/InventoryService.h"

#include <algorithm>
#include <cctype>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <stdexcept>
#include <utility>

namespace {
int parsePositiveThreshold(const std::string& text) {
    try {
        int value = std::stoi(text);
        return value > 0 ? value : 10;
    } catch (...) {
        return 10;
    }
}

int loadThresholdFromTextFile(const std::filesystem::path& path) {
    std::ifstream file(path);
    std::string line;
    while (std::getline(file, line)) {
        auto eq = line.find('=');
        if (eq == std::string::npos) {
            continue;
        }
        if (line.substr(0, eq) == "low_stock_threshold") {
            return parsePositiveThreshold(line.substr(eq + 1));
        }
    }
    return 10;
}

int loadThresholdFromJsonFile(const std::filesystem::path& path) {
    std::ifstream file(path);
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    const std::string key = "\"low_stock_threshold\"";
    auto keyPos = content.find(key);
    if (keyPos == std::string::npos) {
        return 10;
    }

    auto colonPos = content.find(':', keyPos + key.size());
    if (colonPos == std::string::npos) {
        return 10;
    }

    size_t valueStart = colonPos + 1;
    while (valueStart < content.size() && std::isspace(static_cast<unsigned char>(content[valueStart]))) {
        ++valueStart;
    }

    size_t valueEnd = valueStart;
    if (valueEnd < content.size() && (content[valueEnd] == '-' || std::isdigit(static_cast<unsigned char>(content[valueEnd])))) {
        ++valueEnd;
        while (valueEnd < content.size() && std::isdigit(static_cast<unsigned char>(content[valueEnd]))) {
            ++valueEnd;
        }
        return parsePositiveThreshold(content.substr(valueStart, valueEnd - valueStart));
    }

    return 10;
}
}  // namespace

InventoryService::InventoryService(ProductRepository repository, std::string configPath)
    : repository_(std::move(repository)),
      configPath_(std::move(configPath)),
      products_(repository_.load()),
      threshold_(loadThreshold()) {}

int InventoryService::loadThreshold() const {
    const std::filesystem::path textPath(configPath_);
    if (std::filesystem::exists(textPath)) {
        return loadThresholdFromTextFile(textPath);
    }

    const std::filesystem::path jsonPath = textPath.parent_path() / "config.json";
    if (std::filesystem::exists(jsonPath)) {
        return loadThresholdFromJsonFile(jsonPath);
    }

    return 10;
}

int InventoryService::getLowStockThreshold() const {
    return threshold_;
}

std::vector<Product> InventoryService::getAllSortedByStock() const {
    std::vector<Product> sorted = products_;
    std::sort(sorted.begin(), sorted.end(),
              [](const Product& a, const Product& b) { return a.stock < b.stock; });
    return sorted;
}

std::vector<Product> InventoryService::getLowStockProducts() const {
    std::vector<Product> result;
    for (const auto& p : products_) {
        if (p.stock < threshold_) {
            result.push_back(p);
        }
    }
    std::sort(result.begin(), result.end(),
              [](const Product& a, const Product& b) { return a.stock < b.stock; });
    return result;
}

void InventoryService::restock(const std::string& productId, int quantity) {
    if (quantity <= 0) {
        throw std::invalid_argument("补货数量必须大于 0");
    }
    for (auto& p : products_) {
        if (p.id == productId) {
            p.stock += quantity;
            persist();
            return;
        }
    }
    throw std::invalid_argument("商品编号不存在: " + productId);
}

void InventoryService::persist() {
    repository_.save(products_);
}
