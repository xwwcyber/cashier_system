#include "repositories/CouponRepository.h"

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

CouponRepository::CouponRepository(std::string filePath) : filePath_(std::move(filePath)) {}

std::vector<Coupon> CouponRepository::load() const {
    std::ifstream input(filePath_);
    if (!input.is_open()) {
        auto sampleCoupons = buildSampleCoupons();
        save(sampleCoupons);
        return sampleCoupons;
    }

    std::vector<Coupon> coupons;
    std::string line;

    // 跳过表头
    if (std::getline(input, line)) {
        if (line != "code,type,value,minAmount,used,enabled") {
            // 首行不是表头，视为数据行
            auto fields = splitCsvLine(line);
            if (fields.size() != 6) {
                throw std::runtime_error("Invalid coupons header: " + filePath_);
            }
            coupons.push_back(Coupon{
                fields[0],
                fields[1],
                std::stod(fields[2]),
                std::stod(fields[3]),
                fields[4] == "1",
                fields[5] == "1",
            });
        }
    }

    while (std::getline(input, line)) {
        if (line.empty()) {
            continue;
        }

        auto fields = splitCsvLine(line);
        if (fields.size() != 6) {
            throw std::runtime_error("Invalid coupon row: " + line);
        }

        coupons.push_back(Coupon{
            fields[0],
            fields[1],
            std::stod(fields[2]),
            std::stod(fields[3]),
            fields[4] == "1",
            fields[5] == "1",
        });
    }

    return coupons;
}

void CouponRepository::save(const std::vector<Coupon>& coupons) const {
    const std::filesystem::path filePath(filePath_);
    if (filePath.has_parent_path()) {
        std::filesystem::create_directories(filePath.parent_path());
    }

    std::ofstream output(filePath_);
    if (!output.is_open()) {
        throw std::runtime_error("Unable to open coupons file: " + filePath_);
    }

    output << "code,type,value,minAmount,used,enabled\n";
    output << std::fixed << std::setprecision(2);

    for (const auto& coupon : coupons) {
        output << coupon.code << ','
               << coupon.type << ','
               << coupon.value << ','
               << coupon.minAmount << ','
               << (coupon.used ? '1' : '0') << ','
               << (coupon.enabled ? '1' : '0') << '\n';
    }
}

std::vector<Coupon> CouponRepository::buildSampleCoupons() const {
    return {
        {"SAVE10",  "fixed",   10.0,  50.0,  false, true},
        {"OFF20",   "percent", 20.0, 100.0,  false, true},
        {"VIP5",    "fixed",    5.0,   0.0,  false, true},
    };
}
