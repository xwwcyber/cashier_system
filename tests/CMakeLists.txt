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
