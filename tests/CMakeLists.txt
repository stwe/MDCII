enable_testing()

include(../conanbuildinfo.cmake)
conan_basic_setup()

add_executable(MDCII_TEST Tests.cpp)

target_include_directories(MDCII_TEST PUBLIC ../../MDCII/src)
target_link_libraries(MDCII_TEST ${CONAN_LIBS})

add_test(TestSuite MDCII_TEST)
