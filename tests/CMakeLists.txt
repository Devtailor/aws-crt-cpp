include(AwsTestHarness)
enable_testing()
include(CTest)

file(GLOB TEST_SRC "*.cpp")
file(GLOB TEST_HDRS "*.h")
file(GLOB TESTS ${TEST_HDRS} ${TEST_SRC})

set(TEST_BINARY_NAME ${CMAKE_PROJECT_NAME}-tests)

add_test_case(EventLoopResourceSafety)
add_test_case(MqttClientResourceSafety)
add_test_case(ClientBootstrapResourceSafety)
add_test_case(TLSContextResourceSafety)
add_test_case(DateTimeBinding)
add_test_case(BasicJsonParsing)
add_test_case(SHA256ResourceSafety)
add_test_case(MD5ResourceSafety)
add_test_case(SHA256HMACResourceSafety)
add_test_case(HttpDownloadNoBackPressure)
add_test_case(IotPublishSubscribe)
add_test_case(HttpClientConnectionManagerResourceSafety)
add_test_case(HttpClientConnectionWithPendingAcquisitions)
add_test_case(HttpClientConnectionWithPendingAcquisitionsAndClosedConnections)
add_test_case(DefaultResolution)

generate_cpp_test_driver(${TEST_BINARY_NAME})

add_custom_command(TARGET ${TEST_BINARY_NAME} PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/resources ${CMAKE_CURRENT_BINARY_DIR})
