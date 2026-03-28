#include "services/ProductService.h"

#include <algorithm>
#include <cctype>
#include <stdexcept>
#include <utility>

ProductService::ProductService(ProductRepository repository)
    : repository_(std::move(repository)), products_(repository_.load()) {}

std::vector<Product> ProductService::getAll() const {
    return products_;
}

std::vector<Product> ProductService::searchByName(const std::string& keyword) const {
    std::string lower_kw = keyword;
    std::transform(lower_kw.begin(), lower_kw.end(), lower_kw.begin(), ::tolower);

    std::vector<Product> result;
    for (const auto& p : products_) {
        std::string lower_name = p.name;
        std::transform(lower_name.begin(), lower_name.end(), lower_name.begin(), ::tolower);
        if (lower_name.find(lower_kw) != std::string::npos) {
            result.push_back(p);
        }
    }
    return result;
}

std::vector<Product> ProductService::filterByCategory(const std::string& category) const {
    std::vector<Product> result;
    for (const auto& p : products_) {
        if (p.category == category) {
            result.push_back(p);
        }
    }
    return result;
}

std::optional<Product> ProductService::findById(const std::string& id) const {
    for (const auto& p : products_) {
        if (p.id == id) return p;
    }
    return std::nullopt;
}

void ProductService::add(const Product& product) {
    if (findById(product.id).has_value()) {
        throw std::invalid_argument("商品编号已存在: " + product.id);
    }
    if (product.price <= 0.0) {
        throw std::invalid_argument("价格必须大于 0");
    }
    if (product.stock < 0) {
        throw std::invalid_argument("库存不能小于 0");
    }
    products_.push_back(product);
    persist();
}

void ProductService::update(const Product& product) {
    if (product.price <= 0.0) {
        throw std::invalid_argument("价格必须大于 0");
    }
    if (product.stock < 0) {
        throw std::invalid_argument("库存不能小于 0");
    }
    for (auto& p : products_) {
        if (p.id == product.id) {
            p = product;
            persist();
            return;
        }
    }
    throw std::invalid_argument("商品编号不存在: " + product.id);
}

void ProductService::remove(const std::string& id) {
    auto it = std::find_if(products_.begin(), products_.end(),
                           [&id](const Product& p) { return p.id == id; });
    if (it == products_.end()) {
        throw std::invalid_argument("商品编号不存在: " + id);
    }
    products_.erase(it);
    persist();
}

void ProductService::persist() {
    repository_.save(products_);
}
