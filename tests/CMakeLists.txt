enable_testing()

find_package(GTest REQUIRED)

add_executable(MDCII_TEST ZoomTests.cpp)

target_link_libraries(MDCII_TEST GTest::gtest_main)

add_test(ZoomTestSuite MDCII_TEST)
