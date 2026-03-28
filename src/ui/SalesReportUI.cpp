#include "ui/SalesReportUI.h"

#include <iomanip>
#include <iostream>
#include <string>

SalesReportUI::SalesReportUI(SalesReportService service)
    : service_(std::move(service)) {}

void SalesReportUI::showReport(const SalesReport& report) const {
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "\n--- 销售汇总 ---" << std::endl;
    std::cout << "  今日销售额：" << report.todayRevenue << " 元" << std::endl;
    std::cout << "  累计销售额：" << report.totalRevenue << " 元" << std::endl;
    std::cout << "  订单总数：  " << report.orderCount << " 笔" << std::endl;

    std::cout << "\n--- 销量 Top 5 商品 ---" << std::endl;
    if (report.top5Products.empty()) {
        std::cout << "  （暂无数据）" << std::endl;
    } else {
        std::cout << std::left
                  << std::setw(8)  << "编号"
                  << std::setw(16) << "名称"
                  << std::setw(10) << "销量"
                  << "销售额" << std::endl;
        std::cout << std::string(46, '-') << std::endl;
        for (const auto& p : report.top5Products) {
            std::cout << std::left
                      << std::setw(8)  << p.productId
                      << std::setw(16) << p.productName
                      << std::setw(10) << p.totalQuantity
                      << p.totalRevenue << std::endl;
        }
    }

    std::cout << "\n--- 按分类汇总销售额 ---" << std::endl;
    if (report.revenueByCategory.empty()) {
        std::cout << "  （暂无数据）" << std::endl;
    } else {
        for (const auto& kv : report.revenueByCategory) {
            std::cout << "  " << std::left << std::setw(14) << kv.first
                      << kv.second << " 元" << std::endl;
        }
    }
}

void SalesReportUI::handleFullReport() {
    SalesReport report = service_.generate();
    showReport(report);
}

void SalesReportUI::handleFilteredReport() {
    std::cout << "请输入开始日期（YYYY-MM-DD）：";
    std::string start;
    std::getline(std::cin, start);

    std::cout << "请输入结束日期（YYYY-MM-DD）：";
    std::string end;
    std::getline(std::cin, end);

    if (start.size() != 10 || end.size() != 10) {
        std::cout << "日期格式错误，请使用 YYYY-MM-DD 格式。" << std::endl;
        return;
    }
    if (start > end) {
        std::cout << "开始日期不能晚于结束日期。" << std::endl;
        return;
    }

    std::cout << "\n报表范围：" << start << " 至 " << end << std::endl;
    SalesReport report = service_.generate(start, end);
    showReport(report);
}

void SalesReportUI::run() {
    while (true) {
        std::cout << "\n============================" << std::endl;
        std::cout << "        销售报表" << std::endl;
        std::cout << "============================" << std::endl;
        std::cout << "  1. 查看全局报表" << std::endl;
        std::cout << "  2. 按日期范围查询" << std::endl;
        std::cout << "  0. 返回上级菜单" << std::endl;
        std::cout << "============================" << std::endl;
        std::cout << "请输入菜单编号：";

        std::string line;
        if (!std::getline(std::cin, line)) break;
        int choice = -1;
        try { choice = std::stoi(line); } catch (...) {}

        switch (choice) {
            case 1: handleFullReport();     break;
            case 2: handleFilteredReport(); break;
            case 0: return;
            default: std::cout << "无效输入，请重新选择。" << std::endl; break;
        }
    }
}
