
set (TEST_LIBRARIES ${GLIB2_LIBRARIES}
                    ${HYSCAN_DRIVER_LIBRARY})

add_definitions (-DDUMMY_DRIVER_PREFIX="dummy")
add_definitions (-DDUMMY_DRIVER_NUMBER=4)

add_executable (device-schema-test device-schema-test.c)
add_executable (driver-test driver-test.c)
add_executable (uart-test uart-test.c)
add_library (hyscan-dummy0 SHARED hyscan-dummy-discover.c)
add_library (hyscan-dummy1 SHARED dummy-driver.c)
add_library (hyscan-dummy2 SHARED dummy-driver.c)
add_library (hyscan-dummy3 SHARED dummy-driver.c)
add_library (hyscan-dummy4 SHARED dummy-driver.c)

target_link_libraries (device-schema-test ${TEST_LIBRARIES})
target_link_libraries (driver-test ${TEST_LIBRARIES})
target_link_libraries (uart-test ${TEST_LIBRARIES})
target_link_libraries (hyscan-dummy0 ${TEST_LIBRARIES})
target_link_libraries (hyscan-dummy1 ${TEST_LIBRARIES} hyscan-dummy0)
target_link_libraries (hyscan-dummy2 ${TEST_LIBRARIES} hyscan-dummy0)
target_link_libraries (hyscan-dummy3 ${TEST_LIBRARIES} hyscan-dummy0)
target_link_libraries (hyscan-dummy4 ${TEST_LIBRARIES} hyscan-dummy0)

set_target_properties (hyscan-dummy0 PROPERTIES DEFINE_SYMBOL "HYSCAN_API_EXPORTS")
set_target_properties (hyscan-dummy0 PROPERTIES PREFIX "")
set_target_properties (hyscan-dummy1 PROPERTIES PREFIX "")
set_target_properties (hyscan-dummy2 PROPERTIES PREFIX "")
set_target_properties (hyscan-dummy3 PROPERTIES PREFIX "")
set_target_properties (hyscan-dummy4 PROPERTIES PREFIX "")
set_target_properties (hyscan-dummy1 PROPERTIES SUFFIX ".drv")
set_target_properties (hyscan-dummy2 PROPERTIES SUFFIX ".drv")
set_target_properties (hyscan-dummy3 PROPERTIES SUFFIX ".drv")
set_target_properties (hyscan-dummy4 PROPERTIES SUFFIX ".drv")

target_compile_definitions (hyscan-dummy1 PRIVATE "-DDUMMY=1")
target_compile_definitions (hyscan-dummy2 PRIVATE "-DDUMMY=2")
target_compile_definitions (hyscan-dummy3 PRIVATE "-DDUMMY=3")
target_compile_definitions (hyscan-dummy4 PRIVATE "-DDUMMY=4")

add_test (NAME DeviceSchemaTest COMMAND device-schema-test
          WORKING_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
add_test (NAME DriverTest COMMAND driver-test .
          WORKING_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")

install (TARGETS device-schema-test
                 driver-test
         COMPONENT test
         RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
         PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)

install (TARGETS hyscan-dummy0
         COMPONENT test
         RUNTIME DESTINATION "${HYSCAN_INSTALL_DRVDIR}"
         LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
         PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ)

install (TARGETS hyscan-dummy1
                 hyscan-dummy2
                 hyscan-dummy3
                 hyscan-dummy4
         COMPONENT test
         RUNTIME DESTINATION "${HYSCAN_INSTALL_DRVDIR}"
         LIBRARY DESTINATION "${HYSCAN_INSTALL_DRVDIR}"
         PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ)
