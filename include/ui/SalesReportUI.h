#pragma once

#include "services/SalesReportService.h"

/**
 * 销售报表 UI，提供全局报表与按日期筛选报表两种视图。
 */
class SalesReportUI {
public:
    explicit SalesReportUI(SalesReportService service);

    void run();

private:
    SalesReportService service_;

    void showReport(const SalesReport& report) const;
    void handleFullReport();
    void handleFilteredReport();
};
