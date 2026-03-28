#pragma once

#include "services/EmployeeManagementService.h"

/**
 * 员工管理 UI，提供新增员工、重置密码和删除员工三个子功能。
 */
class EmployeeManagementUI {
public:
    EmployeeManagementUI(EmployeeManagementService service,
                         std::string currentOperatorId);

    void run();

private:
    EmployeeManagementService service_;
    std::string currentOperatorId_;

    void handleListEmployees();
    void handleAddEmployee();
    void handleResetPassword();
    void handleDeleteEmployee();
};
