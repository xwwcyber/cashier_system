#include "repositories/OrderRepository.h"

#include <filesystem>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <utility>

// CSV 主字段分隔符为逗号；items 字段内部使用 '|' 分隔各明细行，'^' 分隔明细子字段
static const char ITEM_SEP = '|';
static const char SUBFIELD_SEP = '^';

namespace {
std::vector<std::string> splitBy(const std::string& s, char delim) {
    std::vector<std::string> parts;
    std::stringstream ss(s);
    std::string part;
    while (std::getline(ss, part, delim)) {
        parts.push_back(part);
    }
    return parts;
}
}  // namespace

OrderRepository::OrderRepository(std::string filePath) : filePath_(std::move(filePath)) {}

std::string OrderRepository::serializeItems(const std::vector<OrderItem>& items) const {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2);
    for (size_t i = 0; i < items.size(); ++i) {
        if (i > 0) oss << ITEM_SEP;
        const auto& it = items[i];
        oss << it.productId << SUBFIELD_SEP
            << it.productName << SUBFIELD_SEP
            << it.price << SUBFIELD_SEP
            << it.quantity << SUBFIELD_SEP
            << it.subtotal;
    }
    return oss.str();
}

std::vector<OrderItem> OrderRepository::deserializeItems(const std::string& field) const {
    std::vector<OrderItem> items;
    if (field.empty()) return items;

    for (const auto& part : splitBy(field, ITEM_SEP)) {
        auto sub = splitBy(part, SUBFIELD_SEP);
        if (sub.size() != 5) {
            throw std::runtime_error("Invalid order item encoding: " + part);
        }
        OrderItem it;
        it.productId   = sub[0];
        it.productName = sub[1];
        it.price       = std::stod(sub[2]);
        it.quantity    = std::stoi(sub[3]);
        it.subtotal    = std::stod(sub[4]);
        items.push_back(std::move(it));
    }
    return items;
}

void OrderRepository::append(const Order& order) const {
    const std::filesystem::path filePath(filePath_);
    if (filePath.has_parent_path()) {
        std::filesystem::create_directories(filePath.parent_path());
    }

    const bool needHeader = !std::filesystem::exists(filePath_);
    std::ofstream output(filePath_, std::ios::app);
    if (!output.is_open()) {
        throw std::runtime_error("Unable to open orders file: " + filePath_);
    }

    if (needHeader) {
        output << "orderId,timestamp,cashierId,items,originTotal,"
                  "discountAmount,payableAmount,paidAmount,changeAmount\n";
    }

    output << std::fixed << std::setprecision(2);
    output << order.orderId << ','
           << order.timestamp << ','
           << order.cashierId << ','
           << serializeItems(order.items) << ','
           << order.originTotal << ','
           << order.discountAmount << ','
           << order.payableAmount << ','
           << order.paidAmount << ','
           << order.changeAmount << '\n';
}

std::vector<Order> OrderRepository::loadAll() const {
    std::ifstream input(filePath_);
    if (!input.is_open()) {
        return {};
    }

    std::vector<Order> orders;
    std::string line;

    // 跳过表头
    std::getline(input, line);

    while (std::getline(input, line)) {
        if (line.empty()) continue;

        // 手动拆分：前 3 个字段和后 5 个字段用逗号分隔，items 字段可能含 '^' 和 '|'
        // 格式固定：col0,col1,col2,<items>,col4,col5,col6,col7,col8
        // 找到前 3 个逗号和后 5 个逗号（从末尾数）
        std::vector<size_t> commaPos;
        for (size_t i = 0; i < line.size(); ++i) {
            if (line[i] == ',') commaPos.push_back(i);
        }
        // 至少需要 8 个逗号（9 列）
        if (commaPos.size() < 8) {
            throw std::runtime_error("Invalid order row: " + line);
        }

        size_t c0 = commaPos[0];
        size_t c1 = commaPos[1];
        size_t c2 = commaPos[2];
        // items 字段结束于倒数第5个逗号
        size_t c3 = commaPos[commaPos.size() - 5];
        size_t c4 = commaPos[commaPos.size() - 4];
        size_t c5 = commaPos[commaPos.size() - 3];
        size_t c6 = commaPos[commaPos.size() - 2];
        size_t c7 = commaPos[commaPos.size() - 1];

        Order o;
        o.orderId        = line.substr(0, c0);
        o.timestamp      = line.substr(c0 + 1, c1 - c0 - 1);
        o.cashierId      = line.substr(c1 + 1, c2 - c1 - 1);
        o.items          = deserializeItems(line.substr(c2 + 1, c3 - c2 - 1));
        o.originTotal    = std::stod(line.substr(c3 + 1, c4 - c3 - 1));
        o.discountAmount = std::stod(line.substr(c4 + 1, c5 - c4 - 1));
        o.payableAmount  = std::stod(line.substr(c5 + 1, c6 - c5 - 1));
        o.paidAmount     = std::stod(line.substr(c6 + 1, c7 - c6 - 1));
        o.changeAmount   = std::stod(line.substr(c7 + 1));
        orders.push_back(std::move(o));
    }
    return orders;
}

std::vector<Order> OrderRepository::findById(const std::string& orderId) const {
    std::vector<Order> result;
    for (auto& o : loadAll()) {
        if (o.orderId == orderId) result.push_back(o);
    }
    return result;
}

std::vector<Order> OrderRepository::findByDateRange(const std::string& startDate,
                                                     const std::string& endDate) const {
    std::vector<Order> result;
    for (auto& o : loadAll()) {
        // timestamp 格式 "YYYY-MM-DD HH:MM:SS"，取前 10 位做字符串比较
        std::string date = o.timestamp.size() >= 10 ? o.timestamp.substr(0, 10) : o.timestamp;
        if (date >= startDate && date <= endDate) {
            result.push_back(o);
        }
    }
    return result;
}
