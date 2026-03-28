#include "services/CartService.h"

#include <algorithm>
#include <numeric>
#include <stdexcept>

namespace {
void validatePositiveQuantity(int quantity) {
    if (quantity <= 0) {
        throw std::invalid_argument("商品数量必须大于 0");
    }
}

Product requireAvailableProduct(ProductService& productService, const std::string& productId, int quantity) {
    validatePositiveQuantity(quantity);

    auto product = productService.findById(productId);
    if (!product.has_value()) {
        throw std::invalid_argument("商品编号不存在: " + productId);
    }
    if (product->status != "active") {
        throw std::invalid_argument("商品已下架: " + productId);
    }
    if (product->stock < quantity) {
        throw std::invalid_argument("商品库存不足: " + productId);
    }
    return product.value();
}

OrderItem buildOrderItem(const Product& product, int quantity) {
    return OrderItem{product.id, product.name, product.price, quantity, product.price * quantity};
}
}  // namespace

CartService::CartService(ProductService& productService) : productService_(productService) {}

std::vector<OrderItem> CartService::getItems() const {
    return items_;
}

std::optional<OrderItem> CartService::findItem(const std::string& productId) const {
    auto it = std::find_if(items_.begin(), items_.end(),
                           [&productId](const OrderItem& item) { return item.productId == productId; });
    if (it == items_.end()) {
        return std::nullopt;
    }
    return *it;
}

void CartService::addItem(const std::string& productId, int quantity) {
    validatePositiveQuantity(quantity);

    auto it = std::find_if(items_.begin(), items_.end(),
                           [&productId](const OrderItem& item) { return item.productId == productId; });
    const int existingQuantity = (it == items_.end()) ? 0 : it->quantity;
    Product product = requireAvailableProduct(productService_, productId, existingQuantity + quantity);

    if (it == items_.end()) {
        items_.push_back(buildOrderItem(product, quantity));
        return;
    }

    it->productName = product.name;
    it->price = product.price;
    it->quantity += quantity;
    it->subtotal = it->price * it->quantity;
}

void CartService::updateItemQuantity(const std::string& productId, int quantity) {
    auto it = std::find_if(items_.begin(), items_.end(),
                           [&productId](const OrderItem& item) { return item.productId == productId; });
    if (it == items_.end()) {
        throw std::invalid_argument("购物车中不存在该商品: " + productId);
    }

    Product product = requireAvailableProduct(productService_, productId, quantity);
    it->productName = product.name;
    it->price = product.price;
    it->quantity = quantity;
    it->subtotal = it->price * it->quantity;
}

void CartService::removeItem(const std::string& productId) {
    auto it = std::find_if(items_.begin(), items_.end(),
                           [&productId](const OrderItem& item) { return item.productId == productId; });
    if (it == items_.end()) {
        throw std::invalid_argument("购物车中不存在该商品: " + productId);
    }
    items_.erase(it);
}

void CartService::clear() {
    items_.clear();
}

int CartService::getTotalQuantity() const {
    return std::accumulate(items_.begin(), items_.end(), 0,
                           [](int total, const OrderItem& item) { return total + item.quantity; });
}

double CartService::getTotalAmount() const {
    return std::accumulate(items_.begin(), items_.end(), 0.0,
                           [](double total, const OrderItem& item) { return total + item.subtotal; });
}

bool CartService::empty() const {
    return items_.empty();
}
