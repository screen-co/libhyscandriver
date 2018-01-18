
set (TEST_LIBRARIES ${GLIB2_LIBRARIES}
                    ${LIBXML2_LIBRARIES}
                    ${HYSCAN_DRIVER_LIBRARY})

add_definitions (-DDUMMY_DRIVER_PREFIX="dummy")
add_definitions (-DDUMMY_DRIVER_NUMBER=4)

add_executable (driver-test driver-test.c hyscan-dummy-discover.c)
add_library (hyscan-dummy1-drv SHARED dummy-driver.c)
add_library (hyscan-dummy2-drv SHARED dummy-driver.c)
add_library (hyscan-dummy3-drv SHARED dummy-driver.c)
add_library (hyscan-dummy4-drv SHARED dummy-driver.c)

target_link_libraries (driver-test ${TEST_LIBRARIES})
target_link_libraries (hyscan-dummy1-drv ${TEST_LIBRARIES})
target_link_libraries (hyscan-dummy2-drv ${TEST_LIBRARIES})
target_link_libraries (hyscan-dummy3-drv ${TEST_LIBRARIES})
target_link_libraries (hyscan-dummy4-drv ${TEST_LIBRARIES})

set_target_properties (hyscan-dummy1-drv PROPERTIES PREFIX "")
set_target_properties (hyscan-dummy2-drv PROPERTIES PREFIX "")
set_target_properties (hyscan-dummy3-drv PROPERTIES PREFIX "")
set_target_properties (hyscan-dummy4-drv PROPERTIES PREFIX "")

target_compile_definitions (hyscan-dummy1-drv PRIVATE "-DDUMMY=1")
target_compile_definitions (hyscan-dummy2-drv PRIVATE "-DDUMMY=2")
target_compile_definitions (hyscan-dummy3-drv PRIVATE "-DDUMMY=3")
target_compile_definitions (hyscan-dummy4-drv PRIVATE "-DDUMMY=4")
