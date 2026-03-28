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
