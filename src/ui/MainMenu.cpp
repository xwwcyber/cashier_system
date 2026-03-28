#include "ui/MainMenu.h"

#include "repositories/CouponRepository.h"
#include "repositories/OrderRepository.h"
#include "repositories/ProductRepository.h"
#include "services/CartService.h"
#include "services/CheckoutService.h"
#include "services/InventoryService.h"
#include "services/ProductService.h"
#include "services/SalesReportService.h"
#include "ui/InventoryManagementUI.h"
#include "ui/ProductManagementUI.h"
#include "ui/SalesReportUI.h"

#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

namespace {
void showProducts(const std::vector<Product>& products) {
    if (products.empty()) {
        std::cout << "（无商品）" << std::endl;
        return;
    }

    std::cout << std::left
              << std::setw(8) << "编号"
              << std::setw(16) << "名称"
              << std::setw(10) << "分类"
              << std::setw(10) << "价格"
              << std::setw(8) << "库存"
              << "状态" << std::endl;
    std::cout << std::string(60, '-') << std::endl;
    for (const auto& product : products) {
        std::cout << std::left
                  << std::setw(8) << product.id
                  << std::setw(16) << product.name
                  << std::setw(10) << product.category
                  << std::setw(10) << std::fixed << std::setprecision(2) << product.price
                  << std::setw(8) << product.stock
                  << product.status << std::endl;
    }
}

void showCart(const CartService& cart) {
    auto items = cart.getItems();
    if (items.empty()) {
        std::cout << "（购物车为空）" << std::endl;
        return;
    }

    std::cout << std::left
              << std::setw(8) << "编号"
              << std::setw(16) << "名称"
              << std::setw(10) << "单价"
              << std::setw(8) << "数量"
              << "小计" << std::endl;
    std::cout << std::string(54, '-') << std::endl;
    for (const auto& item : items) {
        std::cout << std::left
                  << std::setw(8) << item.productId
                  << std::setw(16) << item.productName
                  << std::setw(10) << std::fixed << std::setprecision(2) << item.price
                  << std::setw(8) << item.quantity
                  << item.subtotal << std::endl;
    }
    std::cout << std::string(54, '-') << std::endl;
    std::cout << "总件数：" << cart.getTotalQuantity()
              << "，总金额：" << std::fixed << std::setprecision(2) << cart.getTotalAmount()
              << std::endl;
}

int parseChoice(const std::string& line) {
    try {
        return std::stoi(line);
    } catch (...) {
        return -1;
    }
}
}

MainMenu::MainMenu(AuthService& auth) : auth_(auth) {}

void MainMenu::run() {
    while (auth_.isLoggedIn()) {
        const auto& emp = auth_.getCurrentEmployee();
        if (emp.role == "admin") {
            if (!showAdminMenu()) {
                break;
            }
        } else {
            if (!showCashierMenu()) {
                break;
            }
        }
    }
}

bool MainMenu::showAdminMenu() {
    const auto& emp = auth_.getCurrentEmployee();
    ProductRepository repository("data/products.csv");
    InventoryService inventoryService(repository);
    const auto lowStockCount = inventoryService.getLowStockProducts().size();

    std::cout << "\n============================" << std::endl;
    std::cout << "  管理员菜单 [" << emp.name << "]" << std::endl;
    std::cout << "  低库存商品：" << lowStockCount
              << " 件（阈值：" << inventoryService.getLowStockThreshold() << "）" << std::endl;
    std::cout << "============================" << std::endl;
    std::cout << "  1. 开始收银" << std::endl;
    std::cout << "  2. 商品管理" << std::endl;
    std::cout << "  3. 库存管理" << std::endl;
    std::cout << "  4. 优惠券管理" << std::endl;
    std::cout << "  5. 员工管理" << std::endl;
    std::cout << "  6. 销售报表" << std::endl;
    std::cout << "  0. 退出登录" << std::endl;
    std::cout << "============================" << std::endl;
    std::cout << "请输入菜单编号：";

    std::string line;
    if (!std::getline(std::cin, line)) {
        return false;
    }

    switch (parseChoice(line)) {
        case 1: handleStartCheckout();       break;
        case 2: handleProductManagement();   break;
        case 3: handleInventoryManagement(); break;
        case 4: handleCouponManagement();    break;
        case 5: handleEmployeeManagement();  break;
        case 6: handleSalesReport();         break;
        case 0:
            auth_.logout();
            std::cout << "已退出登录。" << std::endl;
            return false;
        default:
            std::cout << "无效菜单编号，请重新输入。" << std::endl;
            break;
    }
    return auth_.isLoggedIn();
}

bool MainMenu::showCashierMenu() {
    const auto& emp = auth_.getCurrentEmployee();
    std::cout << "\n============================" << std::endl;
    std::cout << "  收银员菜单 [" << emp.name << "]" << std::endl;
    std::cout << "============================" << std::endl;
    std::cout << "  1. 开始收银" << std::endl;
    std::cout << "  2. 浏览商品" << std::endl;
    std::cout << "  3. 查询订单" << std::endl;
    std::cout << "  0. 退出登录" << std::endl;
    std::cout << "============================" << std::endl;
    std::cout << "请输入菜单编号：";

    std::string line;
    if (!std::getline(std::cin, line)) {
        return false;
    }

    switch (parseChoice(line)) {
        case 1: handleStartCheckout();  break;
        case 2: handleBrowseProducts(); break;
        case 3: handleQueryOrders();    break;
        case 0:
            auth_.logout();
            std::cout << "已退出登录。" << std::endl;
            return false;
        default:
            std::cout << "无效菜单编号，请重新输入。" << std::endl;
            break;
    }
    return auth_.isLoggedIn();
}

