#pragma once

#include <optional>
#include <string>

#include "models/Employee.h"
#include "repositories/EmployeeRepository.h"

/**
 * 登录鉴权服务，负责员工身份验证与会话状态管理。
 * 同一实例在整个程序生命周期内持有当前登录员工信息。
 */
class AuthService {
public:
    explicit AuthService(EmployeeRepository employeeRepository);

    /**
     * 尝试使用工号和明文密码登录。
     * 连续失败次数达到 maxAttempts 后返回 false 并锁定（需调用 resetFailedAttempts）。
     * @return true 表示登录成功，false 表示失败。
     */
    bool login(const std::string& employeeId, const std::string& password);

    /** 退出当前登录，清除会话状态与失败计数。 */
    void logout();

    /** 当前是否有员工已登录。 */
    bool isLoggedIn() const;

    /** 返回当前登录员工（调用前须确认 isLoggedIn() == true）。 */
    const Employee& getCurrentEmployee() const;

    /** 连续登录失败次数（用于测试及界面判断）。 */
    int getFailedAttempts() const;

    /** 最大允许的连续失败次数，超过后需重置才能继续尝试。 */
    static constexpr int maxAttempts = 3;

private:
    EmployeeRepository employeeRepository_;
    std::optional<Employee> currentEmployee_;
    int failedAttempts_ = 0;
};
