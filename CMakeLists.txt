cmake_minimum_required(VERSION 3.16)

if(NOT CMAKE_CXX_COMPILER AND WIN32)
    find_program(
        CASHIER_GXX
        NAMES g++.exe g++
        PATHS
            "E:/vscode/mingw64/bin"
            "C:/msys64/ucrt64/bin"
            "C:/msys64/mingw64/bin"
        NO_DEFAULT_PATH
    )

    if(NOT CASHIER_GXX)
        find_program(CASHIER_GXX NAMES g++.exe g++)
    endif()

    if(CASHIER_GXX)
        set(CMAKE_CXX_COMPILER "${CASHIER_GXX}" CACHE FILEPATH "C++ compiler" FORCE)
    endif()
endif()

project(CashierSystem LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

# Main executable
add_executable(
    cashier
    src/main.cpp
    src/repositories/ProductRepository.cpp
    src/repositories/EmployeeRepository.cpp
    src/repositories/CouponRepository.cpp
    src/repositories/OrderRepository.cpp
    src/services/AuthService.cpp
    src/services/ProductService.cpp
    src/services/CartService.cpp
    src/services/CheckoutService.cpp
    src/services/InventoryService.cpp
    src/services/SalesReportService.cpp
    src/ui/MainMenu.cpp
    src/ui/ProductManagementUI.cpp
    src/ui/InventoryManagementUI.cpp
    src/ui/SalesReportUI.cpp
)

if(MINGW)
    target_link_options(cashier PRIVATE -static-libgcc -static-libstdc++)
endif()

# Tests
enable_testing()
add_subdirectory(tests)