void MainMenu::handleStartCheckout() {
    ProductRepository repository("data/products.csv");
    ProductService productService(repository);
    CartService cartService(productService);
    CouponRepository couponRepository("data/coupons.csv");
    OrderRepository orderRepository("data/orders.csv");
    CheckoutService checkoutService(productService, couponRepository, orderRepository);

    while (true) {
        std::cout << "\n============================" << std::endl;
        std::cout << "        购物车" << std::endl;
        std::cout << "============================" << std::endl;
        std::cout << "  1. 查看在售商品" << std::endl;
        std::cout << "  2. 加入购物车" << std::endl;
        std::cout << "  3. 查看购物车" << std::endl;
        std::cout << "  4. 修改商品数量" << std::endl;
        std::cout << "  5. 移除商品" << std::endl;
        std::cout << "  6. 结账" << std::endl;
        std::cout << "  0. 返回上级菜单" << std::endl;
        std::cout << "============================" << std::endl;
        std::cout << "请输入菜单编号：";

        std::string line;
        if (!std::getline(std::cin, line)) {
            return;
        }

        switch (parseChoice(line)) {
            case 1: {
                std::vector<Product> activeProducts;
                for (const auto& product : productService.getAll()) {
                    if (product.status == "active") {
                        activeProducts.push_back(product);
                    }
                }
                showProducts(activeProducts);
                break;
            }
            case 2: {
                std::string productId;
                std::string quantityText;
                std::cout << "商品编号：";
                std::getline(std::cin, productId);
                std::cout << "购买数量：";
                std::getline(std::cin, quantityText);

                try {
                    cartService.addItem(productId, std::stoi(quantityText));
                    std::cout << "已加入购物车。" << std::endl;
                    showCart(cartService);
                } catch (const std::exception& e) {
                    std::cout << "加入失败：" << e.what() << std::endl;
                }
                break;
            }
            case 3:
                showCart(cartService);
                break;
            case 4: {
                std::string productId;
                std::string quantityText;
                std::cout << "商品编号：";
                std::getline(std::cin, productId);
                std::cout << "新数量：";
                std::getline(std::cin, quantityText);

                try {
                    cartService.updateItemQuantity(productId, std::stoi(quantityText));
                    std::cout << "购物车已更新。" << std::endl;
                    showCart(cartService);
                } catch (const std::exception& e) {
                    std::cout << "更新失败：" << e.what() << std::endl;
                }
                break;
            }
            case 5: {
                std::string productId;
                std::cout << "商品编号：";
                std::getline(std::cin, productId);

                try {
                    cartService.removeItem(productId);
                    std::cout << "商品已移除。" << std::endl;
                    showCart(cartService);
                } catch (const std::exception& e) {
                    std::cout << "移除失败：" << e.what() << std::endl;
                }
                break;
            }
            case 6: {
                if (cartService.empty()) {
                    std::cout << "购物车为空，无法结账。" << std::endl;
                    break;
                }

                std::string couponCode;
                std::string paidAmountText;
                std::cout << "优惠券码（直接回车表示不使用）：";
                std::getline(std::cin, couponCode);
                std::cout << "实付金额：";
                std::getline(std::cin, paidAmountText);

                try {
                    Order order = checkoutService.checkout(
                        cartService.getItems(),
                        auth_.getCurrentEmployee().id,
                        couponCode,
                        std::stod(paidAmountText));
                    std::cout << "结账成功。订单号：" << order.orderId << std::endl;
                    std::cout << "原价：" << std::fixed << std::setprecision(2) << order.originTotal
                              << "，优惠：" << order.discountAmount
                              << "，应付：" << order.payableAmount
                              << "，实付：" << order.paidAmount
                              << "，找零：" << order.changeAmount << std::endl;
                    cartService.clear();
                } catch (const std::exception& e) {
                    std::cout << "结账失败：" << e.what() << std::endl;
                }
                break;
            }
            case 0:
                return;
            default:
                std::cout << "无效菜单编号，请重新输入。" << std::endl;
                break;
        }
    }
}

void MainMenu::handleProductManagement() {
    ProductRepository repository("data/products.csv");
    ProductService productService(repository);
    ProductManagementUI ui(productService);
    ui.run();
}

void MainMenu::handleInventoryManagement() {
    ProductRepository repository("data/products.csv");
    InventoryService inventoryService(repository);
    InventoryManagementUI ui(inventoryService);
    ui.run();
}

void MainMenu::handleCouponManagement() {
    std::cout << "[优惠券管理] 功能即将推出。按 Enter 返回。" << std::endl;
    std::string dummy;
    std::getline(std::cin, dummy);
}

void MainMenu::handleEmployeeManagement() {
    std::cout << "[员工管理] 功能即将推出。按 Enter 返回。" << std::endl;
    std::string dummy;
    std::getline(std::cin, dummy);
}

void MainMenu::handleSalesReport() {
    OrderRepository orderRepository("data/orders.csv");
    ProductRepository productRepository("data/products.csv");
    SalesReportService service(orderRepository, productRepository);
    SalesReportUI ui(std::move(service));
    ui.run();
}

void MainMenu::handleBrowseProducts() {
    ProductRepository repository("data/products.csv");
    ProductService productService(repository);

    std::vector<Product> activeProducts;
    for (const auto& product : productService.getAll()) {
        if (product.status == "active") {
            activeProducts.push_back(product);
        }
    }

    showProducts(activeProducts);
}

void MainMenu::handleQueryOrders() {
    std::cout << "[查询订单] 功能即将推出。按 Enter 返回。" << std::endl;
    std::string dummy;
    std::getline(std::cin, dummy);
}
