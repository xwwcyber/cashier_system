add_executable(
    product_repository_tests
    ProductRepositoryTests.cpp
    ${CMAKE_SOURCE_DIR}/src/repositories/ProductRepository.cpp
)

target_include_directories(product_repository_tests PRIVATE ${CMAKE_SOURCE_DIR}/include)

if(MINGW)
    target_link_options(product_repository_tests PRIVATE -static-libgcc -static-libstdc++)
endif()

add_test(NAME product_repository_tests COMMAND product_repository_tests)

add_executable(
    employee_repository_tests
    EmployeeRepositoryTests.cpp
    ${CMAKE_SOURCE_DIR}/src/repositories/EmployeeRepository.cpp
)

target_include_directories(employee_repository_tests PRIVATE ${CMAKE_SOURCE_DIR}/include)

if(MINGW)
    target_link_options(employee_repository_tests PRIVATE -static-libgcc -static-libstdc++)
endif()

add_test(NAME employee_repository_tests COMMAND employee_repository_tests)

add_executable(
    coupon_repository_tests
    CouponRepositoryTests.cpp
    ${CMAKE_SOURCE_DIR}/src/repositories/CouponRepository.cpp
)

target_include_directories(coupon_repository_tests PRIVATE ${CMAKE_SOURCE_DIR}/include)

if(MINGW)
    target_link_options(coupon_repository_tests PRIVATE -static-libgcc -static-libstdc++)
endif()

add_test(NAME coupon_repository_tests COMMAND coupon_repository_tests)

add_executable(
    order_repository_tests
    OrderRepositoryTests.cpp
    ${CMAKE_SOURCE_DIR}/src/repositories/OrderRepository.cpp
)

target_include_directories(order_repository_tests PRIVATE ${CMAKE_SOURCE_DIR}/include)

if(MINGW)
    target_link_options(order_repository_tests PRIVATE -static-libgcc -static-libstdc++)
endif()

add_test(NAME order_repository_tests COMMAND order_repository_tests)

add_executable(
    auth_service_tests
    AuthServiceTests.cpp
    ${CMAKE_SOURCE_DIR}/src/repositories/EmployeeRepository.cpp
    ${CMAKE_SOURCE_DIR}/src/services/AuthService.cpp
)

target_include_directories(auth_service_tests PRIVATE ${CMAKE_SOURCE_DIR}/include)

if(MINGW)
    target_link_options(auth_service_tests PRIVATE -static-libgcc -static-libstdc++)
endif()

add_test(NAME auth_service_tests COMMAND auth_service_tests)

add_executable(
    product_service_tests
    ProductServiceTests.cpp
    ${CMAKE_SOURCE_DIR}/src/repositories/ProductRepository.cpp
    ${CMAKE_SOURCE_DIR}/src/services/ProductService.cpp
)

target_include_directories(product_service_tests PRIVATE ${CMAKE_SOURCE_DIR}/include)

if(MINGW)
    target_link_options(product_service_tests PRIVATE -static-libgcc -static-libstdc++)
endif()

add_test(NAME product_service_tests COMMAND product_service_tests)

add_executable(
    cart_service_tests
    CartServiceTests.cpp
    ${CMAKE_SOURCE_DIR}/src/repositories/ProductRepository.cpp
    ${CMAKE_SOURCE_DIR}/src/services/ProductService.cpp
    ${CMAKE_SOURCE_DIR}/src/services/CartService.cpp
)

target_include_directories(cart_service_tests PRIVATE ${CMAKE_SOURCE_DIR}/include)

if(MINGW)
    target_link_options(cart_service_tests PRIVATE -static-libgcc -static-libstdc++)
endif()

add_test(NAME cart_service_tests COMMAND cart_service_tests)

add_executable(
    checkout_service_tests
    CheckoutServiceTests.cpp
    ${CMAKE_SOURCE_DIR}/src/repositories/ProductRepository.cpp
    ${CMAKE_SOURCE_DIR}/src/repositories/CouponRepository.cpp
    ${CMAKE_SOURCE_DIR}/src/repositories/OrderRepository.cpp
    ${CMAKE_SOURCE_DIR}/src/services/ProductService.cpp
    ${CMAKE_SOURCE_DIR}/src/services/CheckoutService.cpp
)

target_include_directories(checkout_service_tests PRIVATE ${CMAKE_SOURCE_DIR}/include)

if(MINGW)
    target_link_options(checkout_service_tests PRIVATE -static-libgcc -static-libstdc++)
endif()

add_test(NAME checkout_service_tests COMMAND checkout_service_tests)

add_executable(
    inventory_service_tests
    InventoryServiceTests.cpp
    ${CMAKE_SOURCE_DIR}/src/repositories/ProductRepository.cpp
    ${CMAKE_SOURCE_DIR}/src/services/InventoryService.cpp
)

target_include_directories(inventory_service_tests PRIVATE ${CMAKE_SOURCE_DIR}/include)

if(MINGW)
    target_link_options(inventory_service_tests PRIVATE -static-libgcc -static-libstdc++)
endif()

add_test(NAME inventory_service_tests COMMAND inventory_service_tests)
